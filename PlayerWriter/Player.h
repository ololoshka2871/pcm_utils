#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "LockingQueue.h"

#include "portaudio.h"

struct Player {
  struct audioContainer {
    static constexpr auto samples_pre_frame = 2;

    enum CMD { C_EOF = -1, C_MUTE = -2 };

    audioContainer() : data{}, offset{0} {}

    void update_volume(double &volume) {
      if (volume < 0) {
        volume = 0;
      }
      if (volume > 1.0) {
        volume = 1.0;
      }
    }

    audioContainer(const float samples[], uint32_t samples_count, double volume)
        : data{&samples[0], &samples[samples_count]}, offset{0} {
      update_volume(volume);

      if (volume != 1.0) {
        std::for_each(data.begin(), data.end(),
                      [volume](float &element) { element *= volume; });
      }
    }

    audioContainer(const int16_t samples[], uint32_t samples_count,
                   double volume)
        : data{&samples[0], &samples[samples_count]}, offset{0} {
      update_volume(volume);

      std::for_each(data.begin(), data.end(), [volume](float &element) {
        element = element / (0xFFFF >> 1) * volume;

        assert(element >= -1.0f && element <= 1.0f);
      });
    }

    size_t avalable() const { return offset >= 0 ? data.size() - offset : 0; }

    size_t read_to(void *ptr, size_t count);

    static audioContainer eof() {
      audioContainer res;
      res.offset = C_EOF;
      return res;
    }

    static audioContainer mute() {
      audioContainer res;
      res.offset = C_MUTE;
      return res;
    }

    static audioContainer zero(int32_t zero_frame_count) {
      return audioContainer(zero_frame_count * samples_pre_frame);
    }

    bool is_end() const { return offset == C_EOF; }
    bool is_mute() const { return offset == C_MUTE; }

  private:
    audioContainer(int32_t zero_frame_count)
        : data(size_t(zero_frame_count)), offset{0} {}

    std::vector<float> data;
    int32_t offset;
  };

  Player(int32_t outpul_index, uint32_t buf_size, int32_t sample_rate);
  ~Player();

  template <typename T>
  void play(T samples[], uint32_t samples_count, double volume = 1.0,
            int32_t prefill_zeros = 0) {
    if (prefill_zeros > 0) {
      playQueue.push(audioContainer::zero(prefill_zeros));
    }
    playQueue.push(audioContainer{samples, samples_count, volume});
  }
  void Mute();

private:
  PaStream *stream;
  LockingQueue<audioContainer> playQueue;
  semaphore end_sem;

  std::unique_ptr<audioContainer> currentelement;

  int callback(const void *input, void *output, unsigned long frameCount,
               const PaStreamCallbackTimeInfo *timeInfo,
               PaStreamCallbackFlags statusFlags);
};

#ifdef _MSC_VER
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

extern "C" DLLEXPORT void initPlayerContext(int32_t outpul_index,
                                            int32_t sample_rate,
                                            int32_t buf_size, void **ctx,
                                            int *errCode);
extern "C" DLLEXPORT void releasePlayerContext(void **ctx);
extern "C" DLLEXPORT void Play(void **ctx, int16_t samples[],
                               uint32_t samples_count);
extern "C" DLLEXPORT void PlayVol(void **ctx, int16_t samples[],
                                  uint32_t samples_count, double volume,
                                  int32_t prefil_zeros);
extern "C" DLLEXPORT void Mute(void **ctx);

#ifndef STC007BINARIZER_H
#define STC007BINARIZER_H

#include <array>
#include <thread>
#include <stdint.h>
#include <string>
#include <vector>

#include "config.h"
#include "stc007line.h"
#include "videoline.h"

#if 0

#include <QElapsedTimer>
#include <QDebug>

#endif

//#define LB_EN_DBG_OUT       1         // Enable debug console output.
//#define LB_EN_PIXEL_DBG     1         // Enable debug checks for pixel coordinates access.
//#define LB_ROUNDED_DIVS     1         // Enable rounded integer divisions in [findVideoPixel()].

// "Digitizer preset" structure.
typedef struct
{
    uint8_t digpre_max_black_lvl;       // Setting for starting point of black level forwards search.
    uint8_t digpre_min_white_lvl;       // Setting for ending point of white level backwards search.
    uint8_t digpre_min_contrast;        // Setting for minimum difference between "black" and "white".
    uint8_t digpre_max_black_dist;      // Setting for maximum delta between detected black level and stop of back level forwards search.
    uint8_t digpre_max_white_dist;      // Setting for maximum delta between detected white level and stop of white level backwards search.
    uint8_t digpre_min_ref_lvl;         // Setting for minimum brightness threshold.
    uint8_t digpre_max_ref_lvl;         // Setting for maximum brightness threshold.
    uint8_t digpre_mark_max_dist;       // Setting for percent of line from each end of the line to search for markers.
    uint8_t digpre_max_threads;         // Setting for maximum threads used for reference level sweeping.
} LineBinPreset;

class STC007Binarizer
{
public:
    // Results of [processLine()].
    enum
    {
        LB_RET_NULL_VIDEO,      // Null poiner to input video line was provided, unable to process.
        LB_RET_NULL_STC,        // Null poiner to output PCM line was provided, unable to process.
        LB_RET_SHORT_LINE,      // Provided video line is too short to have PCM STC-007 data.
        LB_RET_OK               // Video line was successfully converted to PCM line.
    };

    // Console logging options (can be used simultaneously).
    enum
    {
        LOG_SETTINGS = (1<<0),  // External operations with settings.
        LOG_PROCESS = (1<<1),   // General stage-by-stage logging.
        LOG_VIDEO = (1<<2),     // Output pixel coordinates and string of binarized data.
        LOG_BRIGHT = (1<<3),    // Output brightness spread data.
        LOG_SWEEP = (1<<4)      // Output full process of reference level sweeping.
    };

    // Stages of main state machine.
    enum
    {
        STG_INPUT_ALL,          // Try to get valid PCM data with provided reference level and data coordinates.
        STG_INPUT_LEVEL,        // Try to get valid PCM data with provided reference level (search for data coordinates).
        STG_REF_FIND,           // No external data provided, or no valid PCM data was found with provided data, find reference level and data coordinates.
        STG_REFSCAN_RUN,        // Full reference level sweep mode.
        STG_READ_PCM,           // Perform final binarization, get PCM data with provided or found parameters.
        STG_DATA_OK,            // Exit-state, valid PCM data (CRC OK) was detected in the line.
        STG_NO_GOOD,            // Exit-state, no valid PCM data was detected (no PCM markers or bad CRC).
        STG_MAX
    };

    // Results of [readPCMdata()].
    enum
    {
        REF_NO_PCM = 1,         // No PCM markers were found.
        REF_BAD_CRC = 2,        // PCM markers were found, but CRC was bad.
        REF_CRC_OK = 4          // Valid PCM data found, CRC ok.
    };

public:
    STC007Binarizer();
    void setLogLevel(uint8_t);
    void setPixelShiftRetry(bool flag = true);
    void setRefLevelSweep(bool flag = true);
    void setReferenceLevel(uint8_t in_ref = 0);
    void setDataCoordinates(uint16_t in_data_start = 0, uint16_t in_data_stop = 0);
    void setMaxThreads(uint8_t in_threads = 0);
    uint8_t processLine(VideoLine *in_video_line = NULL, STC007Line *out_stc007_line = NULL);

private:
    uint8_t getVideoPixel(uint16_t x_coord);
    void findPCMmarkers(STC007Line *pcm_line, uint8_t thread_id = 0, bool no_log = false);
    void findBlackWhite(STC007Line *pcm_line);
    void calcRefLevel();
    void sweepRefLevel(std::array<uint16_t, 256> *result, uint8_t low_lvl, uint8_t high_lvl, uint8_t thread_id);
    void fillDataWords(STC007Line *pcm_line, uint8_t shift_dir = 0, bool no_log = false);
    uint8_t readPCMdata(STC007Line *pcm_line, uint8_t thread_id = 0, bool no_log = false);

private:
    LineBinPreset digi_set;             // "Digitizer preset" ("digpre_") settings.
    VideoLine *video_line;              // Pointer to input VideoLine.
    uint8_t log_level;                  // Setting for debugging log level.
    uint8_t in_def_reference;           // External default reference level.
    uint16_t in_def_data_start;         // External default data start pixel coordinate.
    uint16_t in_def_data_stop;          // External default data stop pixel coordinate.
    uint8_t max_threads;                // Maximum available computing threads.
    bool do_pixel_shift;                // Setting for shifting pixel offset and repeating binarization if CRC fails.
    bool do_ref_level_sweep;            // Setting for performing full reference level sweep for determining best value.

    uint8_t proc_state;                 // State of processing.
    uint16_t line_length;               // Length of current line (in pixels).
    uint8_t br_black;                   // Brightness level for "BLACK".
    uint8_t br_white;                   // Brightness level for "WHITE".
    uint8_t ref_level;                  // Reference brightness level for comparator threshold.
    uint16_t mark_start_max;            // Maximum pixel coordinate of the START marker search area.
    uint16_t mark_end_min;              // Minimum pixel coordinate of the STOP marker search area.
    uint16_t min_mark_len;              // Minimum length of the "1111" STOP bits in pixels.
    bool ref_sweep_en;                  // Internal flag for enabled reference level sweep.
    bool black_level_detected;          // Was BLACK level in the line detected or not.
    bool white_level_detected;          // Was WHITE level in the line detected or not.
    std::array<uint16_t, 256> spread_data;  // Data spread statistics.
};

#endif // STC007BINARIZER_H

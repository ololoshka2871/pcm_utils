#ifndef VIDEOLINE_H
#define VIDEOLINE_H

#include <stdint.h>
#include <vector>

class VideoLine
{
public:
    // Modes of interlace processing for the line.
    enum
    {
        DEINT_OFF,          // Deintarlace disabled.
        DEINT_TFF,          // Deintarlace enabled, Top Field First.
        DEINT_BFF           // Deintarlace enabled, Bottom Field First.
    };

public:
    VideoLine();
    VideoLine(uint16_t line_number, uint16_t frame_number, 
        uint8_t* pixel_data, size_t pixel_count, uint8_t deint_mode);
    VideoLine(const VideoLine &);
    VideoLine& operator= (const VideoLine &);

    void setLength(uint16_t in_length);
    void setBrighness(uint16_t pixel_index, uint8_t data);
    void setEmpty(bool flag);
    void setSkip(bool flag);
    bool isEmpty();
    bool doSkip();

public:
    uint16_t line_number;               // Number of line in the frame (1=top)
    uint16_t frame_number;              // Number of frame with this line
    std::vector<uint8_t> pixel_data;    // Array of grayscale 8-bit pixels
    uint8_t deint_mode;                 // Mode of deinterlacing that was applied to the frame

private:
    bool empty;                         // Was the line marked as "empty" (no items in [pixel_data]) for fast skipping
    bool skip;                          // Was the line marker as "do not use"
};

#endif // VIDEOLINE_H

#include "pch.h"

#include "videoline.h"

VideoLine::VideoLine()
{
    line_number = 0;
    frame_number = 0;
    pixel_data.reserve(1024);
    deint_mode = DEINT_OFF;
    empty = false;
    skip = false;
}

VideoLine::VideoLine(uint16_t line_number, uint16_t frame_number, uint8_t* pixel_data, size_t pixel_count, uint8_t deint_mode)
    : line_number(line_number), frame_number(frame_number), pixel_data{pixel_data, pixel_data + pixel_count}, deint_mode(deint_mode), 
    empty(false), skip(false) {}

VideoLine::VideoLine(const VideoLine &in_object)
{
    line_number = in_object.line_number;
    frame_number = in_object.frame_number;
    pixel_data = in_object.pixel_data;
    deint_mode = in_object.deint_mode;
    empty = in_object.empty;
    skip = in_object.skip;
}

VideoLine& VideoLine::operator= (const VideoLine &in_object)
{
    if(this==&in_object) return *this;

    line_number = in_object.line_number;
    frame_number = in_object.frame_number;
    pixel_data = in_object.pixel_data;
    deint_mode = in_object.deint_mode;
    empty = in_object.empty;
    skip = in_object.skip;

    return *this;
}

//------------------------ Set length of the line (in pixels).
void VideoLine::setLength(uint16_t in_length)
{
    pixel_data.resize(in_length);
}

//------------------------ Set pixel brightness.
void VideoLine::setBrighness(uint16_t pixel_index, uint8_t data)
{
    if(pixel_index<pixel_data.size()) pixel_data.at(pixel_index) = data;
}

//------------------------ Set flag "line is empty" for fast skipping for padded lines.
void VideoLine::setEmpty(bool flag)
{
    empty = flag;
}

//------------------------ Set flag "do not use" for trimmed lines.
void VideoLine::setSkip(bool flag)
{
    skip = flag;
}

//------------------------ Check if line is set to "empty".
bool VideoLine::isEmpty()
{
    return empty;
}

//------------------------ Check if line processing should be skipped.
bool VideoLine::doSkip()
{
    return skip;
}

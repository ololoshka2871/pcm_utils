#include "stc007line.h"

// Table for 0x1021 polynome CRC16.
static const uint16_t CRC_1021_TABLE[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

STC007Line::STC007Line()
{
    pixel_start_offset = 0;
    pixel_size_mult = INT_CALC_MULT;
    halfpixel_size_mult = pixel_size_mult/INT_CALC_PIXEL_DIV;

    uint8_t i;
    for(i=0;i<=PCM_CRCC_SH0;i++) words[i] = 0;
    for(i=0;i<16;i++) bytes[i] = 0;
    calc_crc = ~words[PCM_CRCC_SH0];

    frame_number = 0;
    line_number = 0;
    deint_mode = DEINT_OFF;
    ref_level = 0;
    mark_st_stage = MARK_ST_START;
    mark_ed_stage = MARK_ED_START;
    marker_start_coord = 0;
    data_start_coord = 0;
    data_stop_coord = 0;
    marker_stop_coord = 0;
    process_time = 0;
    shift_stage = PIX_NO_SHIFT;
    skip_line = false;
    ref_level_sweeped = false;
    data_by_ext_tune = false;
}

STC007Line::STC007Line(const STC007Line &in_object)
{
    for(uint8_t index=0;index<PCM_MAX_SH;index++)
    {
        words[index] = in_object.words[index];
    }
    for(uint8_t index=0;index<16;index++)
    {
        bytes[index] = in_object.bytes[index];
    }
    frame_number = in_object.frame_number;
    line_number = in_object.line_number;
    deint_mode = in_object.deint_mode;
    ref_level = in_object.ref_level;
    mark_st_stage = in_object.mark_st_stage;
    mark_ed_stage = in_object.mark_ed_stage;
    marker_start_coord = in_object.marker_start_coord;
    data_start_coord = in_object.data_start_coord;
    data_stop_coord = in_object.data_stop_coord;
    marker_stop_coord = in_object.marker_stop_coord;
    process_time = in_object.process_time;
    shift_stage = in_object.shift_stage;
    skip_line = in_object.skip_line;
    ref_level_sweeped = in_object.ref_level_sweeped;
    data_by_ext_tune = in_object.data_by_ext_tune;

    calc_crc = in_object.calc_crc;
    pixel_start_offset = in_object.pixel_start_offset;
    pixel_size_mult = in_object.pixel_size_mult;
    halfpixel_size_mult = in_object.halfpixel_size_mult;
}

STC007Line& STC007Line::operator= (const STC007Line &in_object)
{
    if(this==&in_object) return *this;

    for(uint8_t index=0;index<PCM_MAX_SH;index++)
    {
        words[index] = in_object.words[index];
    }
    for(uint8_t index=0;index<16;index++)
    {
        bytes[index] = in_object.bytes[index];
    }
    frame_number = in_object.frame_number;
    line_number = in_object.line_number;
    deint_mode = in_object.deint_mode;
    ref_level = in_object.ref_level;
    mark_st_stage = in_object.mark_st_stage;
    mark_ed_stage = in_object.mark_ed_stage;
    marker_start_coord = in_object.marker_start_coord;
    data_start_coord = in_object.data_start_coord;
    data_stop_coord = in_object.data_stop_coord;
    marker_stop_coord = in_object.marker_stop_coord;
    process_time = in_object.process_time;
    shift_stage = in_object.shift_stage;
    skip_line = in_object.skip_line;
    ref_level_sweeped = in_object.ref_level_sweeped;
    data_by_ext_tune = in_object.data_by_ext_tune;

    calc_crc = in_object.calc_crc;
    pixel_start_offset = in_object.pixel_start_offset;
    pixel_size_mult = in_object.pixel_size_mult;
    halfpixel_size_mult = in_object.halfpixel_size_mult;

    return *this;
}

bool STC007Line::operator!= (const STC007Line &in_object)
{
    if(frame_number!=in_object.frame_number)
    {
        return false;
    }
    else if(line_number!=in_object.line_number)
    {
        return false;
    }
    else
    {
        return true;
    }
}

//------------------------ Calculate and store int-multiplied PPB (pixel-per-bit) value (from 132 bits span).
void STC007Line::updatePPB()
{
    // Pixel = bit * PPB.
    pixel_size_mult = data_stop_coord-data_start_coord;
    pixel_size_mult = (pixel_size_mult*INT_CALC_MULT + 132/2)/132;
    pixel_start_offset = data_start_coord;
    // Calculate half of the pixel for bit center detection.
    halfpixel_size_mult = (pixel_size_mult + INT_CALC_PIXEL_DIV/2)/INT_CALC_PIXEL_DIV;
}

//------------------------ Find coordinate of video pixel in line for requested PCM bit (0...128).
uint16_t STC007Line::findVideoPixel(uint16_t line_length, uint8_t pcm_bit, uint8_t shift)
{
    uint32_t video_pixel;
    // Limit bit position.
    if(pcm_bit>128)
    {
        pcm_bit = 128;
    }
    // Add in 3 START bits.
    pcm_bit = pcm_bit+2;
    // Pixel = bit * pixels-per-bit (PPB).
    video_pixel = ((pcm_bit*pixel_size_mult)+halfpixel_size_mult);
#ifdef LB_ROUNDED_DIVS
    video_pixel = (video_pixel + INT_CALC_MULT/2)/INT_CALC_MULT;
#else
    video_pixel = video_pixel/INT_CALC_MULT;
#endif
    video_pixel = video_pixel+pixel_start_offset;
    // Check for negative pixel shift.
    if(shift==PIX_SHIFT_NEG)
    {
        // Prevent overflow if required.
        if(video_pixel!=0) video_pixel--;
    }
    // Check for positive pixel shift.
    else if(shift==PIX_SHIFT_POS)
    {
        video_pixel++;
    }
    // Check pixel coordinate limits.
    if(video_pixel>=line_length)
    {
        // Correct if out of bounds.
        video_pixel = (line_length-1);
    }
    return (uint16_t)video_pixel;
}

//------------------------ Re-calculate CRCC for all words in the line.
void STC007Line::calcCRC()
{
    CRC16_init(&calc_crc);
    for(uint8_t i=0;i<14;i++)
    {
        calc_crc = getTableCRC16(calc_crc, bytes[i]);
    }
#ifdef STC_LINE_EN_DBG_OUT
    QString log_line;
    log_line.sprintf("[STCL] Calculating CRC... %04x", calc_crc);
    qInfo()<<log_line;
#endif
}

//------------------------ Was PCM START-marker found in the line?
bool STC007Line::hasStartMarker()
{
    if(mark_st_stage==MARK_ST_BOT_2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------ Was PCM STOP-marker found in the line?
bool STC007Line::hasStopMarker()
{
    if(mark_ed_stage==MARK_ED_LEN_OK)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------ Were both PCM markers found in the line?
bool STC007Line::hasMarkers()
{
    if((mark_st_stage==MARK_ST_BOT_2)&&(mark_ed_stage==MARK_ED_LEN_OK))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------ Was re-calculated CRC the same as read one?
bool STC007Line::isCRCvalid()
{
    if(calc_crc==words[PCM_CRCC_SH0])
    {
#ifdef STC_LINE_EN_DBG_OUT
    QString log_line;
    log_line.sprintf("[STCL] Read CRC: %04x, calc CRC: %04x, [Valid CRC]", words[PCM_CRCC_SH0], calc_crc);
    qInfo()<<log_line;
#endif
        return true;
    }
    else
    {
#ifdef STC_LINE_EN_DBG_OUT
    QString log_line;
    log_line.sprintf("[STCL] Read CRC: %04x, calc CRC: %04x [Bad CRC!]", words[PCM_CRCC_SH0], calc_crc);
    qInfo()<<log_line;
#endif
        return false;
    }
}

//------------------------ Does this line contain control signal block (and no audio data)?
bool STC007Line::isControlBlock()
{
    if((words[PCM_L_SH0]==0x3333)&&(words[PCM_R_SH48]==0x0CCC)
        &&(words[PCM_L_SH95]==0x3333)&&(words[PCM_R_SH143]==0x0CCC)
        &&((words[PCM_Q_SH336]&0x3FF0)==0))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------ Prohibition of digital dubbing.
bool STC007Line::isCtrlCopyProhibit()
{
    if((words[PCM_Q_SH336]&CTRL_COPY_MASK)==0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

//------------------------ Presence of P-word.
bool STC007Line::isCtrlEnabledP()
{
    if((words[PCM_Q_SH336]&CTRL_EN_P_MASK)==0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------ Presence of Q-word (14/16-bit mode).
bool STC007Line::isCtrlEnabledQ()
{
    if((words[PCM_Q_SH336]&CTRL_EN_Q_MASK)==0)
    {
        return true;    // 14-bit mode
    }
    else
    {
        return false;   // 16-bit mode (PCM-F1 format)
    }
}

//------------------------ Pre-emphasis of audio.
bool STC007Line::isCtrlEnabledEmphasis()
{
    if((words[PCM_Q_SH336]&CTRL_EMPH_MASK)==0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------ Was PCM data decoded with use of reference level sweep?
bool STC007Line::isDataBySweep()
{
    if(ref_level_sweeped==true)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------ Was PCM data decoded with external parameters from previous lines?
bool STC007Line::isDataBySkip()
{
    if(data_by_ext_tune==true)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------ Check if line processing should be skipped.
bool STC007Line::doSkip()
{
    return skip_line;
}

//------------------------ Get PPB (pixep per bit) integer value.
uint8_t STC007Line::getPPB()
{
    return pixel_size_mult/INT_CALC_MULT;
}

//------------------------ Get PPB (pixep per bit) fraction value (integer after point).
uint8_t STC007Line::getPPBfrac()
{
    return pixel_size_mult%INT_CALC_MULT;
}

//------------------------ Get re-calculated CRC for the data.
uint16_t STC007Line::getCalculatedCRC()
{
    return calc_crc;
}

//------------------------ Convert PCM data to string for debug.
std::string STC007Line::toString()
{
    std::string text_out;
    uint8_t ind, bit_pos;
    // Printing header.
    if(hasStartMarker()==true)
    {
        text_out += PCM_ONE;
        text_out += PCM_ZERO;
        text_out += PCM_ONE;
        text_out += PCM_ZERO;
    }
    else
    {
        text_out += PCM_ZERO;
        text_out += PCM_ZERO;
        text_out += PCM_ZERO;
        text_out += PCM_ZERO;
    }

    // Printing 14 bit words data.
    for(ind=0;ind<8;ind++)
    {
        bit_pos = 14;
        do
        {
            bit_pos--;
            if((words[ind]&(1<<bit_pos))==0)
            {
                text_out += PCM_ZERO;
            }
            else
            {
                text_out += PCM_ONE;
            }
        }
        while(bit_pos>0);
    }
    // Printing 16 bit CRCC data.
    bit_pos = 16;
    do
    {
        bit_pos--;
        if((words[PCM_CRCC_SH0]&(1<<bit_pos))==0)
        {
            text_out += PCM_ZERO;
        }
        else
        {
            text_out += PCM_ONE;
        }
    }
    while(bit_pos>0);
    // Printing footer.
    if(hasStopMarker()==true)
    {
        text_out += PCM_ZERO;
        text_out += PCM_ONE;
        text_out += PCM_ONE;
        text_out += PCM_ONE;
        text_out += PCM_ONE;
    }
    else
    {
        text_out += PCM_ZERO;
        text_out += PCM_ZERO;
        text_out += PCM_ZERO;
        text_out += PCM_ZERO;
        text_out += PCM_ZERO;
    }

    return text_out;
}

//------------------------ Output full information about the line.
std::string STC007Line::textDump()
{
    std::string text_out;
    char c_buf[32];

    if(deint_mode==DEINT_TFF)
    {
        sprintf(c_buf, "F<%03u< L[%03u] B", (unsigned int)frame_number, line_number);
    }
    else if(deint_mode==DEINT_BFF)
    {
        sprintf(c_buf, "F>%03u> L[%03u] B", (unsigned int)frame_number, line_number);
    }
    else
    {
        sprintf(c_buf, "F[%03u] L[%03u] B", (unsigned int)frame_number, line_number);
    }
    text_out += c_buf;

    if(isDataBySweep()==true)
    {
        sprintf(c_buf, ">[%03u] S", ref_level);
    }
    else if(isDataBySkip()==true)
    {
        sprintf(c_buf, "=[%03u] S", ref_level);
    }
    else
    {
        sprintf(c_buf, "?[%03u] S", ref_level);
    }
    text_out += c_buf;
    if(hasStartMarker()==true)
    {
        text_out += "+";
    }
    else
    {
        sprintf(c_buf, "%01u", mark_st_stage);
        text_out += c_buf;
    }
    if((marker_start_coord==0)&&(hasMarkers()==true))
    {
        sprintf(c_buf, "[%02u|%02u] ", marker_start_coord, data_start_coord);
    }
    else
    {
        sprintf(c_buf, "[%02u:%02u] ", marker_start_coord, data_start_coord);
    }
    text_out += c_buf;
    text_out += toString()+" E";
    if(hasStopMarker()==true)
    {
        text_out += "+";
    }
    else
    {
        sprintf(c_buf, "%01u", mark_ed_stage);
        text_out += c_buf;
    }
    if((marker_stop_coord==0)&&(hasMarkers()==true))
    {
        sprintf(c_buf, "[%03u|%03u] ", data_stop_coord, marker_stop_coord);
    }
    else
    {
        sprintf(c_buf, "[%03u:%03u] ", data_stop_coord, marker_stop_coord);
    }
    text_out += c_buf;
    if(shift_stage==PIX_SHIFT_POS)
    {
        sprintf(c_buf, "PPB>%u.%02u> ", getPPB(), getPPBfrac());
    }
    else if(shift_stage==PIX_SHIFT_NEG)
    {
        sprintf(c_buf, "PPB<%u.%02u< ", getPPB(), getPPBfrac());
    }
    else
    {
        sprintf(c_buf, "PPB[%u.%02u] ", getPPB(), getPPBfrac());
    }
    text_out += c_buf;
    sprintf(c_buf, "T[%04u] ", process_time);
    text_out += c_buf;
    if(doSkip()==true)
    {
        text_out += "SKIP!  ";
    }
    else
    {
        if(hasMarkers()==true)
        {
            if(isControlBlock()==true)
            {
                sprintf(c_buf, "CTRL BLOCK [0x%01X]", ((words[STC007Line::PCM_Q_SH336])&0x000F));
            }
            else if(isCRCvalid()==true)
            {
                sprintf(c_buf, "CRC OK  [0x%04X]", words[STC007Line::PCM_CRCC_SH0]);
            }
            else
            {
                sprintf(c_buf, "BD CRC! [0x%04X!=0x%04X]", words[STC007Line::PCM_CRCC_SH0], getCalculatedCRC());
            }
            text_out += c_buf;
        }
        else
        {
            text_out += "No PCM!";
        }
    }
    return text_out;
}

//------------------------ Initialize CRC16 value.
void STC007Line::CRC16_init(uint16_t *CRC_data)
{
    (*CRC_data) = 0xFFFF;
}

//------------------------ Update CRC16 data by calculation.
uint16_t STC007Line::getCalcCRC16(uint16_t CRC_data, uint8_t in_data)
{
    // Name: CRC-16 CCITT-FALSE
    // Poly: 0x1021
    // Init: 0xFFFF
    // Check: 0x29B1 ("123456789")
    uint8_t i;
    CRC_data^=in_data++<<8;
    for (i=0;i<8;i++)
        CRC_data = CRC_data&0x8000?(CRC_data<<1)^0x1021:CRC_data<<1;
    return CRC_data&0xffff;
}

//------------------------ Update CRC16 data by LUT.
uint16_t STC007Line::getTableCRC16(uint16_t CRC_data, uint8_t in_data)
{
    uint16_t old_byte;
    // Push oldest byte out of the buffer, XOR it with input byte.
    old_byte = (CRC_data >> 8) ^ in_data;
    // Get value from the table based on polynome (using discarded and XORed byte as index),
    // XOR value from the table with shifted (on that one byte) buffer.
    CRC_data = (CRC_data << 8) ^ CRC_1021_TABLE[old_byte];
    // Return buffer.
    return CRC_data;
}

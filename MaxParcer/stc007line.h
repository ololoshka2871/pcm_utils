#ifndef STC007LINE_H
#define STC007LINE_H

#include <stdint.h>
#include <string>

#include "config.h"

#if 0

#include <QDebug>
#include <QString>

#endif

//#define STC_LINE_EN_DBG_OUT   1     // Enable debug console output.

class STC007Line
{
public:
    // States for forwards PCM START-marker search state machine.
    enum
    {
        MARK_ST_START,      // Search just started, searching for "1" in the line.
        MARK_ST_TOP_1,      // START-bit[1] ("1") found, searching for "1 -> 0" transition.
        MARK_ST_BOT_1,      // START-bit[2] ("0") found, searching for "0 -> 1" transition.
        MARK_ST_TOP_2,      // START-bit[3] ("1") found, searching for "1 -> 0" transition.
        MARK_ST_BOT_2       // START-bit[4] ("0") found, START-marker found successfully.
    };

    // States for backwards PCM STOP-marker search state machine.
    enum
    {
        MARK_ED_START,      // Search just started, searching for "1" in the line.
        MARK_ED_TOP,        // STOP-bits[5...2] ("1") found, searching for "1 -> 0" transition.
        MARK_ED_BOT,        // STOP-bit[1] ("0") found, checking length of the "white line".
        MARK_ED_LEN_OK      // Length of the STOP-marker is ok, STOP-marker found successfully.
    };

    // Word order in the line.
    enum
    {
        PCM_L_SH0,          // Left channel 14-bit sample for current data block (line offset = 0).
        PCM_R_SH48,         // Right channel 14-bit sample for data block at +48 words (line offset = 16).
        PCM_L_SH95,         // Left channel 14-bit sample for data block at +95 words (line offset = 32).
        PCM_R_SH143,        // Right channel 14-bit sample for data block at +143 words (line offset = 48).
        PCM_L_SH190,        // Left channel 14-bit sample for data block at +190 words (line offset = 64).
        PCM_R_SH238,        // Right channel 14-bit sample for data block at +238 words (line offset = 80).
        PCM_P_SH288,        // P-word (parity) 14-bit sample for data block at +288 words (line offset = 96).
        PCM_Q_SH336,        // Q-word (ECC) 14-bit sample for data block at +336 words (line offset = 112).
        PCM_CRCC_SH0,       // CRCC 16-bit sample for current line (line offset = 0).
        PCM_MAX_SH
    };

    // Bit masks for data control signal.
    enum
    {
        CTRL_COPY_MASK = 0x0008,    // Prohibition of digital dubbing ("0" = disabled, "1" = enabled).
        CTRL_EN_P_MASK = 0x0004,    // Presence of P-word ("0" = present, "1" = absent).
        CTRL_EN_Q_MASK = 0x0002,    // Presence of Q-word/14-16 bit mode ("0" = present [14-bit], "1" = absent [16-bit]).
        CTRL_EMPH_MASK = 0x0001     // Pre-emphasis ("0" = enabled, "1" = disabled).
    };

    // Integer multiplier for pixel coordinate integer calculation.
    enum
    {
        INT_CALC_MULT = 128,
        INT_CALC_PIXEL_DIV = 2
    };

    // Modes of interlace processing for the line.
    enum
    {
        DEINT_OFF,          // Deintarlace disabled.
        DEINT_TFF,          // Deintarlace enabled, Top Field First.
        DEINT_BFF           // Deintarlace enabled, Bottom Field First.
    };

    // Modes of pixel shifter.
    enum
    {
        PIX_NO_SHIFT,       // Pixel shift is disabled, use calculated coordinate.
        PIX_SHIFT_POS,      // Positive pixel shift, shift calculated pixel coordinate one pixel to the right.
        PIX_SHIFT_NEG       // Negative pixel shift, shift calculated pixel coordinate one pixel to the left.
    };

    // Bits placeholders in debug string.
    enum
    {
        PCM_ONE = '#',
        PCM_ZERO = '_'
    };
    /*enum
    {
        PCM_ONE = '1',
        PCM_ZERO = '0'
    };*/

public:
    STC007Line();
    STC007Line(const STC007Line &);
    STC007Line& operator= (const STC007Line &);
    bool operator!= (const STC007Line &);

    void updatePPB();
    uint16_t findVideoPixel(uint16_t line_length, uint8_t pcm_bit, uint8_t shift);
    void calcCRC();
    bool hasStartMarker();
    bool hasStopMarker();
    bool hasMarkers();
    bool isCRCvalid();
    bool isControlBlock();
    bool isCtrlCopyProhibit();
    bool isCtrlEnabledP();
    bool isCtrlEnabledQ();
    bool isCtrlEnabledEmphasis();
    bool isDataBySweep();
    bool isDataBySkip();
    bool doSkip();
    uint8_t getPPB();
    uint8_t getPPBfrac();
    uint16_t getCalculatedCRC();
    std::string toString();
    std::string textDump();

private:
    void CRC16_init(uint16_t *CRC_data);
    uint16_t getCalcCRC16(uint16_t CRC_data, uint8_t in_data);
    uint16_t getTableCRC16(uint16_t CRC_data, uint8_t in_data);

public:
    uint16_t words[PCM_MAX_SH]; // 14 bits words + 16 bit CRCC ([8])
    uint8_t bytes[16];          // Duplicate of all data words
    uint16_t frame_number;      // Number of frame with this line
    uint16_t line_number;       // Number of line in the frame (1=top)
    uint8_t deint_mode;         // Deinterlacing mode.
    uint8_t ref_level;          // Binarization reference level [0...255]
    uint8_t mark_st_stage;      // Starting marker detection state
    uint8_t mark_ed_stage;      // Ending marker detection state
    uint16_t marker_start_coord;    // Pixel coordinate of PCM marker start
    uint16_t data_start_coord;  // Pixel coordinate of PCM data start
    uint16_t data_stop_coord;   // Pixel coordinate of PCM data stop
    uint16_t marker_stop_coord; // Pixel coordinate of PCM marker stop
    uint32_t process_time;      // Amount of time spent processing the line [us]
    uint8_t shift_stage;        // Last stage of pixel-shift process
    bool skip_line;             // This line has to be skipped to assemble data blocks.
    bool ref_level_sweeped;     // Reference level was found by sweeping
    bool data_by_ext_tune;      // Correct CRC was detected for input parameters

private:
    uint16_t calc_crc;              // Re-calculated CRCC
    uint16_t pixel_start_offset;    // Offset (in pixels) of last 4th (black) bit of PCM starting marker from the start of the line.
    uint32_t pixel_size_mult;       // Calculated PCM bit size in pixels, multiplied by [INT_CALC_MULT].
    uint32_t halfpixel_size_mult;   // Half of the [pixel_size_mult], used for bit center calculation.
};

#endif // STC007LINE_H

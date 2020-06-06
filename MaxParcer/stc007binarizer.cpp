#include "pch.h"

#include <chrono>

#include "stc007binarizer.h"

STC007Binarizer::STC007Binarizer()
{
    // Set default preset.
    this->digi_set.digpre_max_black_lvl = 210;
    this->digi_set.digpre_min_white_lvl = 30;
    this->digi_set.digpre_min_contrast = 20;
    this->digi_set.digpre_max_black_dist = 20;
    this->digi_set.digpre_max_white_dist = 32;
    this->digi_set.digpre_min_ref_lvl = 10;
    this->digi_set.digpre_max_ref_lvl = 240;
    this->digi_set.digpre_mark_max_dist = 10;
    this->digi_set.digpre_max_threads = 3;

    this->video_line = NULL;
    this->log_level = 0;
    this->in_def_reference = 0;
    this->in_def_data_start = 0;
    this->in_def_data_stop = 0;
    this->max_threads = 1;
    this->do_pixel_shift = true;
    this->do_ref_level_sweep = true;

    this->proc_state = STG_REF_FIND;
    this->line_length = 0;
    this->br_black = 0;
    this->br_white = 0;
    this->ref_level = this->digi_set.digpre_max_ref_lvl;
    this->mark_start_max = 0;
    this->mark_end_min = 0xFFFF;
    this->ref_sweep_en = false;
    this->black_level_detected = false;
    this->white_level_detected = false;
}

//------------------------ Set debug logging level (LOG_PROCESS, etc...).
void STC007Binarizer::setLogLevel(uint8_t in_level)
{
    this->log_level = in_level;
}

//------------------------ Enable/disable pixel offset shifting and retrying if CRC fails.
void STC007Binarizer::setPixelShiftRetry(bool flag)
{
    this->do_pixel_shift = flag;
#if LB_EN_DBG_OUT
    if((log_level&LOG_SETTINGS)!=0)
    {
        if(flag==true)
        {
            qInfo()<<"[LB] Pixel shifting set to 'enabled'.";
        }
        else
        {
            qInfo()<<"[LB] Pixel shifting set to 'disabled'.";
        }
    }
#endif
}

//------------------------ Enable/disable reference level sweeping (precise, calculation intensive).
void STC007Binarizer::setRefLevelSweep(bool flag)
{
    this->do_ref_level_sweep = flag;
#if LB_EN_DBG_OUT
    if((log_level&LOG_SETTINGS)!=0)
    {
        if(flag==true)
        {
            qInfo()<<"[LB] Reference level sweeping set to 'enabled'.";
        }
        else
        {
            qInfo()<<"[LB] Reference level sweeping set to 'disabled'.";
        }
    }
#endif
}

//------------------------ Set default reference level (0 = disabled).
void STC007Binarizer::setReferenceLevel(uint8_t in_ref)
{
    if(in_ref>0)
    {
        // Set value from the input.
        this->in_def_reference = in_ref;
#if LB_EN_DBG_OUT
        if((log_level&LOG_SETTINGS)!=0)
        {
            qInfo()<<"[LB] Default reference level set to"<<this->in_def_reference;
        }
#endif
    }
    else
    {
        // Reset to default.
        this->in_def_reference = 0;
#if LB_EN_DBG_OUT
        if((log_level&LOG_SETTINGS)!=0)
        {
            qInfo()<<"[LB] Default reference level is disabled.";
        }
#endif
    }
}

//------------------------ Set default pixel coordinates for data (0 = disabled)
void STC007Binarizer::setDataCoordinates(uint16_t in_data_start, uint16_t in_data_stop)
{
    if((in_data_start>0)&&(in_data_stop>0)&&(in_data_start<in_data_stop))
    {
        // Set values from the input.
        this->in_def_data_start = in_data_start;
        this->in_def_data_stop = in_data_stop;
#if LB_EN_DBG_OUT
        if((log_level&LOG_SETTINGS)!=0)
        {
            qInfo()<<"[LB] Default data pixel coordinates set to"<<
                     this->in_def_data_start<<"-"<<this->in_def_data_stop;
        }
#endif
    }
    else
    {
        // Reset to default.
        this->in_def_data_start = 0;
        this->in_def_data_stop = 0;
#if LB_EN_DBG_OUT
        if((log_level&LOG_SETTINGS)!=0)
        {
            qInfo()<<"[LB] Default data pixel coordinates are disabled.";
        }
#endif
    }
}

//------------------------ Set maximum available computing threads.
void STC007Binarizer::setMaxThreads(uint8_t in_threads)
{
    if(in_threads>0)
    {
        // Set value from the input.
        this->max_threads = in_threads;
    }
    else
    {
        // Reset to default.
        this->max_threads = 1;
    }
#if LB_EN_DBG_OUT
    if((log_level&LOG_SETTINGS)!=0)
    {
        qInfo()<<"[LB] Maximum available threads set to"<<this->max_threads;
    }
#endif
}

//------------------------ Take video line, perform AGC, TBC, binarization.
//------------------------ Output PCM data line [STC007line].
uint8_t STC007Binarizer::processLine(VideoLine *in_video_line, STC007Line *out_stc007_line)
{
    uint8_t stage_count, thread_cnt, lvls_per_thread;
    uint8_t min_ref_level, max_ref_level;
    uint32_t tmp_calc;
    bool ref_low_det;

    // Measure processing time for debuging.
    auto start = std::chrono::high_resolution_clock::now();

    ref_low_det = false;

    if(in_video_line==NULL)
    {
#if LB_EN_DBG_OUT
        qWarning()<<"[LB] Null pointer for video line provided, exiting...";
#endif
        return (uint8_t)LB_RET_NULL_VIDEO;
    }
    if(out_stc007_line==NULL)
    {
#if LB_EN_DBG_OUT
        qWarning()<<"[LB] Null pointer for STC-007 line provided, exiting...";
#endif
        return (uint8_t)LB_RET_NULL_STC;
    }
    // Copy VideoLine pointer for internal functions.
    video_line = (VideoLine *)in_video_line;

    // Reset some parameters.
    br_black = 0;
    br_white = 0;

    // Store parameters in new PCM-line object.
    out_stc007_line->frame_number = in_video_line->frame_number;
    out_stc007_line->line_number = in_video_line->line_number;
    if(in_video_line->deint_mode==VideoLine::DEINT_TFF)
    {
        out_stc007_line->deint_mode = STC007Line::DEINT_TFF;
    }
    else if(in_video_line->deint_mode==VideoLine::DEINT_BFF)
    {
        out_stc007_line->deint_mode = STC007Line::DEINT_BFF;
    }
    else
    {
        out_stc007_line->deint_mode = STC007Line::DEINT_OFF;
    }
    out_stc007_line->skip_line = in_video_line->doSkip();

    // Check if line is 100% empty.
    if(in_video_line->isEmpty()==false)
    {
        //----------- STEP 1: Calculate line length, limits for PCM START and PCM STOP markers search,
        //----------- estimate pixels-per-bit value.
        // Get current line length in pixels.
        line_length = in_video_line->pixel_data.size();
        if(line_length<(4+128+5))   // 128 bits of PCM, 4 bits of START marker, 5 bits of STOP marker.
        {
#if LB_EN_DBG_OUT
            qWarning()<<"[LB] Line is too short for PCM data, exiting...";
#endif
            return (uint8_t)LB_RET_SHORT_LINE;
        }

        // Determine limits within the line for marker search.
        mark_start_max = line_length*digi_set.digpre_mark_max_dist/100;
        mark_end_min = line_length-mark_start_max;
        // Estimate minimum ending marker length.
        tmp_calc = line_length*128;
        tmp_calc = tmp_calc/140;
        min_mark_len = (uint16_t)((tmp_calc+64)/128);
#if LB_EN_DBG_OUT
        if((log_level&LOG_PROCESS)!=0)
        {
            QString log_line;
            log_line.sprintf("[LB] -------------------- Line [%03u], size: %03u pixels, estimated PPB: %u, available threads: %u",
                             out_stc007_line->line_number, line_length, min_mark_len, max_threads);
            qInfo()<<log_line;
        }
#endif
        // Pre-calculate minimum PCM STOP marker length (which is 4 bits long).
        min_mark_len = min_mark_len*2;
        //min_mark_len = min_mark_len*5/2;        //  *2.5

        // Set the first stage for state machine.
        proc_state = STG_REF_FIND;
        // Check for provided working reference level.
        if(in_def_reference>digi_set.digpre_min_ref_lvl)
        {
            // Set the state to indicate that at least reference level is provided.
            proc_state = STG_INPUT_LEVEL;
#if LB_EN_DBG_OUT
            if((log_level&LOG_PROCESS)!=0)
            {
                qInfo()<<"[LB] Reference level is provided:"<<in_def_reference;
            }
#endif
        }
        if(in_def_data_stop!=0)
        {
            // Set the state for all possible data inputs.
            proc_state = STG_INPUT_ALL;
#if LB_EN_DBG_OUT
            if((log_level&LOG_PROCESS)!=0)
            {
                qInfo()<<"[LB] Data coordinates are provided:"<<in_def_data_start<<"-"<<in_def_data_stop;
            }
#endif
        }

        // Cycle through stages.
        stage_count = 0;
        do
        {
            // Count loops.
            stage_count++;

            // Select processing mode.
            if(proc_state==STG_DATA_OK)
            {
                // PCM data was decoded successfully.
#if LB_EN_DBG_OUT
                if((log_level&LOG_PROCESS)!=0)
                {
                    qInfo()<<"[LB] -------------------- Valid PCM data was found, work is done.";
                }
#endif
                // Exit stage cycle.
                break;
            }
            else if(proc_state==STG_NO_GOOD)
            {
                // All efforts went to trash - no valid PCM data was found.
#if LB_EN_DBG_OUT
                if((log_level&LOG_PROCESS)!=0)
                {
                    qInfo()<<"[LB] -------------------- No valid PCM data was found in the line.";
                }
#endif
                // Exit stage cycle.
                break;
            }
            else if(proc_state==STG_INPUT_ALL)
            {
                // Preset coordinates from input.
                out_stc007_line->data_start_coord = in_def_data_start;
                out_stc007_line->data_stop_coord = in_def_data_stop;
                // Preset reference level from input.
                out_stc007_line->ref_level = in_def_reference;
                ref_level = in_def_reference;
                // Set parameters to pass checks in the end.
                out_stc007_line->mark_st_stage = STC007Line::MARK_ST_BOT_2;
                out_stc007_line->mark_ed_stage = STC007Line::MARK_ED_LEN_OK;
                // Reference level sweep is disabled for this pass.
                out_stc007_line->ref_level_sweeped = false;
                ref_sweep_en = false;
#if LB_EN_DBG_OUT
                if((log_level&LOG_PROCESS)!=0)
                {
                    qInfo()<<"[LB] ---------- Parameters are set from the input, bypassing coordinates and reference level calculation...";
                }
#endif
                // Try to read PCM data with those parameters.
                if(readPCMdata(out_stc007_line)==REF_CRC_OK)
                {
                    // Set the flag that data was decoded using external parameters.
                    out_stc007_line->data_by_ext_tune = true;
                    // Data is OK, go to exit.
                    proc_state = STG_DATA_OK;
                }
                else
                {
                    // No valid data, let's try to find markers.
                    proc_state = STG_INPUT_LEVEL;
#if LB_EN_DBG_OUT
                    if((log_level&LOG_PROCESS)!=0)
                    {
                        qInfo()<<"[LB] No valid data found with provided data coordinates and reference level.";
                    }
#endif
                }
            }
            else if(proc_state==STG_INPUT_LEVEL)
            {
                // Preset reference level from input.
                out_stc007_line->ref_level = in_def_reference;
                ref_level = in_def_reference;
                // Reset marker coordinates.
                out_stc007_line->data_start_coord = 0;
                out_stc007_line->data_stop_coord = 0;
                // Reset marker detection stages.
                out_stc007_line->mark_st_stage = STC007Line::MARK_ST_START;
                out_stc007_line->mark_ed_stage = STC007Line::MARK_ED_START;
                // Reference level sweep is disabled for this pass.
                out_stc007_line->ref_level_sweeped = false;
                ref_sweep_en = false;
#if LB_EN_DBG_OUT
                if((log_level&LOG_PROCESS)!=0)
                {
                    qInfo()<<"[LB] ---------- Reference level is set from the input, bypassing reference level calculation...";
                }
#endif
                // Find coordinates for PCM START and STOP markers ([findPCMmarkers()] resets all marker coordinates).
                findPCMmarkers(out_stc007_line);

                // Check if markers were found at all.
                if(out_stc007_line->hasMarkers()!=true)
                {
                    // Markers (both or one of those) are not found.
                    // No reason to try decoding anything, go to full processing.
                    proc_state = STG_REF_FIND;
#if LB_EN_DBG_OUT
                    if((log_level&LOG_PROCESS)!=0)
                    {
                        qInfo()<<"[LB] Markers were not found with provided reference level.";
                    }
#endif
                }
                else
                {
                    // Markers were found, try to decode data.
                    if(readPCMdata(out_stc007_line)==REF_CRC_OK)
                    {
                        // Set the flag that data was decoded using external parameters.
                        out_stc007_line->data_by_ext_tune = true;
                        // Data is OK, go to exit.
                        proc_state = STG_DATA_OK;
                    }
                    else
                    {
                        // No valid data, let's try to find reference level.
                        proc_state = STG_REF_FIND;
#if LB_EN_DBG_OUT
                        if((log_level&LOG_PROCESS)!=0)
                        {
                            qInfo()<<"[LB] No valid data found with provided reference level.";
                        }
#endif
                    }
                }
            }
            else if(proc_state==STG_REF_FIND)
            {
                // Nothing is preset, do the whole calculation.
#if LB_EN_DBG_OUT
                if((log_level&LOG_PROCESS)!=0)
                {
                    qInfo()<<"[LB] ---------- Reference level detection starting...";
                }
#endif
                // Find WHITE and BLACK levels, find STOP marker.
                findBlackWhite(out_stc007_line);

                // Check for early PCM detection in the line.
                if((white_level_detected==true)&&(out_stc007_line->hasStopMarker()==true))
                {
                    // If allowed - enable reference level sweep.
                    if(do_ref_level_sweep==true)
                    {
                        ref_sweep_en = true;
                    }
                    if(ref_sweep_en!=true)
                    {
                        // Calculate reference level if sweep is disabled.
                        calcRefLevel();
                        // Store reference level.
                        out_stc007_line->ref_level = ref_level;
                        // Proceed with single data decoding.
                        proc_state = STG_READ_PCM;
                    }
                    else
                    {
                        // Go to full reference level sweep.
                        proc_state = STG_REFSCAN_RUN;

                        // DEBUG
                        //ref_sweep_en = false;

                    }
                }
                else
                {
                    // No WHITE level - no PCM.
                    proc_state = STG_NO_GOOD;
                }
            }
            else if(proc_state==STG_REFSCAN_RUN)
            {
                // Do reference level sweep.
#if LB_EN_DBG_OUT
                if((log_level&LOG_PROCESS)!=0)
                {
                    qInfo()<<"[LB] ---------- Full reference level sweep starting...";
                }
#endif
                // Reset spread data for reference level.
                spread_data.fill(0);

    /*#pragma omp parallel
                sweepRefLevel(&spread_data, br_black, br_white, 0);*/

                // Limit number of threads.
                if(max_threads>digi_set.digpre_max_threads)
                {
                    thread_cnt = digi_set.digpre_max_threads;
                }
                else
                {
                    thread_cnt = max_threads;
                }
                // Calculate number of levels to check for each thread.
                lvls_per_thread = br_white-br_black;
                lvls_per_thread = lvls_per_thread/thread_cnt;
                if(lvls_per_thread==0)
                {
                    lvls_per_thread = 1;
                }
#if LB_EN_DBG_OUT
                if((log_level&LOG_SWEEP)!=0)
                {
                    QString log_line;
                    qInfo()<<"[LB] Levels per thread:"<<lvls_per_thread;
                    log_line = "[LB] Thread spans: ";
                    for(uint8_t thrd=0;thrd<thread_cnt;thrd++)
                    {
                        if((thrd==(thread_cnt-1))||((br_black+lvls_per_thread*(thrd+1))>=br_white))
                        {
                            log_line += " ("+QString::number(br_black+lvls_per_thread*thrd)+":"+QString::number(br_white)+"]";
                            break;
                        }
                        else
                        {
                            log_line += " ("+QString::number(br_black+lvls_per_thread*thrd)+":"+QString::number(br_black+lvls_per_thread*(thrd+1))+"]";
                        }
                    }
                    qInfo()<<log_line;
                }
#endif

                // Storage for temporary threads.
                std::vector<std::thread> threads;
                bool last_thread;
                last_thread = false;
                // Launching all sweep threads.
                for(uint8_t thrd=0;thrd<thread_cnt;thrd++)
                {
                    // Determine range of brightnesses.
                    if((thrd==(thread_cnt-1))||((br_black+lvls_per_thread*(thrd+1))>=br_white))
                    {
                        min_ref_level = br_black+lvls_per_thread*thrd;
                        max_ref_level = br_white;
                        last_thread = true;
                    }
                    else
                    {
                        min_ref_level = br_black+lvls_per_thread*thrd;
                        max_ref_level = br_black+lvls_per_thread*(thrd+1);
                    }
                    // Start separate workers in threads.
                    threads.push_back(std::thread(&STC007Binarizer::sweepRefLevel, this, &spread_data, min_ref_level, max_ref_level, thrd));
                    if(last_thread==true) break;
                }
                // Wait until all threads are finished.
                for(uint8_t thrd=0;thrd<threads.size();thrd++)
                {
                    threads[thrd].join();
                }
                // Clear threads storage.
                for(uint8_t thrd=0;thrd<threads.size();thrd++)
                {
                    threads.pop_back();
                }

                // Preset span boundaries to be updated in cycle.
                min_ref_level = digi_set.digpre_max_ref_lvl;
                max_ref_level = digi_set.digpre_min_ref_lvl;
                ref_low_det = false;
                // Cycle through whole range of brightnesses to find "good" reference level span.
                for(ref_level=br_black;ref_level<br_white;ref_level++)
                {
                    if(spread_data[ref_level]==REF_CRC_OK)
                    {
                        if(max_ref_level<ref_level)
                        {
                            // Store highest reference value.
                            max_ref_level = ref_level;
                        }
                        if(min_ref_level>ref_level)
                        {
                            // Store lowest reference value if it is not locked yet.
                            min_ref_level = ref_level;
                            // Set "reference low level detected" flag.
                            ref_low_det = true;
                        }
                    }
                    else
                    {
                        if(ref_low_det==true)
                        {
                            // If good level was detected earlier and now it is not - search is done.
                            break;
                        }
                    }
                }

                // Check if "good reference level" span was found.
                if(ref_low_det==true)
                {
                    // Get center of the "good reference level" span.
                    ref_level = max_ref_level-min_ref_level;
                    ref_level = (ref_level+1)/2;
                    // Set that as final reference level.
                    ref_level += min_ref_level;
                    out_stc007_line->ref_level = ref_level;
                    // Set flag that reference level was sweeped.
                    out_stc007_line->ref_level_sweeped = true;
#if LB_EN_DBG_OUT
                    if((log_level&LOG_PROCESS)!=0)
                    {
                        QString log_line;
                        log_line.sprintf("[LB] Good reference level span [%03u:%03u] detected", min_ref_level, max_ref_level);
                        qInfo()<<log_line;
                    }
                    if((log_level&LOG_SWEEP)!=0)
                    {
                        QString stats;
                        for(uint16_t index=0;index<256;index++)
                        {
                            if(index==ref_level)
                            {
                                stats += "*";
                            }
                            else
                            {
                                stats += QString::number(spread_data[index]);
                            }
                        }
                        qInfo()<<"[LB] Sweep"<<out_stc007_line->line_number<<stats;
                    }
#endif
                }
                else
                {
                    // No good reference level was found.
#if LB_EN_DBG_OUT
                    if((log_level&LOG_PROCESS)!=0)
                    {
                        qInfo()<<"[LB] No good reference value was found during sweep.";
                    }
#endif
                    // Fallback to other value.
                    // Check for provided working reference level.
                    if(in_def_reference>digi_set.digpre_min_ref_lvl)
                    {
                        // Copy provided value from good line.
                        ref_level = in_def_reference;
                    }
                    else
                    {
                        // Set calculated estimate.
                        calcRefLevel();
                    }
                    // Copy calculated value into the line.
                    out_stc007_line->ref_level = ref_level;
                }

                // Disable "do reference level sweep" for final log output.
                ref_sweep_en = false;

                // Proceed with final data decoding.
                proc_state = STG_READ_PCM;
            }
            else if(proc_state==STG_READ_PCM)
            {
#if LB_EN_DBG_OUT
                if((log_level&LOG_PROCESS)!=0)
                {
                    qInfo()<<"[LB] ---------- Brighness threshold:"<<out_stc007_line->ref_level;
                    if((log_level&LOG_VIDEO)!=0)
                    {
                        QString log_line;
                        uint8_t pixel_data;
                        for(uint16_t pixel=0;pixel<line_length;pixel++)
                        {
                            pixel_data = getVideoPixel(pixel);
                            if(pixel_data>=out_stc007_line->ref_level)
                            {
                                log_line += "1";
                            }
                            else
                            {
                                log_line += "0";
                            }
                        }
                        qInfo()<<"[LB] Binarized line data:";
                        qInfo()<<log_line;
                    }
                }
#endif
                // Find coordinates for PCM START and STOP markers with set reference level.
                findPCMmarkers(out_stc007_line);
                // Reading PCM data with calculated parameters.
                if(readPCMdata(out_stc007_line)==REF_CRC_OK)
                {
                    // Data read OK, work is done.
                    proc_state = STG_DATA_OK;
                }
                else
                {
                    // No valid PCM data found.
                    proc_state = STG_NO_GOOD;
                }
            }
            else
            {
#if LB_EN_DBG_OUT
                qWarning()<<"[LB] -------------------- Impossible state detected, breaking...";
#endif
                break;
            }

            // Check for looping.
            if(stage_count>STG_MAX)
            {
#if LB_EN_DBG_OUT
                qWarning()<<"[LB] -------------------- Inf. loop detected, breaking...";
#endif
                break;
            }
        }
        while(1);   // Stages cycle.
    }
#if LB_EN_DBG_OUT
    else
    {
        if((log_level&LOG_PROCESS)!=0)
        {
            qInfo()<<"[LB] Line marked as 'empty', skipping all processing...";
        }
    }
#endif

    // Store time that processing took.
    uint64_t line_time;
    auto end = std::chrono::high_resolution_clock::now();
    out_stc007_line->process_time = (uint32_t)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

    return (uint8_t)LB_RET_OK;
}


//------------------------ Debug wrapper for getting value from pixel vector.
uint8_t STC007Binarizer::getVideoPixel(uint16_t x_coord)
{
#ifdef LB_EN_PIXEL_DBG
    if(video_line==NULL)
    {
        qWarning()<<"[LB] Video line has NULL pointer!";
        return (line_length-1);
    }
    else
    {
        if(x_coord<=(line_length-1))
        {
            // Normal result.
            return video_line->pixel_data.at(x_coord);
        }
        else
        {
            // Pixel coordinate out of bounds.
            qWarning()<<"[LB] Incorrect pixel coordinate! ("<<x_coord<<") while processing frame"
                      <<frame_number<<"line"<<line_number;
            return (line_length-1);
        }
    }
#else
    return video_line->pixel_data.at(x_coord);
#endif
}

//------------------------ Find coordinates of START and STOP markers.
void STC007Binarizer::findPCMmarkers(STC007Line *pcm_line, uint8_t thread_id, bool no_log)
{
    uint8_t marker_detect_stage, pixel_data;
    uint16_t pixel;
    uint16_t mark_st_1bit_start,    // Pixel coordinate of the start of the 1st "1" START bits.
             mark_st_1bit_end,      // Pixel coordinate of the end of the 1st "1" START bits and of the start of the 1st "0" START bits.
             mark_st_3bit_start,    // Pixel coordinate of the start of the 2nd "1" START bits and of the end of the 1st "0" START bits.
             mark_st_3bit_end,      // Pixel coordinate of the end of the 2nd "1" START bits and of the start of the 2nd "0" START bits.
             mark_ed_bit_start,     // Pixel coordinate of the start of the "1111" STOP bits and of the end of the "0" STOP bit.
             mark_ed_bit_end;       // Pixel coordinate of the end of the "1111" STOP bits.
    //----------- STEP 6: perform binarization and
    //----------- find coordinates of PCM START marker.
    marker_detect_stage = STC007Line::MARK_ST_START;
    mark_st_1bit_start = 0;
    mark_st_1bit_end = 0;
    mark_st_3bit_start = 0;
    mark_st_3bit_end = 0;
    for(pixel=0;pixel<=mark_start_max;pixel++)
    {
        pixel_data = getVideoPixel(pixel);
        if(marker_detect_stage==STC007Line::MARK_ST_START)
        {
            // Detect /\ transition to first "1" marker bit.
            if(pixel_data>=(pcm_line->ref_level))
            {
                mark_st_1bit_start = pixel;
                marker_detect_stage = STC007Line::MARK_ST_TOP_1;
            }
        }
        else if(marker_detect_stage==STC007Line::MARK_ST_TOP_1)
        {
            // Detect \/ transition to first "0" marker bit.
            if(pixel_data<(pcm_line->ref_level))
            {
                mark_st_1bit_end = pixel;
                // Check if detected "bit" complies with required length (min_mark_len = pixel_per_bit x 4).
                if((mark_st_1bit_end-mark_st_1bit_start)>min_mark_len)
                {
                    // Reset search if length is inadequate.
                    marker_detect_stage = STC007Line::MARK_ST_START;
#if LB_EN_DBG_OUT
                    if(((log_level&LOG_PROCESS)!=0)&&(no_log!=true))
                    {
                        QString log_line;
                        log_line.sprintf("[LB-%02u] Erroneous START-bit[1] at [%03u:%03u], restarting...", thread_id, mark_st_1bit_start, mark_st_1bit_end);
                        qInfo()<<log_line;
                    }
#endif
                }
                else
                {
                    marker_detect_stage = STC007Line::MARK_ST_BOT_1;
#if LB_EN_DBG_OUT
                    if(((log_level&LOG_PROCESS)!=0)&&(no_log!=true))
                    {
                        QString log_line;
                        log_line.sprintf("[LB-%02u] START-bit[1] found at [%03u:%03u]", thread_id, mark_st_1bit_start, mark_st_1bit_end);
                        qInfo()<<log_line;
                    }
#endif
                }
            }
        }
        else if(marker_detect_stage==STC007Line::MARK_ST_BOT_1)
        {
            // Detect /\ transition to second "1" marker bit.
            if(pixel_data>=(pcm_line->ref_level))
            {
                mark_st_3bit_start = pixel;
                // Check if detected "bit" complies with required length (min_mark_len = pixel_per_bit x 4).
                if((mark_st_3bit_start-mark_st_1bit_end)>min_mark_len)
                {
                    // Reset search if length is inadequate.
                    marker_detect_stage = STC007Line::MARK_ST_START;
#if LB_EN_DBG_OUT
                    if(((log_level&LOG_PROCESS)!=0)&&(no_log!=true))
                    {
                        QString log_line;
                        log_line.sprintf("[LB-%02u] Erroneous START-bit[2] at [%03u:%03u], restarting...", thread_id, mark_st_1bit_end, mark_st_3bit_start);
                        qInfo()<<log_line;
                    }
#endif
                }
                else
                {
                    marker_detect_stage = STC007Line::MARK_ST_TOP_2;
#if LB_EN_DBG_OUT
                    if(((log_level&LOG_PROCESS)!=0)&&(no_log!=true))
                    {
                        QString log_line;
                        log_line.sprintf("[LB-%02u] START-bit[2] found at [%03u:%03u]", thread_id, mark_st_1bit_end, mark_st_3bit_start);
                        qInfo()<<log_line;
                    }
#endif
                }
            }
        }
        else if(marker_detect_stage==STC007Line::MARK_ST_TOP_2)
        {
            // Detect \/ transition to second "0" marker bit.
            if(pixel_data<(pcm_line->ref_level))
            {
                mark_st_3bit_end = pixel;
                // Check if detected "bit" complies with required length (min_mark_len = pixel_per_bit x 4).
                if((mark_st_3bit_end-mark_st_3bit_start)>min_mark_len)
                {
                    // Reset search if length is inadequate.
                    marker_detect_stage = STC007Line::MARK_ST_START;
#if LB_EN_DBG_OUT
                    if(((log_level&LOG_PROCESS)!=0)&&(no_log!=true))
                    {
                        QString log_line;
                        log_line.sprintf("[LB-%02u] Erroneous START-bit[3] at [%03u:%03u], restarting...", thread_id, mark_st_3bit_start, mark_st_3bit_end);
                        qInfo()<<log_line;
                    }
#endif
                }
                else
                {
                    marker_detect_stage = STC007Line::MARK_ST_BOT_2;
#if LB_EN_DBG_OUT
                    if(((log_level&LOG_PROCESS)!=0)&&(no_log!=true))
                    {
                        QString log_line;
                        log_line.sprintf("[LB-%02u] START-bit[3] found at [%03u:%03u]", thread_id, mark_st_3bit_start, mark_st_3bit_end);
                        qInfo()<<log_line;
                    }
#endif
                    // Stop marker search.
                    break;
                }
            }
        }
    }
    //findPCMmarkers_out_start_find:
#if LB_EN_DBG_OUT
    if(((log_level&LOG_PROCESS)!=0)&&(no_log!=true))
    {
        if(marker_detect_stage!=STC007Line::MARK_ST_BOT_2)
        {
            QString log_line;
            log_line.sprintf("[LB-%02u] START marker was NOT found!", thread_id);
            qInfo()<<log_line;
        }
    }
#endif
    // Store final state.
    pcm_line->mark_st_stage = marker_detect_stage;
    // Save marker start coordinate.
    pcm_line->marker_start_coord = mark_st_1bit_start;
    // Save data start coordinate.
    pcm_line->data_start_coord = mark_st_1bit_end;

    //----------- STEP 7: Refine coordinates of PCM STOP marker with new reference level.
    // Re-find PCM ending marker (after reference adjustment).
    marker_detect_stage = STC007Line::MARK_ED_START;
    mark_ed_bit_start = 0;
    mark_ed_bit_end = 0;
    // Perform PCM STOP-marker refinement only if PCM START-marker was detected.
    if(pcm_line->hasStartMarker())
    {
        for(pixel=(line_length-1);pixel>=mark_end_min;pixel--)
        {
            pixel_data = getVideoPixel(pixel);
            if(marker_detect_stage==STC007Line::MARK_ED_START)
            {
                // Detect /\ transition to "1" marker bit.
                if(pixel_data>=(pcm_line->ref_level))
                {
                    mark_ed_bit_end = pixel+1;
                    marker_detect_stage=STC007Line::MARK_ED_TOP;
                }
            }
            else if(marker_detect_stage==STC007Line::MARK_ED_TOP)
            {
                // Detect \/ transition to "0" marker bit.
                if(pixel_data<(pcm_line->ref_level))
                {
                    mark_ed_bit_start = (pixel+1);
                    marker_detect_stage = STC007Line::MARK_ED_BOT;
                    // Check if "1" bit length is of correct length.
                    if((mark_ed_bit_end-mark_ed_bit_start)>=(min_mark_len/2))
                    {
                        marker_detect_stage = STC007Line::MARK_ED_LEN_OK;
#if LB_EN_DBG_OUT
                        if(((log_level&LOG_PROCESS)!=0)&&(no_log!=true))
                        {
                            QString log_line;
                            log_line.sprintf("[LB-%02u] STOP-marker updated to be at [%03u:%03u]", thread_id, mark_ed_bit_start, mark_ed_bit_end);
                            qInfo()<<log_line;
                        }
#endif
                        // Stop marker search.
                        break;
                    }
                    else
                    {
                        // Wrong "1" bit length, probably noise - reset state machine.
                        marker_detect_stage = STC007Line::MARK_ED_START;
#if LB_EN_DBG_OUT
                        if(((log_level&LOG_PROCESS)!=0)&&(no_log!=true))
                        {
                            QString log_line;
                            log_line.sprintf("[LB-%02u] Erroneous STOP-marker at [%03u:%03u], restarting...", thread_id, mark_ed_bit_start, mark_ed_bit_end);
                            qInfo()<<log_line;
                        }
#endif
                    }
                }
            }
        }
    }
    //findPCMmarkers_out_stop_find:
    // Store final state.
    pcm_line->mark_ed_stage = marker_detect_stage;
    // Save data stop coordinate.
    pcm_line->data_stop_coord = mark_ed_bit_start;
    // Save marker stop coordinate.
    pcm_line->marker_stop_coord = mark_ed_bit_end;
}

//------------------------ Find WHITE and BLACK levels, calculate reference level.
void STC007Binarizer::findBlackWhite(STC007Line *pcm_line)
{
    uint8_t pixel_data, brt_lev, marker_detect_stage;
    uint16_t black_lvl_count, white_lvl_count, search_lim;
    uint16_t mark_ed_bit_start,     // Pixel coordinate of the start of the "1111" STOP bits and of the end of the "0" STOP bit.
            mark_ed_bit_end;        // Pixel coordinate of the end of the "1111" STOP bits.
    //----------- STEP 2: Calculate brightness spread for the line
    //----------- and find WHITE level.
    // Reset brightness spread data.
    spread_data.fill(0);
    // Pre-calculate rough brighness spread (can contain black borders and other garbade).
    // Calculate spread of brighness for the left side of the line (containing PCM START marker).
    for(uint16_t pixel=0;pixel<mark_start_max;pixel++)
    {
        spread_data[getVideoPixel(pixel)]++;
    }
    // Calculate spread of brighness for the right side of the line (containing PCM STOP marker).
    for(uint16_t pixel=mark_end_min;pixel<line_length;pixel++)
    {
        spread_data[getVideoPixel(pixel)]++;
    }
#if LB_EN_DBG_OUT
    if(((log_level&LOG_BRIGHT)!=0)&&((log_level&LOG_PROCESS)!=0))
    {
        QString stats;
        for(uint16_t index=0;index<256;index++)
        {
            stats += QString::number(spread_data[index])+",";
        }
        qInfo()<<"[LB] Full line brighness spread:"<<stats;
    }
#endif
    // Find "white" level (via most frequent high-value).
    brt_lev = 255;
    white_lvl_count = 0;
    white_level_detected = false;
    while(brt_lev>=digi_set.digpre_min_white_lvl)
    {
        // Search for maximum encounters in high brightness.
        if(spread_data[brt_lev]>white_lvl_count)
        {
            // Store current encounters of "white" level.
            white_lvl_count = spread_data[brt_lev];
            // Store new WHITE level (as coordinate of peak encounters).
            br_white = brt_lev;
            // Store "WHITE level found" flag.
            white_level_detected = true;
        }
        if(white_level_detected==true)
        {
            // If white level was found in process, check for current distance from that peak.
            if((br_white-brt_lev)>digi_set.digpre_max_white_dist)
            {
                // No need to search so far from found peak.
                break;
            }
        }
        brt_lev--;
    }
#if LB_EN_DBG_OUT
    if((log_level&LOG_PROCESS)!=0)
    {
        if(white_level_detected==true)
        {
            QString log_line;
            log_line.sprintf("[LB] WHITE level detected as [%03u] with [%03u] encounters", br_white, white_lvl_count);
            qInfo()<<log_line;
        }
        else
        {
            QString log_line;
            log_line.sprintf("[LB] WHITE level was NOT detected!");
            qInfo()<<log_line;
        }
    }
#endif

    //----------- STEP 3: Find presence and coordinates of PCM STOP marker,
    //----------- if found, recalculate brightness spread for CRC part of the line.
    marker_detect_stage = STC007Line::MARK_ED_START;
    mark_ed_bit_start = 0;
    mark_ed_bit_end = 0;
    // Check if white level was detected (if not - markers can not be found).
    if(white_level_detected==true)
    {
        // Define reference level for white detection.
        //ref_level = br_white-(br_white/5);
        ref_level = br_white/2;
        // Find position of PCM ending marker.
        for(uint16_t pixel=(line_length-1);pixel>=mark_end_min;pixel--)
        {
            pixel_data = getVideoPixel(pixel);
            if(marker_detect_stage==STC007Line::MARK_ED_START)
            {
                // Detect /\ transition to "1" marker bit.
                if(pixel_data>=ref_level)
                {
                    mark_ed_bit_end = pixel+1;
                    marker_detect_stage=STC007Line::MARK_ED_TOP;
                }
            }
            else if(marker_detect_stage==STC007Line::MARK_ED_TOP)
            {
                // Detect \/ transition to "0" marker bit.
                if(pixel_data<ref_level)
                {
                    mark_ed_bit_start = pixel+1;
                    marker_detect_stage = STC007Line::MARK_ED_BOT;
                    // Check if "1" bit length is more than pre-calculated at start.
                    if((mark_ed_bit_end-mark_ed_bit_start)>=min_mark_len)
                    {
                        marker_detect_stage = STC007Line::MARK_ED_LEN_OK;
#if LB_EN_DBG_OUT
                        if((log_level&LOG_PROCESS)!=0)
                        {
                            QString log_line;
                            log_line.sprintf("[LB] STOP-marker found at [%03u:%03u]", mark_ed_bit_start, mark_ed_bit_end);
                            qInfo()<<log_line;
                        }
#endif
                        // Store length of the ending marker (4 x PPB).
                        min_mark_len = mark_ed_bit_end-mark_ed_bit_start;
                        // Set the limited distance (covering CRC area) from STOP-marker for short brightness spread re-calc.
                        search_lim = min_mark_len*16;
                        if(search_lim>mark_ed_bit_start)
                        {
                            search_lim = 0;
                        }
                        else
                        {
                            search_lim = mark_ed_bit_start-search_lim;
                        }
                        // Reset brightness spread data.
                        spread_data.fill(0);
                        // Re-calculate spread of brighness for the end of the line.
                        for(uint16_t pixel=(mark_ed_bit_start-1);pixel>search_lim;pixel--)
                        {
                            spread_data[getVideoPixel(pixel)]++;
                        }
#if LB_EN_DBG_OUT
                        if((log_level&LOG_BRIGHT)!=0)
                        {
                            QString stats;
                            for(uint16_t index=0;index<256;index++)
                            {
                                stats += QString::number(spread_data[index])+",";
                            }
                            qInfo()<<"[LB] CRC part ("<<search_lim<<"-"<<(mark_ed_bit_start-1)<<") brighness spread:"<<stats;
                        }
#endif
                        // Stop marker search.
                        break;
                    }
                    else
                    {
                        // Wrong "1" bit length, probably noise - reset state machine.
                        marker_detect_stage = STC007Line::MARK_ED_START;
#if LB_EN_DBG_OUT
                        if((log_level&LOG_PROCESS)!=0)
                        {
                            QString log_line;
                            log_line.sprintf("[LB] Erroneous STOP-marker at [%03u:%03u], restarting...", mark_ed_bit_start, mark_ed_bit_end);
                            qInfo()<<log_line;
                        }
#endif
                    }
                }
            }
        }
        //findBlackWhite_out_stop_find:
#if LB_EN_DBG_OUT
        // Check if PCM ending marker was found.
        if(marker_detect_stage!=STC007Line::MARK_ED_LEN_OK)
        {
            if((log_level&LOG_PROCESS)!=0)
            {
                qInfo()<<"[LB] STOP marker was NOT found!";
            }
        }
#endif
    }
    // Store final state.
    pcm_line->mark_ed_stage = marker_detect_stage;
    // Save coordinates.
    pcm_line->data_stop_coord = mark_ed_bit_start;
    pcm_line->marker_stop_coord = mark_ed_bit_end;

    //----------- STEP 4: Find BLACK level from brighness spread data.
    // Find "black" level (via most frequent low-value).
    brt_lev = 0;
    black_lvl_count = 0;
    black_level_detected = false;
    while(brt_lev<digi_set.digpre_max_black_lvl)
    {
        // Search for maximum encounters in low brightness.
        if(spread_data[brt_lev]>black_lvl_count)
        {
            // Store current encounters of "black" level.
            black_lvl_count = spread_data[brt_lev];
            // Store new "white" level (as coordinate of peak encounters).
            br_black = brt_lev;
            // Store "black level found" flag.
            black_level_detected = true;
        }
        if(black_level_detected==true)
        {
            // If black level was found, check for current distance from that peak.
            if((brt_lev-br_black)>digi_set.digpre_max_black_dist)
            {
                // No need to search so far from found peak.
                break;
            }
        }
        brt_lev++;
    }
    // Reset reference level.
    if(br_white<digi_set.digpre_max_ref_lvl)
    {
        ref_level = digi_set.digpre_max_ref_lvl;
    }
    else
    {
        ref_level = digi_set.digpre_min_ref_lvl;
    }
#if LB_EN_DBG_OUT
    if((log_level&LOG_PROCESS)!=0)
    {
        if(black_level_detected==true)
        {
            QString log_line;
            log_line.sprintf("[LB] BLACK level detected as [%03u] with [%03u] encounters", br_black, black_lvl_count);
            qInfo()<<log_line;
        }
        else
        {
            QString log_line;
            log_line.sprintf("[LB] BLACK level was NOT detected!");
            qInfo()<<log_line;
        }
    }
#endif
}

//------------------------ Calculate reference level from BLACK and WHITE levels.
void STC007Binarizer::calcRefLevel()
{
    uint8_t br_delta;
    //----------- STEP 5: Check contrast and calculate reference brightness level for binarization.
    // Get delta between "white" and "black".
    br_delta = br_white-br_black;
    // Check for contrast.
    if(br_delta>=digi_set.digpre_min_contrast)
    {
        // Contrast is Ok.
        // Calculate PCM reference level.

        //br_delta = br_delta/3;        // br_delta/1.5
        //br_delta = br_delta/3;
        //br_delta = br_delta/2;
        //br_delta = br_delta*10/22;    // br_delta/2.2
        //br_delta = br_delta/3;
        //br_delta = (br_delta*4)/13; // br_delta/3.25
        br_delta = (br_delta*2)/7;  // br_delta/3.5
        //br_delta = (br_delta*25)/91;  // br_delta/3.64
        //br_delta = br_delta/4;

        // Shift reference level by black level.
        ref_level = br_delta+br_black;
        // Limit reference level.
        if(ref_level<digi_set.digpre_min_ref_lvl)
        {
            ref_level = digi_set.digpre_min_ref_lvl;
        }
        else if(ref_level>digi_set.digpre_max_ref_lvl)
        {
            ref_level = digi_set.digpre_max_ref_lvl;
        }
    }
    else
    {
#if LB_EN_DBG_OUT
        if((log_level&LOG_PROCESS)!=0)
        {
            qInfo()<<"[LB] Contrast is too low!";
        }
#endif
        // Contrast is too low, set reference beyond the range of brightnesses.
        if(br_white<digi_set.digpre_max_ref_lvl)
        {
            ref_level = digi_set.digpre_max_ref_lvl;
        }
        else
        {
            ref_level = digi_set.digpre_min_ref_lvl;
        }
    }
#if LB_EN_DBG_OUT
    if((log_level&LOG_PROCESS)!=0)
    {
        qInfo()<<"[LB] Calculated reference level:"<<ref_level;
    }
#endif
}

//------------------------ Perform reference level sweep from one level to another (multithreadable).
void STC007Binarizer::sweepRefLevel(std::array<uint16_t, 256> *result, uint8_t low_lvl, uint8_t high_lvl, uint8_t thread_id)
{
    uint8_t ref_index;
    STC007Line dummy_line;
#if LB_EN_DBG_OUT
    if((log_level&LOG_SWEEP)!=0)
    {
        QString log_line;
        log_line.sprintf("[LB-%02u] Reference level sweep: [%03u...%03u]", thread_id, (low_lvl+1), high_lvl);
        qInfo()<<log_line;
    }
#endif

    // Cycle through provided range of brightnesses to get CRC result stats.
    for(ref_index=high_lvl;ref_index>low_lvl;ref_index--)
    {
        dummy_line.ref_level = ref_index;
        // Find coordinates for PCM START and STOP markers with set reference level.
        if((log_level&LOG_SWEEP)!=0)
        {
            findPCMmarkers(&dummy_line, thread_id, false);
        }
        else
        {
            findPCMmarkers(&dummy_line, thread_id, true);
        }
        // Check if markers were found.
        if(dummy_line.hasMarkers()==true)
        {
            // Binarize and read PCM data, calculate CRC.
            // Store decode result in spread data.
            if((log_level&LOG_SWEEP)!=0)
            {
                result->at(ref_index) = readPCMdata(&dummy_line, thread_id, false);
            }
            else
            {
                result->at(ref_index) = readPCMdata(&dummy_line, thread_id, true);
            }
        }
#if LB_EN_DBG_OUT
        else if((log_level&LOG_SWEEP)!=0)
        {
            QString log_line;
            log_line.sprintf("[LB-%02u] Markers not found", thread_id);
            qInfo()<<log_line;
        }
#endif
    }
}

//------------------------ Calculate PPB, fill up data words, recalculate and compare CRC.
void STC007Binarizer::fillDataWords(STC007Line *pcm_line, uint8_t shift_dir, bool no_log)
{
    uint8_t pixel_data, shift_sw, pcm_byte;

    uint8_t word_bit_pos, byte_bit_pos;
    uint8_t word_index, byte_index;
    uint16_t pcm_word;

    // No pixel shift.
    shift_sw = STC007Line::PIX_NO_SHIFT;
    // Perform pixel offset shifting if required.
    if(shift_dir==1)
    {
        // Shift line to the left by 1 pixel.
        shift_sw = STC007Line::PIX_SHIFT_POS;
    }
    else if(shift_dir==2)
    {
        // Shift line to the right by 1 pixel.
        shift_sw = STC007Line::PIX_SHIFT_NEG;
    }

#if LB_EN_DBG_OUT
    if(((log_level&LOG_VIDEO)!=0)&&(no_log!=true))
    {
        QString log_line;
        log_line = "[LB] Pixel coordinates: ";
        pcm_word = 0;
        word_bit_pos = 0;
        for(uint8_t pcm_bit=0;pcm_bit<129;pcm_bit++)
        {
            pcm_word++;
            // Find bit coodinate in video line.
            log_line += QString::number(pcm_line->findVideoPixel(line_length, pcm_bit, shift_sw))+",";
        }
        log_line += " total pixels: "+QString::number(pcm_word);
        qInfo()<<log_line;
    }
#endif

    // Fill up 14 bit words (and 16 bit CRCC).
    pcm_byte = 0;
    pcm_word = 0;
    byte_bit_pos = 7;
    word_bit_pos = 13;
    byte_index = 0;
    word_index = 0;

    for(uint8_t pcm_bit=1;pcm_bit<129;pcm_bit++)
    {
        // Pick the bit.
        pixel_data = getVideoPixel(pcm_line->findVideoPixel(line_length, pcm_bit, shift_sw));
        // Perform binarization.
        if(pixel_data>=(pcm_line->ref_level))
        {
            // Set the bit if presents at current bit position.
            pcm_word |= (1<<word_bit_pos);
            pcm_byte |= (1<<byte_bit_pos);
        }
        // Check if the byte is complete.
        if(byte_bit_pos==0)
        {
            // Save completed byte.
            pcm_line->bytes[byte_index] = pcm_byte;
            // Go to the next byte.
            byte_index++;
            // Reset bits in next byte.
            pcm_byte = 0;
            byte_bit_pos = 8;
        }
        // Check if the word is complete.
        if(word_bit_pos==0)
        {
            // Save completed word.
            pcm_line->words[word_index] = pcm_word;
            if(pcm_bit>112)
            {
                // CRC was read, line is finished, exit cycle.
                break;
            }
            else if(pcm_bit==112)
            {
                // That was the last 14-bit word, set next word to be 16-bit CRCC.
                word_bit_pos = 16;
            }
            else
            {
                // Word was read, set next 14-bit word.
                word_bit_pos = 14;
            }
            // Go to the next word.
            word_index++;
            // Reset bits in next word.
            pcm_word = 0;
        }
        // Shift to the next bit.
        word_bit_pos--;
        byte_bit_pos--;
    }

    // Store shift stage in PCM line.
    pcm_line->shift_stage = shift_sw;
    // Calculate new CRCC for the data.
    pcm_line->calcCRC();
}

//------------------------ Calculate PPB, perform binarization and data fill-up,
//------------------------ calculate and compare CRCC, perform pixel-shift if CRCC failes (if enabled).
uint8_t STC007Binarizer::readPCMdata(STC007Line *pcm_line, uint8_t thread_id, bool no_log)
{
    uint8_t shift_try_cnt;
    //----------- STEP 8: Calculate PPB, perform binarization and data fill-up,
    //----------- calculate and compare CRCC, perform pixel-shift if CRCC failes if enabled.
    // Check if PCM pattern was detected.
    if(pcm_line->hasMarkers()==true)
    {
        // Calculate PPB.
        pcm_line->updatePPB();
#if LB_EN_DBG_OUT
        if(((log_level&LOG_PROCESS)!=0)&&(no_log!=true))
        {
            QString log_line;
            log_line.sprintf("[LB-%02u] Calculated PPB: %u.%u", thread_id, pcm_line->getPPB(), pcm_line->getPPBfrac());
            qInfo()<<log_line;
        }
#endif

        // Set counter outside of the pixel shift cycle limits.
        shift_try_cnt = 4;
        if(do_pixel_shift==true)
        {
            // Set the first pixel shift cycle.
            shift_try_cnt = 0;
        }

        // Pixel offset retry cycle.
        do
        {
            // Binarize the video line, fill up data words, calculate CRC.
            fillDataWords(pcm_line, shift_try_cnt);

            // Stop pixel shifting if CRC is OK.
            if(pcm_line->isCRCvalid()==true) break;

            // Proceed with pixel shifting retrying.
            shift_try_cnt++;
            // Re-store shift stage in PCM line (for indicating failed attempts).
            //pcm_line->shift_stage = shift_try_cnt;
        }
        while(shift_try_cnt<3);

        // Check results.
        if(pcm_line->isCRCvalid()==true)
        {
            // Good reference level.
            return REF_CRC_OK;
        }
        else
        {
            // PCM was detected, but CRC is no good.
            return REF_BAD_CRC;
        }
    }
    else
    {
        // No markers - no PCM data.
        return REF_NO_PCM;
    }
}

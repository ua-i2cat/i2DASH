#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <gpac/isomedia.h>
#include <gpac/internal/isomedia_dev.h>
#include <libavcodec/avcodec.h>

#include "error.h"

typedef struct {
    int segment_number;
    int fragment_number;
    int frame_number;

    /** segment duration in ms */
    int segment_duration;

    int segment_marker;

    int frames_per_sample;
    int samples_per_fragment;
    int fragments_per_segment;
    int frames_per_fragment;
    int frames_per_segment;

    uint64_t fragment_dts;
    
    float frame_rate;
    AVCodecContext *avcodeccontext;

    FILE *p_file;

    char *path;
    char *segment_path;
 
    GF_ISOFile *file;
    GF_ISOSample *sample;
    GF_AVCConfig *avccfg;
} i2DASHContext;

i2DASHError i2dash_context_reconfigure(i2DASHContext *context, int seg_nb, int frag_nb, int frame_nb, int seg_dur, float fps, char * dest_path, AVCodecContext * avccont);

/**
 * @brief       initialize an i2DASHContext.
 *
 * @param path  path to a new file 
 * @note        default segment duration: 1000ms.
 * @note        default frame rate: 24fps.
 * @note        by default, one fragment per segment.
 * @return      on success, a pointer to the new i2DASHContext.
 * @return      NULL if the call was not succesful.
 */
i2DASHContext *i2dash_context_new(const char *path);

/**
 * @brief   closes the ISOM file (and writes it to disk) and free the
 *          resources assigned to a i2DASHContext.
 * @post    the provided pointer's memory address should be considered
 *          as unallocated memory.
 * @return  i2DASH_OK if the file was correctly closed.
 * @return  i2DASH_ERROR if something may have gone wrong.
 */
i2DASHError i2dash_context_free(i2DASHContext *context);

/**
 * @brief   updates the frame rate and the related parameters inside the
 *          context structure.
 * @param   context pointer the context structure to update
 * @param   frame_rate new frame rate to consider
 * @return  i2DASH_BAD_PARAM if the <em>frame_rate</em> is not valid (no
 *          changes are made then). i2DASH_OK otherwise.
 */
i2DASHError i2dash_context_update_frame_rate(i2DASHContext *context, float frame_rate);

#endif

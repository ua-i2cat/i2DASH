#ifndef _I2DASH_H_
#define _I2DASH_H_

#include "context.h"
#include "sample.h"
#include "error.h"


/**
 * @brief writes a new frame to the context's ISOM file
 *
 * @param context context that identifies the segmentation in process
 * @param buffer buffer containing the NALs forming the frame
 * @param buffer_len length in bytes of buffer
 *
 * @return i2DASH_OK if no error was found.
 * @return i2DASH_ERROR otherwise.
 */

i2DASHError i2dash_write_init(i2DASHContext *context);

i2DASHError i2dash_write(i2DASHContext *context, const char *buffer, int buffer_len);

i2DASHError i2dash_close(i2DASHContext *context);

#endif

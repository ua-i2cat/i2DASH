#include "i2dash.h"
#include "segment.h"
#include "fragment.h"
#include "debug.h"

static GF_Err avc_import_ffextradata(const u8 *extradata, const u64 extradata_size, GF_AVCConfig *dstcfg);

i2DASHError i2dash_write_init(i2DASHContext *context, GF_ISOFile *input)
{
    GF_Err err;
    GF_AVCConfig *avccfg;
	AVCodecContext * p_video_codec_ctx = context->avcodeccontext;

    u32 description_index;
    u32 timescale = context->frame_rate;
	u32 track;

    char segment_path[256];
    bzero(segment_path, 256);

    i2dash_debug_msg("Starting init segment");
    int ret = sprintf(segment_path, "%s_init.mp4",
                      (const char *)context->path);
    if (ret < 0) {
        i2dash_debug_err("init segment");
        return i2DASH_ERROR;
	}

    i2dash_debug_msg("init segment: %s", segment_path);

    avccfg = gf_odf_avc_cfg_new();
    if (!avccfg) {
        i2dash_debug_err("Cannot create AVCConfig");
        return i2DASH_ERROR;
    }

	err = avc_import_ffextradata(p_video_codec_ctx->extradata, p_video_codec_ctx->extradata_size, avccfg);
    if (err != GF_OK) {
        i2dash_debug_err("avc_import_ffextradata: %s",
                gf_error_to_string(err));
		gf_odf_avc_cfg_del(avccfg);
        return i2DASH_ERROR;
    }
    
    //avccfg->configurationVersion = 1;
    
    i2dash_debug_msg("segment_path before check if is null: %s", segment_path);
    if(segment_path != NULL) {
        context->file = gf_isom_open(segment_path, GF_ISOM_OPEN_WRITE, NULL);
        if (context->file == NULL) {
            i2dash_debug_err("gf_isom_open: %s", segment_path);
            return i2DASH_ERROR;
        }
    }

	track = gf_isom_new_track(context->file, 1, GF_ISOM_MEDIA_VISUAL, p_video_codec_ctx->time_base.den/*timescale*/);
    if(!track) {
        i2dash_debug_err("gf_isom_new_track: %d", (int)track);
        return i2DASH_ERROR;
    }

    err = gf_isom_set_track_enabled(context->file, 1, 1);
    if (err != GF_OK) {
        i2dash_debug_err("gf_isom_set_track_enabled: %s",
                gf_error_to_string(err));
        return i2DASH_ERROR;
    }
    
    err = gf_isom_avc_config_new(context->file, 1, avccfg, NULL, NULL, &description_index);
    if (err != GF_OK) {
        i2dash_debug_err("gf_isom_avc_config_new: %s",
                        gf_error_to_string(err));
        return i2DASH_ERROR;
    }

    gf_odf_avc_cfg_del(avccfg);

	gf_isom_set_visual_info(context->file, track, description_index, p_video_codec_ctx->width, p_video_codec_ctx->height);

/*
	err = gf_isom_modify_alternate_brand(context->file, GF_4CC('d','a','s','h'), 1);
	if (err != GF_OK) {
	i2dash_debug_err("gf_isom_modify_alternate_brand: %s", gf_error_to_string(err));
	return i2DASH_ERROR;
	}*/
	
	err = gf_isom_setup_track_fragment(context->file, 1, 1, 1, 0, 0, 0, 0);
    if(err != GF_OK) {
        i2dash_debug_err("gf_isom_setup_track_fragment: %s",
                                gf_error_to_string(err));
        return i2DASH_ERROR;
    }

    err = gf_isom_finalize_for_fragment(context->file, 1);
    if (err != GF_OK) {
        i2dash_debug_err("gf_isom_finalize_for_fragment: %s",
                        gf_error_to_string(err));
        return i2DASH_ERROR;
    }

    context->fragment_dts = 0;
	sprintf(segment_path, "%s_%d.m4s", (const char *)context->path, 1);
	i2dash_debug_msg("segment_path: %s", segment_path);

    err = gf_isom_start_segment(context->file, segment_path, 1);
    if (err != GF_OK) {
        i2dash_debug_err("gf_isom_start_segment: %s", 
                            gf_error_to_string(err));
        return i2DASH_ERROR;
    }
	i2dash_debug_msg("Init finished: %s\n", segment_path);
	
    return i2DASH_OK;
}

i2DASHError i2dash_write(i2DASHContext *context, const char *buffer, int buffer_len)
{   
    int next_frame_number = context->frame_number + 1;
    
    // init file with moov creation
    if (context->segment_number == 0 && context->frame_number == 0) {
        char segment_path[256];
        bzero(segment_path, 256);

        i2dash_debug_msg("Starting init segment");
        int ret = sprintf(segment_path, "%s_init.mp4",
                          (const char *)context->path);
        if (ret < 0) {
            i2dash_debug_err("init segment");
            return i2DASH_ERROR;
        }

        if (i2dash_segment_new(context, segment_path) != i2DASH_OK) {
            i2dash_debug_err("i2dash_segment_new: KO");
            return i2DASH_ERROR;
        }
        //i2dash_debug_msg("i2dash_segment_new: OK");

        if(i2dash_fragment_setup(context) != i2DASH_OK){
            i2dash_debug_err("i2dash_fragment_setup: KO");
            return i2DASH_ERROR;
        }

        //i2dash_debug_msg("i2dash_fragment_setup: OK");
        i2dash_debug_msg("Init finished: %s\n", segment_path);

        i2dash_debug_msg("Starting segment %d", context->segment_number);
        i2dash_debug_msg("Starting fragment %d", context->fragment_number);
        int ret2 = sprintf(segment_path, "%s_%d.m4s",
                          (const char *)context->path,
                          context->segment_number);
        if (ret2 < 0) {
            i2dash_debug_err("segment: %d", context->segment_number);
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("start segment: %s", segment_path);

        // start segments
        if(i2dash_segment_start(context, segment_path) != i2DASH_OK){
            i2dash_debug_err("i2dash_segment_start: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_segment_start: OK");
        context->segment_number++;
        context->fragment_number++;
    }
    else if (next_frame_number % context->frames_per_segment == 0) {
        if(i2dash_fragment_close(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_fragment_close: KO");
            return i2DASH_ERROR;
        }
        //i2dash_debug_msg("i2dash_fragment_close: OK");

        if (i2dash_segment_close(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_segment_close: KO");
            return i2DASH_ERROR;
        }
        //i2dash_debug_msg("i2dash_segment_close: OK\n");

        i2dash_debug_msg("finished fragment: %d", context->fragment_number-1);
        i2dash_debug_msg("finished segment: %d\n", context->segment_number-1);

        context->frame_number = 0;

        i2dash_debug_msg("Starting segment %d", context->segment_number);
        i2dash_debug_msg("Starting fragment %d", context->fragment_number);

        char segment_path[256];
        bzero(segment_path, 256);

        int ret = sprintf(segment_path, "%s_%d.m4s",
                          (const char *)context->path,
                          context->segment_number);
        if (ret < 0) {
            i2dash_debug_err("segment: %d", context->segment_number);
            return i2DASH_ERROR;
        }

        if(i2dash_segment_start(context, segment_path) != i2DASH_OK){
            i2dash_debug_err("i2dash_segment_start: KO");
            return i2DASH_ERROR;
        }
        //i2dash_debug_msg("i2dash_segment_start: OK");

        context->fragment_number++;
        context->segment_number++;
    }

    if(i2dash_fragment_write(context, buffer, buffer_len, 0, 0) != i2DASH_OK) {
        i2dash_debug_err("i2dash_fragment_write: KO");
        return i2DASH_ERROR;
    }
    //i2dash_debug_msg("i2dash_fragment_write: OK");

    i2dash_debug_msg("Written frame: %d\n", context->frame_number);
    context->frame_number++;

    return i2DASH_OK;
}

static GF_Err avc_import_ffextradata(const u8 *extradata, const u64 extradata_size, GF_AVCConfig *dstcfg)
{
	u8 nal_size;
	AVCState avc;
	GF_BitStream *bs;
	if (!extradata || !extradata_size)
		return GF_BAD_PARAM;
	bs = gf_bs_new(extradata, extradata_size, GF_BITSTREAM_READ);
	if (!bs)
		return GF_BAD_PARAM;
	if (gf_bs_read_u32(bs) != 0x00000001) {
		gf_bs_del(bs);
		return GF_BAD_PARAM;
	}
	//SPS
	{
		s32 idx;
		char *buffer = NULL;
		const u64 nal_start = 4;
		nal_size = gf_media_nalu_next_start_code_bs(bs);
		if (nal_start + nal_size > extradata_size) {
			gf_bs_del(bs);
			return GF_BAD_PARAM;
		}
		buffer = (char*)gf_malloc(nal_size);
		gf_bs_read_data(bs, buffer, nal_size);
		gf_bs_seek(bs, nal_start);
		if ((gf_bs_read_u8(bs) & 0x1F) != GF_AVC_NALU_SEQ_PARAM) {
			gf_bs_del(bs);
			gf_free(buffer);
			return GF_BAD_PARAM;
		}

		idx = gf_media_avc_read_sps(buffer, nal_size, &avc, 0, NULL);
		if (idx < 0) {
			gf_bs_del(bs);
			gf_free(buffer);
			return GF_BAD_PARAM;
		}

		dstcfg->configurationVersion = 1;
		dstcfg->profile_compatibility = avc.sps[idx].prof_compat;
		dstcfg->AVCProfileIndication = avc.sps[idx].profile_idc;
		dstcfg->AVCLevelIndication = avc.sps[idx].level_idc;
		dstcfg->chroma_format = avc.sps[idx].chroma_format;
		dstcfg->luma_bit_depth = 8 + avc.sps[idx].luma_bit_depth_m8;
		dstcfg->chroma_bit_depth = 8 + avc.sps[idx].chroma_bit_depth_m8;

		{
			GF_AVCConfigSlot *slc = (GF_AVCConfigSlot*)gf_malloc(sizeof(GF_AVCConfigSlot));
			slc->size = nal_size;
			slc->id = idx;
			slc->data = buffer;
			gf_list_add(dstcfg->sequenceParameterSets, slc);
		}
	}

	//PPS
	{
		s32 idx;
		char *buffer = NULL;
		const u64 nal_start = 4 + nal_size + 4;
		gf_bs_seek(bs, nal_start);
		nal_size = gf_media_nalu_next_start_code_bs(bs);
		if (nal_start + nal_size > extradata_size) {
			gf_bs_del(bs);
			return GF_BAD_PARAM;
		}
		buffer = (char*)gf_malloc(nal_size);
		gf_bs_read_data(bs, buffer, nal_size);
		gf_bs_seek(bs, nal_start);
		if ((gf_bs_read_u8(bs) & 0x1F) != GF_AVC_NALU_PIC_PARAM) {
			gf_bs_del(bs);
			gf_free(buffer);
			return GF_BAD_PARAM;
		}
		
		idx = gf_media_avc_read_pps(buffer, nal_size, &avc);
		if (idx < 0) {
			gf_bs_del(bs);
			gf_free(buffer);
			return GF_BAD_PARAM;
		}

		{
			GF_AVCConfigSlot *slc = (GF_AVCConfigSlot*)gf_malloc(sizeof(GF_AVCConfigSlot));
			slc->size = nal_size;
			slc->id = idx;
			slc->data = buffer;
			gf_list_add(dstcfg->pictureParameterSets, slc);
		}
	}

	gf_bs_del(bs);
	return GF_OK;
}


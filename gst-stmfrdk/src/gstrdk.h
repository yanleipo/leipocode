#ifndef __GST_RDK_H__
#define __GST_RDK_H__

#include <gst/gst.h>
#include <config.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>

//#define SUBTITLE_TELTEXT_SUPPORT

//#define FILE_SUPPORT
//#define HTTPSRC_SUPPORT

//#define USE_MPEGTSDEMUX

#define ZAPPING_INTERVAL_IN_SEC 14

typedef struct _STGSTPLAYER_T_
{
  GstElement *m_pipeline;       /* holds the pipeline */
	
	GstElement *m_source;         /* holds the source bin */
	GstElement *m_tee;            /* holds the tee element */
	GstElement *m_queue;          /* holds the queue element */
	GstElement *m_sink;           /* holds the sink bin */
	
	GStaticRecMutex m_mutex;      /* protect pipeline */
	GstBus     *bus;              /* bus watch */
	guint      m_bus_event_source_id;
	
	/* sink bin element */
	GstElement *demux;
	GstElement *video_decoder;
	GstElement *video_sink;
	GstElement *audio_decoder;
	GstElement *audio_sink;
	GstElement *video_queue;
	GstElement *audio_queue;

#ifdef SUBTITLE_TELTEXT_SUPPORT
	GstElement* subt_decoder;
	GstElement* ffmpegcsp;
	GstElement* vid_dmx_queue;
	GstElement* aud_dmx_queue;
	GstElement* subt_dmx_queue;
	gboolean ttx_pad_linked;
#endif

	gboolean audio_pad_linked;
	gboolean video_pad_linked;
	
	GMainLoop *main_loop;         /* main loop */	
	
	GstTask *input_task;          /* keyboard task */
	GStaticRecMutex *input_task_mutex; /* keyboard task mutex */
	
	gchar      uri[255];
} STGstPlayer;

#endif

#include "gstrdk.h"

static void onDemuxSrcPadAdded (GstElement* element, GstPad *demux_src_pad, gpointer data)
{  
	STGstPlayer *player = (STGstPlayer *) data;
	
	gchar *name;
	GstCaps* caps;
	
	caps = gst_pad_get_caps(demux_src_pad);
	name = gst_structure_get_name(gst_caps_get_structure (caps, 0));
	gst_caps_unref(caps);
	
	if (strstr(name,"video/") != NULL) {
	  GstPad* sink_pad = gst_element_get_static_pad(player->video_decoder, "sink");
		
		if (!GST_PAD_LINK_SUCCESSFUL(gst_pad_link(demux_src_pad, sink_pad)))
		{
			printf("Failed to connect video pad\n");
		}
		else
		{
			printf("Successfully linked video\n");
		}
		gst_object_unref(sink_pad);
		
	} else if (strstr(name,"audio/") != NULL) {
	  GstPad* sink_pad = gst_element_get_static_pad(player->audio_decoder, "sink");
		
		if (!GST_PAD_LINK_SUCCESSFUL(gst_pad_link(demux_src_pad, sink_pad)))
		{
			printf("Failed to connect audio pad\n");
		}
		else
		{
			printf("Successfully linked audio\n");
		}
		gst_object_unref(sink_pad);
	}
  return;
}

gboolean create_pipeline (STGstPlayer * player)
{  
	int blocksize = 188*192*4;
	GstElement *m_pipeline;	
	GstElement *source;
	GstElement *demux;
	GstElement *video_decoder;
	GstElement *video_sink;
	GstElement *audio_decoder;
	GstElement *audio_sink;
	GstElement *video_queue;
	GstElement *audio_queue;
	
	
	m_pipeline = gst_pipeline_new("pipeline");
	player->m_pipeline = m_pipeline;
	
	source = gst_element_factory_make("souphttpsrc", "soup");
	player->m_source = source;
	
	demux = gst_element_factory_make ("mpegtsdemux", "player_demux");
	player->demux = demux;

	video_decoder = gst_element_factory_make ("stvideo", "player_vdec");
	player->video_decoder = video_decoder;
	
	video_sink = gst_element_factory_make ("stdisplaysink", "player_vsink");
	player->video_sink = video_sink;

	video_queue = gst_element_factory_make ("queue", "player_vqueue");
	player->video_queue = video_queue;
	
	audio_decoder = gst_element_factory_make ("staudio", "player_adec");
	player->audio_decoder = audio_decoder;
	
	audio_sink = gst_element_factory_make ("staudiosink", "player_asink");
	player->audio_sink = audio_sink;

	audio_queue = gst_element_factory_make ("queue", "player_aqueue");
	player->audio_queue = audio_queue;	
	
	g_object_set(G_OBJECT (player->m_source), "blocksize", blocksize,  NULL); 
	g_object_set(G_OBJECT (player->m_source), "is-live", TRUE,  NULL);
	g_object_set(G_OBJECT(player->m_source), "location", player->uri, NULL);
	
	g_object_set(G_OBJECT(player->video_sink), "async", FALSE, NULL);		
	g_object_set(G_OBJECT(player->audio_sink), "async", FALSE, NULL);
	

	gst_bin_add_many (GST_BIN(player->m_pipeline), source, demux, video_decoder, video_queue, video_sink, audio_decoder, audio_queue, audio_sink, NULL);

	gst_element_link(source, demux);
	gst_element_link_many(video_decoder, video_queue, video_sink, NULL);
	gst_element_link_many(audio_decoder, audio_queue, audio_sink, NULL);
	gst_element_link_many(demux, video_decoder, video_decoder, NULL);
	
	/* connect to signal */
	g_signal_connect (demux, "pad-added", G_CALLBACK (onDemuxSrcPadAdded), player);

	return TRUE;
}

gboolean
set_pipeline_state (STGstPlayer * player, GstState state)
{
  GstStateChangeReturn ret;
	GstElement *m_pipeline = player->m_pipeline;
	
	ret = gst_element_set_state (m_pipeline, state);
	switch (ret)
	{
	  case GST_STATE_CHANGE_FAILURE:
		  return FALSE;
		case GST_STATE_CHANGE_SUCCESS:
		  break;
		case GST_STATE_CHANGE_ASYNC:
		  break;
		case GST_STATE_CHANGE_NO_PREROLL:
		  break;
		default:
		  return FALSE;
	}
	
	/* to wait the state change to complete, timeout value 10 seconds */
	ret = gst_element_get_state (m_pipeline, NULL, NULL, (10 * GST_SECOND));
	if (ret == GST_STATE_CHANGE_FAILURE) {
	  printf ("pipeline failed to state change %d\n", state);
		return FALSE;
	} else if (ret == GST_STATE_CHANGE_ASYNC) {
	  printf ("pipeline Too long time changing to state %d\n", state);
		return FALSE;
	}
	
	return TRUE;
}

gboolean destroy_pipeline (STGstPlayer * player)
{
	GstElement *m_pipeline = player->m_pipeline;	

	gst_object_unref (GST_OBJECT(m_pipeline));
	
	player->m_pipeline = NULL;
	
	/* the source element has been unref when unref the pipeline */
	player->m_source = NULL;

	
  return TRUE;
}

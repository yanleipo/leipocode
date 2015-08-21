#include "gstrdk.h"

static GstElement *
create_source_element_1 (STGstPlayer * player)
{
  /* create source bin - see HNSourcePrivate::createElement() */
  GstElement *bin = gst_bin_new("hn_source_bin");
	if (bin == NULL) {
	  g_print ("failed to create source bin\n");
		return NULL;
	}
	
	gst_element_add_pad(bin, gst_ghost_pad_new_no_target("src", GST_PAD_SRC));
	
	return bin;
}

static gboolean
create_source_element_2 (STGstPlayer * player)
{
  GstElement *source;
  GstElement *bin = player->m_source;
	GstPad *pad;
	GstPad *ghost_pad;
	
	int blocksize = 188*192*4;
	
	/* create souphttpsrc element in source bin */
#ifdef FILE_SUPPORT
	source = gst_element_factory_make("filesrc", "soup");
#else
#ifdef HTTPSRC_SUPPORT
	source = gst_element_factory_make("httpsrc", "curl");
#else
	source = gst_element_factory_make("souphttpsrc", "soup");
#endif
#endif

	if (source == NULL) {
	  g_print ("failed to create souphttpsrc element\n");
		return FALSE;
	}
	
	gst_bin_add(GST_BIN(bin), source);
	
	/* set property */
	g_object_set(G_OBJECT (source), "blocksize", blocksize,  NULL); 
	g_object_set(G_OBJECT (source), "is-live", TRUE,  NULL); 
	
	/* point bin ghost pad to last element source pad */
	pad = gst_element_get_static_pad (source, "src");
	ghost_pad = gst_element_get_static_pad (bin, "src");
	if (gst_ghost_pad_set_target ((GstGhostPad*)ghost_pad, pad) == FALSE) {
	  g_print ("failed to link souphttpsrc element pad to source bin ghost pad\n");
		return FALSE;
	}
	gst_object_unref (ghost_pad);
	gst_object_unref (pad);
	
	/* set uri */
	g_object_set(G_OBJECT(source), "location", player->uri, NULL);
	
	return TRUE;
}

static GstElement *
create_sink_element_1 (STGstPlayer * player)
{
  /* create playsink bin - see MediaPlayerSinkPrivate::createElement() */
	GstElement* bin = gst_bin_new("player_bin");
	
	gst_element_add_pad(bin, gst_ghost_pad_new_no_target("sink", GST_PAD_SINK));
	
	return bin;
}

static void onDemuxSrcPadAdded (GstElement* element, GstPad *demux_src_pad, gpointer data)
{  
	STGstPlayer *player = (STGstPlayer *) data;
	
	gchar *name;
	GstCaps* caps;
	
	caps = gst_pad_get_caps(demux_src_pad);
	name = gst_structure_get_name(gst_caps_get_structure (caps, 0));
	gst_caps_unref(caps);
	
	//g_print ("got demux pad caps %s\n", name);
	
	if (strstr(name,"video/") != NULL) {
#ifdef SUBTITLE_TELTEXT_SUPPORT
	  GstPad* sink_pad = gst_element_get_static_pad(player->vid_dmx_queue, "sink");
#else
	  GstPad* sink_pad = gst_element_get_static_pad(player->video_decoder, "sink");
#endif
		
		if (player->video_pad_linked == TRUE) {
		  g_print ("video already linked - no link\n");
			return;
		}
		
		if (!GST_PAD_LINK_SUCCESSFUL(gst_pad_link(demux_src_pad, sink_pad)))
		{
			g_print("Failed to connect video pad\n");
		}
		else
		{
			//g_print("Successfully linked video\n");
      player->video_pad_linked = TRUE;			
		}
		gst_object_unref(sink_pad);
		
	} else if (strstr(name,"audio/") != NULL) {
#ifdef SUBTITLE_TELTEXT_SUPPORT
	  GstPad* sink_pad = gst_element_get_static_pad(player->aud_dmx_queue, "sink");
#else
	  GstPad* sink_pad = gst_element_get_static_pad(player->audio_decoder, "sink");
#endif
		
		if (player->audio_pad_linked == TRUE) {
		  g_print ("audio already linked - no link\n");
			return;
		}
		
		if (!GST_PAD_LINK_SUCCESSFUL(gst_pad_link(demux_src_pad, sink_pad)))
		{
			g_print("Failed to connect audio pad\n");
		}
		else
		{
			//g_print("Successfully linked audio\n");
      player->audio_pad_linked = TRUE;			
		}
		gst_object_unref(sink_pad);
	}
#ifdef SUBTITLE_TELTEXT_SUPPORT
	else if ((strstr(name,"subpicture/") != NULL) || (strstr(name,"teletext") != NULL)) {
		GstPad* sink_pad = gst_element_get_static_pad(player->subt_dmx_queue, "sink");
		if (player->ttx_pad_linked == TRUE) {
		  g_print ("subtitle already linked - no link\n");
			return;
		}
		
		if (!GST_PAD_LINK_SUCCESSFUL(gst_pad_link(demux_src_pad, sink_pad)))
		{
			g_print("Failed to connect subtitle pad\n");
		}
		else
		{
			g_print("Successfully linked subtitle\n");
			player->ttx_pad_linked = TRUE;			
		}
		gst_object_unref(sink_pad);
	}
#endif	
  return;
}


static gboolean
create_sink_element_2 (STGstPlayer * player)
{
  GstElement *bin = player->m_sink;
	GstPad *pad;
	GstPad *ghost_pad;
	
	GstElement *demux;
	GstElement *video_decoder;
	GstElement *video_sink;
	GstElement *audio_decoder;
	GstElement *audio_sink;
	GstElement *video_queue;
	GstElement *audio_queue;
	
#ifdef USE_MPEGTSDEMUX
	demux = gst_element_factory_make ("mpegtsdemux", "player_demux");
	if (demux == NULL) {
	  g_print ("failed to create mpegtsdemux element\n");
		return FALSE;
	}
#else
	demux = gst_element_factory_make ("stts_demux", "player_demux");
	if (demux == NULL) {
	  g_print ("failed to create stts_demux element\n");
		return FALSE;
	}
#endif
	player->demux = demux;
	/* connect to signal */
	g_signal_connect (demux, "pad-added", G_CALLBACK (onDemuxSrcPadAdded), player);
	
	video_decoder = gst_element_factory_make ("stvideo", "player_vdec");
	if (video_decoder == NULL) {
	  g_print ("failed to create stvideo element\n");
		return FALSE;
	}
	player->video_decoder = video_decoder;
	
	video_sink = gst_element_factory_make ("stdisplaysink", "player_vsink");
	if (video_sink == NULL) {
	  g_print ("failed to create stdisplaysink element\n");
		return FALSE;
	}
	player->video_sink = video_sink;
	g_object_set(G_OBJECT(player->video_sink), "async", FALSE, NULL);
	
#ifdef SUBTITLE_TELTEXT_SUPPORT
	player->vid_dmx_queue = gst_element_factory_make ("queue", "dmx_vsink");
	if (player->vid_dmx_queue == NULL) {
	  g_print ("failed to create queue element\n");
		return FALSE;
	}
	player->aud_dmx_queue = gst_element_factory_make ("queue", "dmx_asink");
	if (player->aud_dmx_queue == NULL) {
	  g_print ("failed to create queue element\n");
		return FALSE;
	}
	player->subt_dmx_queue = gst_element_factory_make ("queue", "dmx_tsink");
	if (player->subt_dmx_queue == NULL) {
	  g_print ("failed to create queue element\n");
		return FALSE;
	}
	player->subt_decoder = gst_element_factory_make("sttextoverlay", "player_subdec");
	if (player->subt_decoder == NULL) {
		g_print("Failed to instantiate subtitle decoder (sttextoverlay)\n");
		return FALSE;
	}
	player->ffmpegcsp = gst_element_factory_make("ffmpegcolorspace", "player_csp");
	if (player->ffmpegcsp == NULL) {
		g_print("Failed to instantiate subtitle colour space conversion (ffmpegcolorspace)\n");
		return FALSE;
	}
#endif	
	
	video_queue = gst_element_factory_make ("queue2", "player_vqueue");
	if (video_queue == NULL) {
	  g_print ("failed to create queue2 element\n");
		return FALSE;
	}
	player->video_queue = video_queue;
	
	audio_decoder = gst_element_factory_make ("staudio", "player_adec");
	if (audio_decoder == NULL) {
	  g_print ("failed to create staudio element\n");
		return FALSE;
	}
	player->audio_decoder = audio_decoder;
	
	audio_sink = gst_element_factory_make ("staudiosink", "player_asink");
	if (audio_sink == NULL) {
	  g_print ("failed to create staudiosink element\n");
		return FALSE;
	}
	player->audio_sink = audio_sink;
	g_object_set(G_OBJECT(player->audio_sink), "async", FALSE, NULL);
	
	audio_queue = gst_element_factory_make ("queue2", "player_aqueue");
	if (audio_queue == NULL) {
	  g_print ("failed to create queue2 element\n");
		return FALSE;
	}
	player->audio_queue = audio_queue;	
	
	/* add all into bin */
#ifdef SUBTITLE_TELTEXT_SUPPORT
	gst_bin_add_many (GST_BIN(bin), demux, player->vid_dmx_queue, player->video_decoder, player->subt_dmx_queue, player->subt_decoder, player->ffmpegcsp, player->video_queue, player->video_sink, player->aud_dmx_queue, player->audio_decoder, player->audio_queue, player->audio_sink, NULL);
#else
	gst_bin_add_many (GST_BIN(bin), demux, video_decoder, video_queue, video_sink, audio_decoder, audio_queue, audio_sink, NULL);
#endif
	
	/* link video */
#ifdef SUBTITLE_TELTEXT_SUPPORT
	if (gst_element_link_many(player->vid_dmx_queue, player->video_decoder, player->subt_decoder, player->ffmpegcsp, player->video_queue, player->video_sink, NULL) == FALSE) {
#else
	if (gst_element_link_many(video_decoder, video_queue, video_sink, NULL) == FALSE) {
#endif
	  g_print ("failed to link video decoder queue sink\n");
		return FALSE;
	}

	/* link audio */
#ifdef SUBTITLE_TELTEXT_SUPPORT
	if (gst_element_link_many(player->aud_dmx_queue, player->audio_decoder, player->audio_queue, player->audio_sink, NULL) == FALSE) {
#else
	if (gst_element_link_many(audio_decoder, audio_queue, audio_sink, NULL) == FALSE) {
#endif
	  g_print ("failed to link audio decoder queue sink\n");
		return FALSE;
	}
	
#ifdef SUBTITLE_TELTEXT_SUPPORT
	/* link subtitle */
	if (gst_element_link_many(player->subt_dmx_queue, player->subt_decoder, NULL) == FALSE) {
	  g_print ("failed to link audio decoder queue sink\n");
		return FALSE;
	}
	g_object_set (G_OBJECT (player->subt_decoder), "silent", TRUE, NULL);
#endif
	
	/* link demux sink pad to ghost pad */
	pad = gst_element_get_static_pad (demux, "sink");
	ghost_pad = gst_element_get_static_pad (bin, "sink");
	if (gst_ghost_pad_set_target ((GstGhostPad*)ghost_pad, pad) == FALSE) {
	  g_print ("failed to link demux element pad to sink bin ghost pad\n");
		return FALSE;
	}
	gst_object_unref (ghost_pad);
	gst_object_unref (pad);	
	
	return TRUE;
}


static gboolean
addsink (STGstPlayer * player)
{
  GstElement *m_queue;
	GstElement *m_pipeline = player->m_pipeline;
	GstElement *m_sink = player->m_sink;
	GstElement *m_tee = player->m_tee;
	
  /* create queue for the sink and connect to tee */
	m_queue = gst_element_factory_make("queue", NULL);
	if (m_queue == NULL) {
	  g_print ("failed to create queue element\n");
		return FALSE;
	}
	player->m_queue = m_queue;
	
	/* keep queue property as default */
	
	/* add queue into pipeline */
	gst_bin_add(GST_BIN(m_pipeline), m_queue);
	gst_element_sync_state_with_parent (m_queue);
	
	/* add sink element into pipeline */
	gst_bin_add (GST_BIN(m_pipeline), m_sink);	
	
	/* link the sink element to queue */
	gst_element_link (m_queue, m_sink);
	gst_element_sync_state_with_parent (m_sink);
	gst_element_link (m_tee, m_queue);	
	
	return TRUE;
}

static gboolean 
handleBusMessage (GstBus * bus, GstMessage * msg, gpointer data)
{
  STGstPlayer *player = *((STGstPlayer **) data);
	
	/* check if msg is NULL */
  if (msg == NULL) {
    g_print ("In %s, msg is NULL!", __FUNCTION__);
    return FALSE;
  }
	
	switch (GST_MESSAGE_TYPE (msg)) {
	  case GST_MESSAGE_EOS:
		  g_print ("End of stream ---> Press 'q' to quit\n");
		  break;
			
		case GST_MESSAGE_ERROR:
		{
		  GError *err;
			gchar *debug;
			
			gst_message_parse_error (msg, &err, &debug);
			g_print ("ERROR: gst-rdk received UNKNOWN error %d (%s), from %s\n", err->code, err->message, debug);
		}
		  break;
			
		case GST_MESSAGE_STATE_CHANGED:
		{
		  GstState old_state;
			GstState new_state;
			GstState pending_state;
			
			gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);

#if 0			
			g_print ("Element %s changed state from %s to %s\n", GST_OBJECT_NAME(msg->src), 
			                                                     gst_element_state_get_name(old_state), 
																													 gst_element_state_get_name(new_state));
#endif																													 
		}
		  break;
		
		case GST_MESSAGE_ASYNC_DONE:
		{
#if 0		
		  g_print ("Element %s completed async status change\n", GST_MESSAGE_SRC_NAME(msg));
#endif			
		}
		  break;
		
		default:
		  break;
	}
	
	return TRUE;
}

gboolean create_pipeline (STGstPlayer * player)
{
  GStaticRecMutex *m_mutex = &(player->m_mutex);
	GstElement *m_pipeline;
	GstElement *m_source;
	GstElement *m_tee;
	GstBus *bus;
	guint m_bus_event_source_id;
	GstElement *m_sink;	
	
  /* create pipeline - see RMFMediaSourcePrivate::init() */
	g_static_rec_mutex_init (m_mutex);
	
	g_static_rec_mutex_lock (m_mutex);
	
	m_pipeline = gst_pipeline_new("pipeline");
	player->m_pipeline = m_pipeline;
	
	/* create source element step 1 - see HNSourcePrivate::createElement() */
	m_source = (GstElement*) create_source_element_1(player);
	player->m_source = m_source;
	
	/* create tee element */
	m_tee = gst_element_factory_make("tee", "source_tee");
	player->m_tee = m_tee;

  if (m_pipeline == NULL || m_source == NULL || m_tee == NULL) {
	  g_print ("failed to create pipeline || source || tee \n");
	  return FALSE;
	}
	
	/* add source and tee into pipeline */
	gst_bin_add_many(GST_BIN(m_pipeline), m_source, m_tee, NULL);
	
	/* link source and tee */
	if (gst_element_link(m_source, m_tee) == FALSE) {
	  g_print ("failed to link source and tee\n");
		return FALSE;
	}
	
	/* create pipeline bus and add watch */
	bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
	m_bus_event_source_id = gst_bus_add_watch(bus, handleBusMessage, player);
	player->m_bus_event_source_id = m_bus_event_source_id;
	gst_object_unref(bus);
	
	/* create source element step 2 - HNSourcePrivate::populateBin */
	if (create_source_element_2 (player) == FALSE) {
	  g_print ("failed to create source elements\n");
		return FALSE;
	}
	
	/* create sink element */
	m_sink = (GstElement*) create_sink_element_1(player);
	player->m_sink = m_sink;
	
	if (create_sink_element_2 (player) == FALSE) {
	  g_print ("failed to create playsink elements\n");
		return FALSE;
	}
	
	/* add sink into pipeline - see RMFMediaSourcePrivate::addSink() */
	if (addsink (player) == FALSE) {
	  g_print ("failed to add sink into pipeline\n");
		return FALSE;
	}
	
	g_static_rec_mutex_unlock (m_mutex);
	
	return TRUE;
}

gboolean
set_pipeline_state (STGstPlayer * player, GstState state)
{
  GstStateChangeReturn ret;
	GstElement *m_pipeline = player->m_pipeline;
	
	g_print ("pipeline state change to %d\n", state);
	ret = gst_element_set_state (m_pipeline, state);
	switch (ret)
	{
	  case GST_STATE_CHANGE_FAILURE:
		  //g_print ("GST_STATE_CHANGE_FAILURE\n");
		  return FALSE;
		case GST_STATE_CHANGE_SUCCESS:
		  //g_print ("GST_STATE_CHANGE_SUCCESS\n");
		  break;
		case GST_STATE_CHANGE_ASYNC:
		  //g_print ("GST_STATE_CHANGE_ASYNC\n");
		  break;
		case GST_STATE_CHANGE_NO_PREROLL:
		  //g_print ("GST_STATE_CHANGE_NO_PREROLL\n");
		  break;
		default:
		  //g_print ("GST_STATE_CHANGE Unknown\n");
		  return FALSE;
	}
	
	/* to wait the state change to complete, timeout value 10 seconds */
	ret = gst_element_get_state (m_pipeline, NULL, NULL, (10 * GST_SECOND));
	if (ret == GST_STATE_CHANGE_FAILURE) {
	  g_print ("pipeline failed to state change %d\n", state);
		return FALSE;
	} else if (ret == GST_STATE_CHANGE_ASYNC) {
	  g_print ("pipeline Too long time changing to state %d\n", state);
		return FALSE;
	}
	
	return TRUE;
}

static gboolean
dropsink (STGstPlayer * player)
{
  GstElement *m_pipeline = player->m_pipeline;	
  GstElement *m_sink = player->m_sink;	
	GstElement *m_queue = player->m_queue;
	GstElement *m_tee = player->m_tee;
	
	GstPad* queue_sink_pad;
	GstPad* tee_src_pad;
	
	int i;
	
  /* disconnect sink and source element - see RMFMediaSinkPrivate::setSource (Null) */
	if (m_pipeline == NULL) {
	  printf ("pipeline not initialized\n");
	  return FALSE;
	}
	
	if (m_sink == NULL) {
	  printf ("sink not initialized\n");
		return FALSE;
	}
	
	if (m_queue == NULL) {
	  printf ("queue not initialized\n");
		return FALSE;
	}
	
	/* unlink queue and sink element */
	gst_element_unlink(m_queue, m_sink);
	
	/* change sink element state to NULL */
	gst_element_set_state(m_sink, GST_STATE_NULL);
	
	queue_sink_pad = gst_element_get_static_pad(m_queue, "sink");
  tee_src_pad = gst_pad_get_peer(queue_sink_pad);
	
	if (queue_sink_pad == NULL) {
	  printf ("failed to find queue sink pad\n");
		return FALSE;
	}
	
	if (tee_src_pad == NULL) {
	  printf ("failed to find tee src pad\n");
		return FALSE;
	}	
	
	if (gst_pad_unlink(tee_src_pad, queue_sink_pad) == FALSE) {
	  printf ("failed to unlink tee and queue\n");
		return FALSE;
	}
	
	gst_element_release_request_pad(m_tee, tee_src_pad);
	gst_object_unref(tee_src_pad);
  gst_object_unref(queue_sink_pad);
	
	if (gst_bin_remove(GST_BIN(m_pipeline), m_queue) == FALSE) {
	  printf ("failed to remove queue from pipeline\n");
		return FALSE;
	}
	
	if (gst_bin_remove(GST_BIN(m_pipeline), m_sink) == FALSE) {
	  printf ("failed to remove sink from pipeline\n");
		return FALSE;
	}

	/* from the gstreamer documentation, the element will be unref when gst_bin_remove */
	/* also can see gstreamer test code test/gstbin.c */
	player->m_queue = NULL;
	player->m_sink = NULL;
	
	/* all other internals reset */
	player->demux = NULL;
	player->video_decoder = NULL;
	player->video_sink = NULL;
	player->audio_decoder = NULL;
	player->audio_sink = NULL;
	player->video_queue = NULL;
	player->audio_queue = NULL;
	player->audio_pad_linked = FALSE;
	player->video_pad_linked = FALSE;

#ifdef SUBTITLE_TELTEXT_SUPPORT
	player->subt_decoder = NULL;
	player->ffmpegcsp = NULL;
	player->vid_dmx_queue = NULL;
	player->aud_dmx_queue = NULL;
	player->subt_dmx_queue = NULL;
	player->ttx_pad_linked  = FALSE;
#endif

#if 0	
	if (m_queue == NULL) {
	  printf ("m_queue has been unref by remove from bin \n");
	} else if (GST_IS_OBJECT(m_queue) == FALSE) {
	  printf ("m_queue not object and maybe unref by removing from bin \n");
		m_queue = NULL;
	} else {
	  int ref_cnt;

		ref_cnt = GST_OBJECT_REFCOUNT_VALUE(m_queue);
	  if (ref_cnt > 0) {
	    gst_element_set_state(m_queue, GST_STATE_NULL);
		}
		
		for (i = 0; i < ref_cnt; i++) {
		  gst_object_unref(m_queue);
		}
		
		m_queue = NULL;
	}
#endif
	
  return TRUE;
}

static gboolean
remove_sink_element (STGstPlayer * player)
{
  GstElement *m_sink = player->m_sink;
	
	if (m_sink == NULL) {
	  printf ("sink not initialized\n");
		return FALSE;
	}
	
	gst_object_unref(GST_OBJECT(m_sink));
	
  return TRUE;
}

gboolean destroy_pipeline (STGstPlayer * player)
{
  GStaticRecMutex *m_mutex = &(player->m_mutex);
	GstElement *m_pipeline = player->m_pipeline;	
	
	g_static_rec_mutex_lock (m_mutex);	 
	
	/* drop sink element from pipeline - see RMFMediaSourcePrivate::term() */
	if (dropsink (player) == FALSE) {
	  g_print ("failed to drop sink from pipeline\n");
		return FALSE;
	}

  /* destroy pipeline and source element/bin - see RMFMediaSourcePrivate::term() */
	g_source_remove (player->m_bus_event_source_id);
	player->m_bus_event_source_id = 0;
	
	gst_object_unref (GST_OBJECT(m_pipeline));
	
	player->m_pipeline = NULL;
	player->bus = NULL;
	
	g_static_rec_mutex_unlock (m_mutex);
	
	/* waiting for the pending buscall() to complete */
	g_static_rec_mutex_lock (m_mutex);
	g_static_rec_mutex_unlock (m_mutex);
	g_static_rec_mutex_free (m_mutex);	
	
	/* the source element has been unref when unref the pipeline */
	player->m_source = NULL;
	player->m_tee = NULL;

#if 0
  /* the sink has been removed from the pipeline and unref in dropsink() */
	/* remove sink element here - see RMFMediaSinkPrivate::~RMFMediaSinkPrivate() */
	if (remove_sink_element (player) == FALSE) {
	  g_print ("failed to remove sink element\n");
		return FALSE;
	}	
#endif	
	
  return TRUE;
}

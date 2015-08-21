/* main application code */
/* playback from given http live uri */
#include "gstrdk.h"

#define GST_RDK_VERSION "GST_RDK_0.0.1"

#ifdef FILE_SUPPORT
#define CHAN1_URL "dvbt_sub_ttx.ts"
#define CHAN2_URL "dvbt_sub_ttx.ts"
#else
#ifdef HTTPSRC_SUPPORT
#define CHAN1_URL "http://10.80.114.134:8080/vldms/tuner?ocap_locator=ocap://0x9D"
#define CHAN2_URL "http://10.80.114.134:8080/vldms/tuner?ocap_locator=ocap://0x9E"
#else
#define CHAN1_URL "http://10.80.114.232/dvbt_sub_ttx.ts"
#define CHAN2_URL "http://10.80.114.232/dvbt_sub_ttx.ts"
#endif
#endif

/* ST gstreamer player */
static STGstPlayer rdk_player;

static int
player_init (STGstPlayer * player)
{
  if (player == NULL) {
    return 1;
  }
	
  player->m_pipeline = NULL;
	player->m_source = NULL;
	player->m_tee = NULL;
	player->m_queue = NULL;
	player->m_sink = NULL;
	player->bus = NULL;
	player->m_bus_event_source_id = 0;
	player->demux = NULL;
	player->video_decoder = NULL;
	player->video_sink = NULL;
	player->audio_decoder = NULL;
	player->audio_sink = NULL;
	player->video_queue = NULL;
	player->audio_queue = NULL;
	
	player->audio_pad_linked = FALSE;
	player->video_pad_linked = FALSE;
	player->video_pad_linked = FALSE;

	memset (&(player->m_mutex), '\0', sizeof(GStaticRecMutex));
	memset (player->uri, '\0', 256);
	
	player->main_loop = NULL;	
	
	player->input_task_mutex = NULL;
	player->input_task = NULL;
	
	return 0;
}

static gboolean 
handle_channel_zapping (STGstPlayer * player, int loop)
{  
	int zapping_cnt = 0;
	gchar uri_to_play [255];
		
	while (zapping_cnt < loop) {
    /* set state to NULL */
	  if (set_pipeline_state (player, GST_STATE_NULL) == FALSE) {
		  g_print ("failed to set state to NULL\n");
			return FALSE;
		}
	
	  /* destroy pipeline */
	  if (destroy_pipeline (player) == FALSE) {
	    g_print ("failed to destroy pipeline\n");
	    return FALSE;
	  }
	
	  g_print ("=============> zapping count %d <==============\n", zapping_cnt);
		
		memset (uri_to_play, '\0', sizeof (uri_to_play));
		if (zapping_cnt % 2 == 0) {
		  strcpy (uri_to_play, CHAN2_URL);
		} else {		  
		  strcpy (uri_to_play, CHAN1_URL);
		}
		memcpy (player->uri, uri_to_play, strlen(uri_to_play));
		g_print ("Trying to play %s\n", player->uri);
	
	  /* create pipeline */
	  if (create_pipeline (player) == FALSE) {
	    g_print ("failed to create pipeline\n");
	    return FALSE;
	  }
	
	  /* set state to PLAYING */
	  if (set_pipeline_state (player, GST_STATE_PLAYING) == FALSE) {
		  g_print ("failed to set state to PLAYING\n");
			return FALSE;
		}
		
		//usleep(100 * 1000); // 100ms
		sleep (ZAPPING_INTERVAL_IN_SEC);
		zapping_cnt ++;
	}	
	
  return TRUE;
}

static void
process_input_key_from_keyboard (void *data)
{
  STGstPlayer *player = *((STGstPlayer **) data);
	GMainLoop *main_loop = player->main_loop;
	
	if (g_main_loop_is_running (main_loop)) {
	  int c = getchar();
		
		switch (c) {
		  case 'q':
			  g_print ("quit main loop\n");
				g_main_loop_quit (main_loop);
			  break;
			case 'z':
			  g_print ("zapping robust test\n"); // more loops
				if (handle_channel_zapping (player, 100) == FALSE) {
				  g_print("failed to zapping\n");
				}
				break;
			case 'o':
			  g_print ("zapping robust test\n"); // more loops
				if (handle_channel_zapping (player, 999999) == FALSE) {
				  g_print("failed to zapping\n");
				}
				break;
			case 'k':
			  g_print ("zapping test\n"); // 1 loop only
				if (handle_channel_zapping (player, 1) == FALSE) {
				  g_print("failed to zapping\n");
				}
				break;				
			default:
			  g_print ("unknown key no response\n");
			  break;
		}
	}
}

gint
main (gint argc, gchar * argv[])
{
  GError *err = NULL;
  gchar *uri_to_play = CHAN1_URL;

	STGstPlayer *player;
	
	g_print ("****************\n");
  g_print ("GST-RDK v%s\n", GST_RDK_VERSION);
  g_print ("****************\n");
  	
	g_print ("Trying to play %s\n", uri_to_play);
	
  /* Init GStreamer */
  if (!gst_init_check (&argc, &argv, &err)) {
    g_print ("failed not initialize GStreamer: %s\n",
        err ? err->message : "unknown error occurred");
    if (err) {
      g_error_free (err);
    }
  }

	player = &rdk_player;	
	if (player_init (player) != 0) {
	  return 1;
	}
	
	memcpy (player->uri, uri_to_play, strlen(uri_to_play));
	
	/* create main loop */
	player->main_loop = g_main_loop_new (NULL, FALSE);
	
	/* create keyboard task */
	player->input_task_mutex = g_new (GStaticRecMutex, 1);
	player->input_task = gst_task_create ((GstTaskFunction) process_input_key_from_keyboard, &player);
	gst_object_set_name (GST_OBJECT_CAST (player->input_task), "gst-rdk-keyb");
	if (player->input_task_mutex && player->input_task) {
	  g_static_rec_mutex_init (player->input_task_mutex);
		gst_task_set_lock (player->input_task, player->input_task_mutex);
		gst_task_start (player->input_task);
	}
	
	/* create pipeline and start playback */
	if (create_pipeline (player) == FALSE) {
	  g_print ("failed to create pipeline\n");
	  return 1;
	}
	
	/* set state to PLAYING */
	if (set_pipeline_state (player, GST_STATE_PLAYING) == FALSE) {
	  g_print ("failed to start playback\n");
		return 1;
	}
	
	/* run main loop */
	g_main_loop_run (player->main_loop);
	
	if (set_pipeline_state (player, GST_STATE_NULL) == FALSE) {
	  g_print ("failed to stop playback\n");
		return 1;
	}
	
	/* clean up the keyboard task */	
	gst_task_stop (player->input_task);
	g_static_rec_mutex_lock (player->input_task_mutex);
	g_static_rec_mutex_unlock (player->input_task_mutex);
	gst_task_join (player->input_task);
	gst_object_unref (player->input_task);
	g_static_rec_mutex_free (player->input_task_mutex);
	
	/* stop playback and destroy the pipeline */	
	if (destroy_pipeline (player) == FALSE) {
	  g_print ("failed to destroy pipeline\n");
	  return 1;
	}
	
	return 0;
}
	
	

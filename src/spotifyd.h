#pragma once

#include <libspotify/api.h>
#include <sys/queue.h>

#include "config.h"
#include "callbacks.h"
#include "audio.h"
#include "commandq.h"

#define API_MESSAGE_LEN 1024

audio_fifo_t g_audiofifo;

sp_track *search_result[NUM_SEARCH_RESULTS];
pthread_mutex_t search_result_lock;

pthread_mutex_t notify_mutex;
pthread_cond_t notify_cond;
char notify_do;

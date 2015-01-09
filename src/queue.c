#include <libspotify/api.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "config.h"
#include "queue.h"
#include "spotifyd.h"

sp_track *queue[PLAY_QUEUE_LEN];
unsigned queue_len;
bool queue_random;
unsigned queue_position;


void queue_init()
{
	pthread_mutex_init(&queue_lock, NULL);
	pthread_mutex_lock(&queue_lock);
	queue_len = 0;
	queue_position = 0;
	queue_random = 0;
	memset(queue, 0, PLAY_QUEUE_LEN * sizeof(sp_track *));
	pthread_mutex_unlock(&queue_lock);
}

bool queue_add_track(sp_track *track)
{
	bool track_existed;

	pthread_mutex_lock(&queue_lock);
	if(track != NULL)
	{
		sp_track_add_ref(track);
		queue[queue_len++] = track;
		track_existed = 1;
	}
	else
	{
		track_existed = 0;
	}
	pthread_mutex_unlock(&queue_lock);

	return track_existed;
}

int queue_get_next()
{
	srand(time(NULL));
	pthread_mutex_lock(&queue_lock);
	int next_track;
	if(queue_random)
	{
		next_track = rand()%queue_len;
	}
	else
	{
		next_track = (queue_position + 1)%queue_len;
	}
	pthread_mutex_unlock(&queue_lock);
	return next_track;
}

bool queue_toggle_random()
{
	pthread_mutex_lock(&queue_lock);
	queue_random = !queue_random;
	pthread_mutex_unlock(&queue_lock);
	return queue_random;
}

sp_track *queue_get(unsigned i)
{
	return queue[i];
}

void queue_set_current(unsigned i)
{
	pthread_mutex_lock(&queue_lock);
	queue_position = i;
	pthread_mutex_unlock(&queue_lock);
}

sp_track *queue_get_current()
{
	sp_track *ret_val = NULL;
	pthread_mutex_lock(&queue_lock);
	ret_val = queue[queue_position];
	pthread_mutex_unlock(&queue_lock);
	return ret_val;
}

unsigned queue_get_len()
{
	return queue_len;
}

void queue_del_track(unsigned trackn)
{
	pthread_mutex_lock(&queue_lock);
	if(trackn < queue_len && queue[trackn] != NULL)
	{
		sp_track_release(queue[trackn]);
		queue[trackn] = NULL;
		--queue_len;
		memmove(&queue[trackn], &queue[trackn+1], PLAY_QUEUE_LEN - trackn);
		memset(&queue[queue_len], 0, sizeof(sp_track *)*(PLAY_QUEUE_LEN - queue_len));
	}
	pthread_mutex_unlock(&queue_lock);
}

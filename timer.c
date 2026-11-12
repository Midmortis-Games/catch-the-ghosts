#pragma once

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

struct Timer {
	void (*callback)(void);
	float waitTime;
	float timeLeft;
	bool looping;
	bool finished;
};

struct Timer timerCreate(float timeout, void (*callback)(void), bool looping)
{
	assert(isfinite(timeout));
	assert(timeout > 0.0f);

	struct Timer timer = {
		.waitTime = timeout,
		.timeLeft = timeout,
		.callback = callback,
		.looping = looping,
	};

	return timer;
}

void timerReset(struct Timer *timer)
{
	timer->timeLeft = timer->waitTime;
}

void timerRestart(struct Timer *timer)
{
	timerReset(timer);
	timer->finished = false;
}

void timerProcess(struct Timer *timer, float delta)
{
	assert(timer != NULL);
	assert(isfinite(delta));
	assert(delta >= 0.0f);

	/* Reached timeout in a previous call, not looping */
	if (timer->finished) {
		return;
	}

	float timeSpent = delta;

	/* Hasn't reached timeout */
	if (timer->timeLeft > timeSpent) {
		timer->timeLeft -= timeSpent;
		return;
	}

	/* Reached timeout, not looping */
	if (!timer->looping) {
		timer->timeLeft = 0.0f;
		timer->finished = true;
		if (timer->callback != NULL) {
			timer->callback();
		}
		return;
	}

	/* Reached timeout, looping */
	float overflow = timeSpent - timer->timeLeft;

	timer->timeLeft = timer->waitTime - fmod(overflow, timer->waitTime);

	/* If reached timeout X times while looping, call callback X times */
	if (timer->callback != NULL) {
		float overflowLeft = overflow;
		do {
			timer->callback();
			overflowLeft -= timer->waitTime;
		} while (overflowLeft >= 0.0f);
	}
}

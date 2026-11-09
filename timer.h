#pragma once

#include <stdbool.h>

struct Timer {
	void (*callback)(void);
	float waitTime;
	float timeLeft;
	bool looping;
	bool finished;
};

struct Timer createTimer(float timeout, void (*callback)(void), bool looping);
void processTimer(struct Timer *timer, float delta);

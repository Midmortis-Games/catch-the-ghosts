#pragma once

#include <assert.h>

#include "common.h"

STATE currentState = STATE_START;

void stateEnterStartScreen(void);
void stateProcessStartScreen(void);
void stateExitStartScreen(void);

void stateEnterGameplay(void);
void stateProcessGameplay(void);
void stateExitGameplay(void);

void stateEnterDeath(void);
void stateProcessDeath(void);
void stateExitDeath(void);

void stateUpdate(void)
{
	switch (currentState) {
	case STATE_START:
		stateEnterGameplay();
		stateProcessGameplay();
		/* stateEnterStartScreen(); */
		/* stateProcessStartScreen(); */
		break;
	case STATE_START_SCREEN:
		stateProcessStartScreen();
		break;
	case STATE_GAMEPLAY:
		stateProcessGameplay();
		break;
	case STATE_DEATH:
		stateProcessDeath();
		break;
	default:
		assert(false);
	}
}

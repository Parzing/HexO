#include <stdio.h>
#include <unistd.h>
#include "action.h"

static int counter = 0;

Action get_user_action(void)
{
	Action actions[] = {
		ACT_UP_L, ACT_DOWN_R, ACT_LEFT, ACT_PLACE, ACT_DEFAULT
	};
	Action mock = actions[counter];
	counter++;
	return mock;
}

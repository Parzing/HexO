#include <stdio.h>
#include <unistd.h>
#include "../src/action.h"

static Action key_to_action(char key)
{
	switch (key) {
		case KEY_UPL:   return ACT_UP_L;
		case KEY_UPR:   return ACT_UP_R;
		case KEY_DOWNL:	return ACT_DOWN_L;
		case KEY_DOWNR:	return ACT_DOWN_R;
		case KEY_LEFT: 	return ACT_LEFT;
		case KEY_RIGHT:	return ACT_RIGHT;
		case KEY_SPACE:	return ACT_PLACE;
		case KEY_ENTER:	return ACT_PLACE;
		default:        return ACT_DEFAULT;
	}
}

Action get_user_action(void)
{
	char buf[64];
	int n = read(STDIN_FILENO, buf, sizeof(buf));
	for (int i = 0; i < n; i++){
		Action action = key_to_action(buf[i]);
		if (action != ACT_DEFAULT) {
			return action;
		}
	}
	return ACT_DEFAULT;
}

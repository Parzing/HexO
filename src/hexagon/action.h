#ifndef ACTION_H
#define ACTION_H

#define KEY_UPL ('q')
#define KEY_UPR ('e')
#define KEY_DOWNL ('z')
#define KEY_DOWNR ('c')
#define KEY_LEFT ('a')
#define KEY_RIGHT ('d')
#define KEY_SPACE (' ')
#define KEY_ENTER ('\n')

typedef enum {
	ACT_DEFAULT,
	ACT_UP_L,
	ACT_UP_R,
	ACT_DOWN_L,
	ACT_DOWN_R,
	ACT_LEFT,
	ACT_RIGHT,
	ACT_PLACE,
	ACT_REMOVE
} Action;

/*
 * Read one action from the keyboard, discarding all other inputs
 */
Action get_user_action(void);

#endif

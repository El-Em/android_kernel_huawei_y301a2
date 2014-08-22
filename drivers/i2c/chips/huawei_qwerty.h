/* drivers/i2c/chips/huawei_qwerty.h
 *
 * Copyright (C) 2007-2009 Huawei.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
 
#ifndef __HUAWEI_QWERTY_H
#define __HUAWEI_QWERTY_H


#define ADP5587_KEYPAD_ROW_SIZE	5
#define	ADP5587_KEYPAD_COL_SIZE	10

#define KEYMAP_INDEX(row, col) ((row)*ADP5587_KEYPAD_COL_SIZE + (col))

#define KEYMAP_SIZE ADP5587_KEYPAD_ROW_SIZE * ADP5587_KEYPAD_COL_SIZE


static unsigned short keypad_keymap_8930[KEYMAP_SIZE] = {
	[KEYMAP_INDEX(0, 0)] = KEY_W,
	[KEYMAP_INDEX(0, 1)] = KEY_S,
	[KEYMAP_INDEX(0, 2)] = KEY_Z,
	[KEYMAP_INDEX(0, 3)] = KEY_EMAIL,	//@
	[KEYMAP_INDEX(0, 4)] = KEY_LEFTSHIFT,
	[KEYMAP_INDEX(0, 5)] = KEY_LEFTALT,
	[KEYMAP_INDEX(0, 6)] = KEY_A,
	[KEYMAP_INDEX(0, 7)] = KEY_Q,

	[KEYMAP_INDEX(1, 0)] = KEY_R,
	[KEYMAP_INDEX(1, 1)] = KEY_F,
	[KEYMAP_INDEX(1, 2)] = KEY_C,
	[KEYMAP_INDEX(1, 3)] = KEY_COMMA,	//,
	[KEYMAP_INDEX(1, 4)] = KEY_QUESTION,	//?
	[KEYMAP_INDEX(1, 5)] = KEY_X,
	[KEYMAP_INDEX(1, 6)] = KEY_D,
	[KEYMAP_INDEX(1, 7)] = KEY_E,

	[KEYMAP_INDEX(2, 0)] = KEY_Y,
	[KEYMAP_INDEX(2, 1)] = KEY_H,
	[KEYMAP_INDEX(2, 2)] = KEY_B,
	[KEYMAP_INDEX(2, 3)] = KEY_SPACE,
	[KEYMAP_INDEX(2, 4)] = KEY_SPACE,
	[KEYMAP_INDEX(2, 5)] = KEY_V,
	[KEYMAP_INDEX(2, 6)] = KEY_G,
	[KEYMAP_INDEX(2, 7)] = KEY_T,

	[KEYMAP_INDEX(3, 0)] = KEY_I,
	[KEYMAP_INDEX(3, 1)] = KEY_K,
	[KEYMAP_INDEX(3, 2)] = KEY_M,
	[KEYMAP_INDEX(3, 3)] = KEY_UP,
	[KEYMAP_INDEX(3, 4)] = KEY_DOT,	//.
	[KEYMAP_INDEX(3, 5)] = KEY_N,
	[KEYMAP_INDEX(3, 6)] = KEY_J,
	[KEYMAP_INDEX(3, 7)] = KEY_U,
	
	[KEYMAP_INDEX(4, 0)] = KEY_P,
	[KEYMAP_INDEX(4, 1)] = KEY_BACKSPACE,
	[KEYMAP_INDEX(4, 2)] = KEY_ENTER,
	[KEYMAP_INDEX(4, 3)] = KEY_DOWN,
	[KEYMAP_INDEX(4, 4)] = KEY_LEFT,
	[KEYMAP_INDEX(4, 5)] = KEY_RIGHT,
	[KEYMAP_INDEX(4, 6)] = KEY_L,
	[KEYMAP_INDEX(4, 7)] = KEY_O,
};
#endif

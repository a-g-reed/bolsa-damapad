/*
Copyright 2021 Matthew Dias

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "damapad.h" 

// OLED STUFF STARTS HERE
// based on https://github.com/qmk/qmk_firmware/blob/master/keyboards/kyria/keymaps/j-inc/keymap.c

// In your rules.mk make sure you have:
// OLED_DRIVER_ENABLE = yes


enum layer_names {
    _FKEY,
    _NUMS,
    _MEDIA,
    _MISC,

};

/* bool is_alt_tab_active = false; // ADD this near the begining of keymap.c
bool is_alt_shift_tab_active = false; // ADD this near the begining of keymap.c
uint16_t alt_tab_timer = 0;     // we will be using them soon.

//ALT TAB Encoder Timer
void matrix_scan_user(void) { // The very important timer.
  if (is_alt_tab_active) {
    if (timer_elapsed(alt_tab_timer) > 1000) {
      unregister_code(KC_LALT);
      unregister_code(KC_LSHIFT);
      is_alt_tab_active = false;
      is_alt_shift_tab_active = false;
    }
  }
}; */

bool oled_tracker = false;
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	if (record->event.pressed) {
		oled_tracker ^=1;
	}
return true;
}

#ifdef OLED_ENABLE
// WPM-responsive animation stuff here
#    define IDLE_FRAMES 5
#    define IDLE_SPEED 20  // below this wpm value your animation will idle

// #define PREP_FRAMES 1 // uncomment if >1

#    define TAP_FRAMES 2
#    define TAP_SPEED 40  // above this wpm value typing animation to trigger

#    define ANIM_FRAME_DURATION 200  // how long each frame lasts in ms
// #define SLEEP_TIMER 60000 // should sleep after this period of 0 wpm, needs fixing
#    define ANIM_SIZE 636  // number of bytes in array, minimize for adequate firmware size, max is 1024

uint32_t anim_timer         = 0;
uint32_t anim_sleep         = 0;
uint8_t  current_idle_frame = 0;
// uint8_t current_prep_frame = 0; // uncomment if PREP_FRAMES >1
uint8_t current_tap_frame = 0;

// Code containing pixel art, contains:
// 5 idle frames, 1 prep frame, and 2 tap frames

// To make your own pixel art:
// save a png/jpeg of an 128x32 image (resource: https://www.pixilart.com/draw )
// follow this guide up to and including "CONVERT YOUR IMAGE" https://docs.splitkb.com/hc/en-us/articles/360013811280-How-do-I-convert-an-image-for-use-on-an-OLED-display-
// replace numbers in brackets with your own
// if you start getting errors when compiling make sure you didn't accedentally delete a bracket
static void render_anim(void) {
    static const char PROGMEM idle[IDLE_FRAMES][ANIM_SIZE] = {
				 {
				 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16, 16,  8,  8,  4,  4,  4,  8, 48, 64,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 24,100,130,  2,  2,  2,  2,  2,  1,  0,  0,  0,  0,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,  0, 48, 48,  0,192,193,193,194,  4,  8, 16, 32, 64,128,  0,  0,  0,128,128,128,128, 64, 64, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  8,  8,  8,  4,  4,  4,  4,  4,  2,  2,  2,  2,  1,  1,  1,  1,  0,  0,  0, 
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,192, 56,  4,  3,  0,  0,  0,  0,  0,  0,  0, 12, 12, 12, 13,  1,  0, 64,160, 33, 34, 18, 17, 17, 17,  9,  8,  8,  8,  8,  4,  4,  8,  8, 16, 16, 16, 16, 16, 17, 15,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,128,128, 64, 64, 64, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  8,  8,  4,  4,  4,  4,  2,  3,  2,  2,  1,  1,  1,  1,  1,  1,  2,  2,  4,  4,  8,  8,  8,  8,  8,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
				 },
				 {
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16, 16,  8,  8,  4,  4,  4,  8, 48, 64,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 24,100,130,  2,  2,  2,  2,  2,  1,  0,  0,  0,  0,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,  0, 48, 48,  0,192,193,193,194,  4,  8, 16, 32, 64,128,  0,  0,  0,128,128,128,128, 64, 64, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  8,  8,  8,  4,  4,  4,  4,  4,  2,  2,  2,  2,  1,  1,  1,  1,  0,  0,  0, 
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,192, 56,  4,  3,  0,  0,  0,  0,  0,  0,  0, 12, 12, 12, 13,  1,  0, 64,160, 33, 34, 18, 17, 17, 17,  9,  8,  8,  8,  8,  4,  4,  8,  8, 16, 16, 16, 16, 16, 17, 15,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,128,128, 64, 64, 64, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  8,  8,  4,  4,  4,  4,  2,  3,  2,  2,  1,  1,  1,  1,  1,  1,  2,  2,  4,  4,  8,  8,  8,  8,  8,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
				 },
				 {
				 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128, 64, 64, 64, 64, 32, 32, 32, 32, 16,  8,  4,  2,  2,  4, 24, 96,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 60,194,  1,  1,  2,  2,  4,  4,  2,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 96, 96,  0,129,130,130,132,  8, 16, 32, 64,128,  0,  0,  0,  0,128,128,128,128, 64, 64, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  8,  8,  8,  4,  4,  4,  4,  4,  2,  2,  2,  2,  1,  1,  1,  1,  0,  0,  0, 
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,112, 25,  6,  0,  0,  0,  0,  0,  0,  0, 24, 24, 24, 27,  3,  0, 64,160, 34, 36, 20, 18, 18, 18, 11,  8,  8,  8,  8,  5,  5,  9,  9, 16, 16, 16, 16, 16, 17, 15,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,128,128, 64, 64, 64, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  8,  8,  4,  4,  4,  4,  2,  3,  2,  2,  1,  1,  1,  1,  1,  1,  2,  2,  4,  4,  8,  8,  8,  8,  8,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
				 },
				 {
				 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,  0,  0,  0,  0,  0,128, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  4,  2,  1,  1,  2, 12, 48, 64,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 30,225,  0,  0,  1,  1,  2,  2,  1,  0,  0,  0,  0,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,  0, 48, 48,  0,192,193,193,194,  4,  8, 16, 32, 64,128,  0,  0,  0,128,128,128,128, 64, 64, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  8,  8,  8,  4,  4,  4,  4,  4,  2,  2,  2,  2,  1,  1,  1,  1,  0,  0,  0, 
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,112, 12,  3,  0,  0,  0,  0,  0,  0,  0, 12, 12, 12, 13,  1,  0, 64,160, 33, 34, 18, 17, 17, 17,  9,  8,  8,  8,  8,  4,  4,  8,  8, 16, 16, 16, 16, 16, 17, 15,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,128,128, 64, 64, 64, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  8,  8,  4,  4,  4,  4,  2,  3,  2,  2,  1,  1,  1,  1,  1,  1,  2,  2,  4,  4,  8,  8,  8,  8,  8,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
				 },
				 {
				 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  4,  2,  2,  2,  4, 56, 64,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 28,226,  1,  1,  2,  2,  2,  2,  1,  0,  0,  0,  0,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,  0, 48, 48,  0,192,193,193,194,  4,  8, 16, 32, 64,128,  0,  0,  0,128,128,128,128, 64, 64, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  8,  8,  8,  4,  4,  4,  4,  4,  2,  2,  2,  2,  1,  1,  1,  1,  0,  0,  0, 
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,112, 12,  3,  0,  0,  0,  0,  0,  0,  0, 12, 12, 12, 13,  1,  0, 64,160, 33, 34, 18, 17, 17, 17,  9,  8,  8,  8,  8,  4,  4,  8,  8, 16, 16, 16, 16, 16, 17, 15,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
				0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,128,128, 64, 64, 64, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  8,  8,  4,  4,  4,  4,  2,  3,  2,  2,  1,  1,  1,  1,  1,  1,  2,  2,  4,  4,  8,  8,  8,  8,  8,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
				 }
		};
        static const char PROGMEM tap[TAP_FRAMES][ANIM_SIZE]   = {
        {
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,  0,  0,  0,  0,  0,128, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  4,  2,  1,  1,  2, 12, 48, 64,128,  0,  0,  0,  0,  0,  0,  0,248,248,248,248,  0,  0,  0,  0,  0,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 30,225,  0,  0,  1,  1,  2,  2,129,128,128,  0,  0,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,  0, 48, 48,  0,  0,  1,  1,  2,  4,  8, 16, 32, 67,135,  7,  1,  0,184,188,190,159, 95, 95, 79, 76, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  8,  8,  8,  4,  4,  4,  4,  4,  2,  2,  2,  2,  1,  1,  1,  1,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,112, 12,  3,  0,  0, 24,  6,  5,152,153,132, 67,124, 65, 65, 64, 64, 32, 33, 34, 18, 17, 17, 17,  9,  8,  8,  8,  8,  4,  4,  8,  8, 16, 16, 16, 16, 16, 17, 15,  1, 61,124,252,252,252,252,252, 60, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,128,128, 64, 64, 64, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  8,  8,  4,  4,  4,  4,  2,  3,  2,  2,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  3,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0
         },
        {
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,  0,  0,  0,  0,  0,128, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  4,  2,  1,  1,  2, 12, 48, 64,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 30,225,  0,  0,  1,  1,  2,  2,  1,  0,  0,  0,  0,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,  0, 48, 48,  0,  0,  1,225, 26,  6,  9, 49, 53,  1,138,124,  0,  0,128,128,128,128, 64, 64, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  8,  8,  8,  4,  4,  4,  4,  4,  2,  2,  2,  2,  1,  1,  1,  1,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,112, 12,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  0, 64,160, 33, 34, 18, 17, 17, 17,  9,  8,  8,  8,  8,  4,  4,  4,  4,  4,  4,  2,  2,  2,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,128,128, 64, 64, 64, 64, 64, 32, 32, 32, 32, 16, 16, 16, 16,  8,  8,  8,  8,  4,  4,  4,  4,  2,  3,122,122,121,121,121,121, 57, 49,  2,  2,  4,  4,  8,  8,  8,  8,  8,199,224,224,192,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
         }
    };

    // assumes 1 frame prep stage
    void animation_phase(void) {
        if (!oled_tracker) {
            current_idle_frame = (current_idle_frame + 1) % IDLE_FRAMES;
            oled_write_raw_P(idle[abs((IDLE_FRAMES - 1) - current_idle_frame)], ANIM_SIZE);
        }
        if (oled_tracker) {
            current_tap_frame = (current_tap_frame + 1) % TAP_FRAMES;
            oled_write_raw_P(tap[abs((TAP_FRAMES - 1) - current_tap_frame)], ANIM_SIZE);
			oled_tracker = false;
        }
    }
    if (oled_tracker) {
        oled_on();  // not essential but turns on animation OLED with any alpha keypress
        if (timer_elapsed32(anim_timer) > ANIM_FRAME_DURATION) {
            anim_timer = timer_read32();
            animation_phase();
        }
        anim_sleep = timer_read32();
    } else {
        if (timer_elapsed32(anim_sleep) > OLED_TIMEOUT) {
            oled_off();
        } else {
            if (timer_elapsed32(anim_timer) > ANIM_FRAME_DURATION) {
                anim_timer = timer_read32();
                animation_phase();
            }
        }
    }
}

// Used to draw on to the oled screen
bool oled_task_kb(void) {
    if (!oled_task_user()) {
    	return false; 
    }
	render_anim();  // renders pixelart
    oled_set_cursor(0, 0);                            
	switch (get_highest_layer(layer_state|default_layer_state)) {
        case _FKEY:
            oled_write_P(PSTR("1"), false);
            break;
        case _NUMS:
            oled_write_P(PSTR("2"), false);
            break;
        case _MEDIA:
            oled_write_P(PSTR("3"), false);
            break;
        case _MISC:
            oled_write_P(PSTR("4"), false);
            break;
	}
	return false;
}
oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    return OLED_ROTATION_180;
	}
#endif


/* #ifdef ENCODER_ENABLE
bool encoder_update_kb(uint8_t index, bool clockwise) {
    if (!encoder_update_user(index, clockwise)) { return false; }
    if (index == 0) {
        switch (get_highest_layer(layer_state|default_layer_state)) {
        case _FKEY:
            if (clockwise) {
            tap_code_delay(KC_VOLU, 10);
			} else {
            tap_code_delay(KC_VOLD, 10);
			}
			oled_tracker ^=1;
			break;
        case _NUMS:
            if (clockwise) {
            tap_code_delay(KC_PGDN, 10);
			} else {
            tap_code_delay(KC_PGUP, 10);
			}
			oled_tracker ^=1;
			break;
        case _MEDIA:
            if (clockwise) {
            tap_code_delay(KC_VOLU, 10);
			} else {
            tap_code_delay(KC_VOLD, 10);
			}
			oled_tracker ^=1;
			break;			
        case _MISC:
            if (clockwise) {
                if (!is_alt_tab_active) {
                    is_alt_tab_active = true;
                unregister_code(KC_LSHIFT);
                register_code(KC_LALT);
                }
                alt_tab_timer = timer_read();
                tap_code(KC_TAB);
            } else {
                if (!is_alt_shift_tab_active) {
                    is_alt_shift_tab_active = true;
                register_code(KC_LALT);
                register_code(KC_LSHIFT);
                }
                alt_tab_timer = timer_read();
                tap_code(KC_TAB);
			break;
			}
	}
    }
    return true;
}
#endif */

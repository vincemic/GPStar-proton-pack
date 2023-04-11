/**
 *   Proton Pack - Arduino Powered Ghostbusters Proton Pack & Neutrona Wand.
 *   Copyright (C) 2023 Michael Rajotte <michael.rajotte@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, see <https://www.gnu.org/licenses/>.
 *
 */

/* 
 *  You need to edit wavTrigger.h and make sure you comment out the proper serial port. (Near the top of the wavTrigger.h file).
 *  We are going to use tx/rx #3 on the Mega.  __WT_USE_SERIAL3___
 */
#include <wavTrigger.h>
#include <millisDelay.h> 
#include <FastLED.h>
#include <ezButton.h>
#include <Ramp.h>

/*
 * -------------****** CUSTOM USER CONFIGURABLE SETTINGS ******-------------
 * Change the variables below to alter the behaviour of your Proton Pack.
 * All the default settings below are setup for a stock Haslab Proton Pack lighting kit.
 */
 
 /* 
 *  PowerCell and Cyclotron Lid LEDs.
 *  25 LEDs in the stock Haslab kit. 13 in the Powercell and 12 in the Cyclotron lid. 
 *  
 *  For the stock Haslab LEDs, use const int i_pack_num_leds = 25; and const i_1984_cyclotron_leds 2, 5, 8, 11.
 *  For a 40 LED NeoPixel ring, use const int i_pack_num_leds = 53 and const i_1984_cyclotron_leds 1, 11, 19, 29.
 *  You can use any LED setup with up to 40 LEDs. If you change them out to individual NeoPixels or NeoPixel Rings, adjust your settings accordingly.  
 *
 *  i_1984_cyclotron_leds is the middle led aligned in each lens window. Adjust this setting if you use different LED setups and installations.
 *  Put the sequence in order from lowest to highest in a clockwise direction. (Top right lens as cyclotron lens #1 and work your way clockwise)
 *
 */ 
 // For stock Haslab LEDs
const int i_pack_num_leds = 25;
const int i_1984_cyclotron_leds[4] = { 2, 5, 8, 11 };

// For a 40 LED NeoPixel ring.
//const int i_pack_num_leds = 53;
//const int i_1984_cyclotron_leds[4] = { 1, 11, 19, 29 };

/*
 * Cyclotron Lid LED delays.
 * Time in milliseconds between when a LED changes.
 * 1000 = 1 second.
 * For a 40 LED NeoPixel ring, 10 for i_2021_delay is good.
 * For stock Haslab LEDs, 15 for i_2021_delay is good.
 * i_1984_delay does not need to be changed at all, unless you want to make the delay shorter or quicker.
 */
const int i_1984_delay = 1050;
const int i_2021_delay = 15; // 15 for stock Haslab LEDs. Change to 10 for a 40 LED NeoPixel ring.

/*
 * (OPTIONAL) Inner Cyclotron (cake) NeoPixel ring
 * If you are not using any, then this can be left alone.
 * Leave at least one in place, even if you are not using this optional item.
 * 24 -> For a 24 LED NeoPixel Ring
 * 35 -> For a 35 LED NeoPixel Ring. (Recommended ring size)
 */
//#define CYCLOTRON_NUM_LEDS 24
#define CYCLOTRON_NUM_LEDS 35

/*
 * You can set the default master startup volume for your pack here.
 * When a Neutrona wand is connected, it will sync to these settings.
 * Values are in % of the volume.
 * 0 = quietest
 * 100 = loudest
 */
const int STARTUP_VOLUME = 100;

/*
 * You can set the default music volume for your pack here.
 * When a Neutrona wand is connected, it will sync to these settings.
 * Values are in % of the volume.
 * 0 = quietest
 * 100 = loudest
 */
const int STARTUP_VOLUME_MUSIC = 100;

/*
 * You can set the default sound effects volume for your pack here.
 * When a Neutrona wand is connected, it will sync to these settings.
 * Values are in % of the volume.
 * 0 = quietest
 * 100 = loudest
 */
const int STARTUP_VOLUME_EFFECTS = 100;

/*
 * Minimum volume that the pack can achieve. 
 * Values must be from 0 to -70. 0 = the loudest and -70 = the quietest.
 * Volume changes are based on percentages. 
 * If your pack is overpowering the wand at lower volumes, you can either increase the minimum value in the wand,
 * or decrease the minimum value for the pack.
*/
const int MINIMUM_VOLUME = -50;

/*
 * Percentage increments of main volume change.
*/
const int VOLUME_MULTIPLIER = 2;

/*
 * Percentage increments of the music volume change..
*/
const int VOLUME_MUSIC_MULTIPLIER = 5;

/*
 * Percentage increments of the sound effects volume change.
*/
const int VOLUME_EFFECTS_MULTIPLIER = 5;

/*
 * Inner cyclotron NeoPixel ring speed.
 * The lower the number, the faster it will spin.
 * Default settings for a 35 NeoPixel ring is: 5 for 2021 mode and 9 for 1984 mode.
 * If you are using a ring with less than 35 NeoPixels, you may need to slightly raise these numbers.
 */
const int i_2021_inner_delay = 5;
const int i_1984_inner_delay = 9;

/* 
 *  Default the cyclotron lights direction to clockwise. 
 *  This can be controlled by an optional switch on pin 29. 
 *  Set to false to be counter clockwise.
 */
boolean b_clockwise = true;

/*
 * When set to true, 1984 mode is turned into 1989 mode. 
 * The pack will play 1989 sound effects instead of 1984 sound effects.
*/
const boolean b_gb2_mode = false;

/*
 * When set to false, 1984 mode LED's will fade in or out.
*/
const boolean b_fade_cyclotron_led = true;

/*
 * When fading is enabled for 1984 mode cyclotron lid lights, control the delay of the fading.
*/
const int i_1984_fade_out_delay = 210;
const int i_1984_fade_in_delay = 210;

/*
 * Set to true to enable the onboard amplifer on the wav trigger. 
 * Turning off the onboard amp draws less power. 
 * If using the AUX cable jack, the amp can be disabled to save power.
 * If you use the output pins directly on the wav trigger board to your speakers, you will need to enable the onboard amp.
 * NOTE: The On-board mono audio amplifier and speaker connector specifications: 2W into 4 Ohms, 1.25W into 8 Ohms
 */
const boolean b_onboard_amp_enabled = false;

/*
 * If you want the optional n-filter NeoPixel jewel to strobe during overheat, set to true.
 * If false, the light stay solid white during overheat.
 */
const boolean b_overheat_strobe = false;

/*
 * Enable or disable overall smoke settings.
 * This can be toggled with a switch on PIN 37.
 */
boolean b_smoke_enabled = true;

/*
 * ****************** ADVANCED USER CONFIGURABLE SMOKE SETTINGS BELOW ************************
 * The default settings work very well. Changing them can produce strange timing effect.
 */
 
/*
 * Enable or disable smoke during continuous firing.
 * Control which of the 3 pins that go high during continuous firing smoke effects.
 * This can be overriden if b_smoke_enabled is set to false.
 */
const boolean b_smoke_1_continuous_firing = true;
const boolean b_smoke_2_continuous_firing = true;
const boolean b_fan_continuous_firing = true;

/*
 * Enable or disable smoke in individual wand power modes for continuous firing smoke.
 * Example: if b_smoke_continuous_mode_1 is true, smoke will happen in continuous firing in wand power mode 1. If false, no smoke in mode 1.
 * This is overridden if b_smoke_enabled or can be by the continuous_firing settings above when they are set to false.
 */
const boolean b_smoke_continuous_mode_1 = true;
const boolean b_smoke_continuous_mode_2 = true;
const boolean b_smoke_continuous_mode_3 = true;
const boolean b_smoke_continuous_mode_4 = true;
const boolean b_smoke_continuous_mode_5 = true;

/*
 * How long (in milliseconds) until the smoke pins (+ fan) are activated during continuous firing in each firing power mode. (not overheating venting)
 * Example: 30,000 milliseconds (30 seconds)
 */
const unsigned long int i_smoke_timer_mode_1 = 30000;
const unsigned long int i_smoke_timer_mode_2 = 15000;
const unsigned long int i_smoke_timer_mode_3 = 10000;
const unsigned long int i_smoke_timer_mode_4 = 7500;
const unsigned long int i_smoke_timer_mode_5 = 6000;

/*
 *  How long do you want your smoke pins (+ fan) to stay on while firing for each firing power mode. (not overheating venting)
 *  When the pins are high (controlled by the i_smoke_timer above), then smoke will be generated if you have smoke machines etc wired up.
 *  Default is 3000 milliseconds (3 seconds). 
 *  This does not affect smoke during overheat. 
 *  This only affects how long your smoke stays on after it has been triggered in continuous firing.
 */
const unsigned long int i_smoke_on_time_mode_1 = 3000;
const unsigned long int i_smoke_on_time_mode_2 = 3000;
const unsigned long int i_smoke_on_time_mode_3 = 3500;
const unsigned long int i_smoke_on_time_mode_4 = 3500;
const unsigned long int i_smoke_on_time_mode_5 = 4000;

/*
 * Enable or disable smoke during overheat sequences.
 * Control which of the 3 pins that go high during overheat.
 * This can be overridden if b_smoke_enabled is set to false.
 */
const boolean b_smoke_1_overheat = true;
const boolean b_smoke_2_overheat = true;
const boolean b_fan_overheat = true;

/*
 * Enable or disable overheat smoke in different wand power modes.
 * Example: If b_smoke_overheat_mode_1 is false, then no smoke will be generated during overheat in wand power mode 1, if overheat is enabled for that power mode in the wand code.
 * This is overridden if b_smoke_enabled or can be by the b_overheat settings above when they are set to false.
 */
const boolean b_smoke_overheat_mode_1 = true;
const boolean b_smoke_overheat_mode_2 = true;
const boolean b_smoke_overheat_mode_3 = true;
const boolean b_smoke_overheat_mode_4 = true;
const boolean b_smoke_overheat_mode_5 = true;

/*
 * Set this to true if you want to know if your wand and pack are communicating.
 * If the wand and pack have a serial connection, you will hear a beeping sound.
 * Set to false to turn off the sound.
 */
const boolean b_diagnostic = false;

/*
 * -------------****** DO NOT CHANGE ANYTHING BELOW THIS LINE ******-------------
 */

/* 
 *  SD Card sound files in order. If you have no sound, your SD card might be too slow, try another one.
 *  File naming 000_ is important as well. For music, it is 100_ and higher.
 *  Also note if you add more sounds to this list, you need to update the i_last_effects_track variable.
 *  The wav trigger uses this to determine how many music tracks there are if any.
 */
enum sound_fx {
  S_EMPTY, 
  S_BOOTUP,
  S_SHUTDOWN,
  S_IDLE_LOOP,
  S_IDLE_LOOP_GUN,
  S_FIRE_START,
  S_FIRE_START_SPARK,
  S_FIRE_LOOP,
  S_FIRE_LOOP_GUN,
  S_FIRE_LOOP_IMPACT,
  S_FIRING_END,
  S_FIRING_END_GUN,
  S_AFTERLIFE_BEEP_WAND,
  S_AFTERLIFE_GUN_RAMP_LOW,
  S_AFTERLIFE_GUN_RAMP_HIGH,
  S_AFTERLIFE_PACK_STARTUP,
  S_AFTERLIFE_PACK_IDLE_LOOP,
  S_WAND_SHUTDOWN,
  S_AFTERLIFE_BEEP_WAND_S1,
  S_AFTERLIFE_BEEP_WAND_S2,
  S_AFTERLIFE_BEEP_WAND_S3,
  S_AFTERLIFE_BEEP_WAND_S4,
  S_AFTERLIFE_BEEP_WAND_S5,
  S_AFTERLIFE_GUN_RAMP_1,
  S_AFTERLIFE_GUN_LOOP_1,
  S_AFTERLIFE_GUN_RAMP_2,
  S_AFTERLIFE_GUN_LOOP_2,
  S_AFTERLIFE_GUN_RAMP_DOWN_2,
  S_AFTERLIFE_GUN_RAMP_DOWN_1,
  S_IDLE_LOOP_GUN_2,
  S_IDLE_LOOP_GUN_3,
  S_IDLE_LOOP_GUN_4,
  S_IDLE_LOOP_GUN_5,
  S_IDLE_LOOP_GUN_1,
  S_PACK_BEEPING,
  S_PACK_SHUTDOWN,
  S_PACK_SHUTDOWN_AFTERLIFE,
  S_GB2_PACK_START,
  S_GB2_PACK_LOOP,
  S_GB2_PACK_OFF,
  S_CLICK,
  S_VENT,
  S_VENT_SLOW,
  S_VENT_FAST,
  S_VENT_DRY,
  S_VENT_BEEP,
  S_VENT_BEEP_3,
  S_VENT_BEEP_7,
  S_PACK_SLIME_OPEN,
  S_PACK_SLIME_CLOSE,
  S_PACK_SLIME_TANK_LOOP,
  S_SLIME_START,
  S_SLIME_LOOP,
  S_SLIME_END,
  S_STASIS_START,
  S_STASIS_LOOP,
  S_STASIS_END,
  S_MESON_START,
  S_MESON_LOOP,
  S_MESON_END,
  S_BEEP_8,
  S_VENT_SMOKE,
  S_MODE_SWITCH,
  S_BEEPS,
  S_BEEPS_ALT,
  S_SPARKS_LONG,
  S_SPARKS_LOOP,
  S_BEEPS_LOW,
  S_BEEPS_BARGRAPH,
  S_MESON_OPEN,
  S_STASIS_OPEN,
  S_FIRING_END_MID,
  S_FIRING_LOOP_GB1,
  S_CROSS_STREAMS_END,
  S_CROSS_STREAMS_START,
  S_PACK_RIBBON_ALARM_1,
  S_PACK_RIBBON_ALARM_2
};

/*
 * Need to keep track which is the last sound effect, so we can iterate over the effects to adjust volume gain on them.
 */
const int i_last_effects_track = S_PACK_RIBBON_ALARM_2;

 /* 
 *  PowerCell and Cyclotron Lid LEDs + optional n_filter NeoPixel.
 *  7 additional (32 in total in stock form) for a NeoPixel jewel that you can put into the n-filter (optional). 
 *  This jewel chains off cyclotron lens #4 in the lid (top left lens).
 */
#define PACK_NUM_LEDS i_pack_num_leds + 7
#define VENT_LIGHT_START i_pack_num_leds

/*
 * Proton pack powercell and cyclotron lid led pin.
*/
#define PACK_LED_PIN 53
CRGB pack_leds[PACK_NUM_LEDS];

/*
 * Inner Cyclotron LEDs. (optional).
 * Uses pin 13.
 */
#define CYCLOTRON_LED_PIN 13
CRGB cyclotron_leds[CYCLOTRON_NUM_LEDS];

/*
 * Delay for fastled to update the addressable LEDs. 
 * We have up to 88 addressable LEDs if using NeoPixel jewels in the inner cyclotron and n-filter.
 * 0.03 ms to update 1 LED. So 3 ms should be ok. Lets bump it up to 6 just in case.
 */
const int i_fast_led_delay = 6;
millisDelay ms_fast_led;

/*
 * Powercell LEDs control.
 */
const int i_powercell_delay_1984 = 75;
const int i_powercell_delay_2021 = 40;
int i_powercell_delay = i_powercell_delay_2021;
int i_powercell_led = 0;
millisDelay ms_powercell;

/* 
 *  Cyclotron Inner Switch Panel LEDs control. (optional)
 */
const int i_cyclotron_switch_led_delay_base = 150;
int i_cyclotron_switch_led_delay = i_cyclotron_switch_led_delay_base;
int i_cyclotron_sw_led = 0;
int i_cyclotron_switch_led_mulitplier = 1;
millisDelay ms_cyclotron_switch_led;

/* 
 *  State of the pack.
 */
enum PACK_STATE { MODE_OFF, MODE_ON };
enum PACK_STATE PACK_STATUS;

/*
 * Pack action state.
 */
enum PACK_ACTION_STATE { ACTION_IDLE, ACTION_OFF, ACTION_ACTIVATE };
enum PACK_ACTION_STATE PACK_ACTION_STATUS;

/*
 * Cyclotron lid LEDs control and lid detection.
 */
const int cyclotron_led_start = 13; // First LED in the cyclotron.
int i_led_cyclotron = cyclotron_led_start; // Current cyclotron LED that we are lighting up.
const int i_2021_ramp_delay = 300;
const int i_2021_ramp_length = 6000;
const int i_1984_ramp_length = 3000;
const int i_2021_ramp_down_length = 5000;
const int i_1984_ramp_down_length = 2500;
int i_current_ramp_speed = i_2021_ramp_delay;
int i_cyclotron_multiplier = 1;
bool b_2021_ramp_up = true;
bool b_2021_ramp_up_start = true;
bool b_2021_ramp_down_start = false;
bool b_2021_ramp_down = false;
bool b_reset_start_led = true;
bool b_1984_led_start = true;
rampInt r_2021_ramp;
millisDelay ms_cyclotron;
boolean b_cyclotron_lid_on = true;
rampInt ms_cyclotron_fade_out_led_1;
rampInt ms_cyclotron_fade_out_led_2;
rampInt ms_cyclotron_fade_out_led_3;
rampInt ms_cyclotron_fade_out_led_4;
rampInt ms_cyclotron_fade_out_led_5;
rampInt ms_cyclotron_fade_out_led_6;
rampInt ms_cyclotron_fade_out_led_7;
rampInt ms_cyclotron_fade_out_led_8;
rampInt ms_cyclotron_fade_out_led_9;
rampInt ms_cyclotron_fade_out_led_10;
rampInt ms_cyclotron_fade_out_led_11;
rampInt ms_cyclotron_fade_out_led_12;
rampInt ms_cyclotron_fade_out_led_13;
rampInt ms_cyclotron_fade_out_led_14;
rampInt ms_cyclotron_fade_out_led_15;
rampInt ms_cyclotron_fade_out_led_16;
rampInt ms_cyclotron_fade_out_led_17;
rampInt ms_cyclotron_fade_out_led_18;
rampInt ms_cyclotron_fade_out_led_19;
rampInt ms_cyclotron_fade_out_led_20;
rampInt ms_cyclotron_fade_out_led_21;
rampInt ms_cyclotron_fade_out_led_22;
rampInt ms_cyclotron_fade_out_led_23;
rampInt ms_cyclotron_fade_out_led_24;
rampInt ms_cyclotron_fade_out_led_25;
rampInt ms_cyclotron_fade_out_led_26;
rampInt ms_cyclotron_fade_out_led_27;
rampInt ms_cyclotron_fade_out_led_28;
rampInt ms_cyclotron_fade_out_led_29;
rampInt ms_cyclotron_fade_out_led_30;
rampInt ms_cyclotron_fade_out_led_31;
rampInt ms_cyclotron_fade_out_led_32;
rampInt ms_cyclotron_fade_out_led_33;
rampInt ms_cyclotron_fade_out_led_34;
rampInt ms_cyclotron_fade_out_led_35;
rampInt ms_cyclotron_fade_out_led_36;
rampInt ms_cyclotron_fade_out_led_37;
rampInt ms_cyclotron_fade_out_led_38;
rampInt ms_cyclotron_fade_out_led_39;
rampInt ms_cyclotron_fade_out_led_40;
rampInt ms_cyclotron_fade_in_led_1;
rampInt ms_cyclotron_fade_in_led_2;
rampInt ms_cyclotron_fade_in_led_3;
rampInt ms_cyclotron_fade_in_led_4;
rampInt ms_cyclotron_fade_in_led_5;
rampInt ms_cyclotron_fade_in_led_6;
rampInt ms_cyclotron_fade_in_led_7;
rampInt ms_cyclotron_fade_in_led_8;
rampInt ms_cyclotron_fade_in_led_9;
rampInt ms_cyclotron_fade_in_led_10;
rampInt ms_cyclotron_fade_in_led_11;
rampInt ms_cyclotron_fade_in_led_12;
rampInt ms_cyclotron_fade_in_led_13;
rampInt ms_cyclotron_fade_in_led_14;
rampInt ms_cyclotron_fade_in_led_15;
rampInt ms_cyclotron_fade_in_led_16;
rampInt ms_cyclotron_fade_in_led_17;
rampInt ms_cyclotron_fade_in_led_18;
rampInt ms_cyclotron_fade_in_led_19;
rampInt ms_cyclotron_fade_in_led_20;
rampInt ms_cyclotron_fade_in_led_21;
rampInt ms_cyclotron_fade_in_led_22;
rampInt ms_cyclotron_fade_in_led_23;
rampInt ms_cyclotron_fade_in_led_24;
rampInt ms_cyclotron_fade_in_led_25;
rampInt ms_cyclotron_fade_in_led_26;
rampInt ms_cyclotron_fade_in_led_27;
rampInt ms_cyclotron_fade_in_led_28;
rampInt ms_cyclotron_fade_in_led_29;
rampInt ms_cyclotron_fade_in_led_30;
rampInt ms_cyclotron_fade_in_led_31;
rampInt ms_cyclotron_fade_in_led_32;
rampInt ms_cyclotron_fade_in_led_33;
rampInt ms_cyclotron_fade_in_led_34;
rampInt ms_cyclotron_fade_in_led_35;
rampInt ms_cyclotron_fade_in_led_36;
rampInt ms_cyclotron_fade_in_led_37;
rampInt ms_cyclotron_fade_in_led_38;
rampInt ms_cyclotron_fade_in_led_39;
rampInt ms_cyclotron_fade_in_led_40;
boolean i_cyclotron_led_on_status[40] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
rampInt ms_cyclotron_led_fade_out[40] = { ms_cyclotron_fade_out_led_1, ms_cyclotron_fade_out_led_2, ms_cyclotron_fade_out_led_3, ms_cyclotron_fade_out_led_4, ms_cyclotron_fade_out_led_5, ms_cyclotron_fade_out_led_6, ms_cyclotron_fade_out_led_7, ms_cyclotron_fade_out_led_8, ms_cyclotron_fade_out_led_9, ms_cyclotron_fade_out_led_10, ms_cyclotron_fade_out_led_11, ms_cyclotron_fade_out_led_12, ms_cyclotron_fade_out_led_13, ms_cyclotron_fade_out_led_14, ms_cyclotron_fade_out_led_15, ms_cyclotron_fade_out_led_16, ms_cyclotron_fade_out_led_17, ms_cyclotron_fade_out_led_18, ms_cyclotron_fade_out_led_19, ms_cyclotron_fade_out_led_20, ms_cyclotron_fade_out_led_21, ms_cyclotron_fade_out_led_22, ms_cyclotron_fade_out_led_23, ms_cyclotron_fade_out_led_24, ms_cyclotron_fade_out_led_25, ms_cyclotron_fade_out_led_26, ms_cyclotron_fade_out_led_27, ms_cyclotron_fade_out_led_28, ms_cyclotron_fade_out_led_29, ms_cyclotron_fade_out_led_30, ms_cyclotron_fade_out_led_31, ms_cyclotron_fade_out_led_32, ms_cyclotron_fade_out_led_33, ms_cyclotron_fade_out_led_34, ms_cyclotron_fade_out_led_35, ms_cyclotron_fade_out_led_36, ms_cyclotron_fade_out_led_37, ms_cyclotron_fade_out_led_38, ms_cyclotron_fade_out_led_39, ms_cyclotron_fade_out_led_40 };
rampInt ms_cyclotron_led_fade_in[40] = { ms_cyclotron_fade_in_led_1, ms_cyclotron_fade_in_led_2, ms_cyclotron_fade_in_led_3, ms_cyclotron_fade_in_led_4, ms_cyclotron_fade_in_led_5, ms_cyclotron_fade_in_led_6, ms_cyclotron_fade_in_led_7, ms_cyclotron_fade_in_led_8, ms_cyclotron_fade_in_led_9, ms_cyclotron_fade_in_led_10, ms_cyclotron_fade_in_led_11, ms_cyclotron_fade_in_led_12, ms_cyclotron_fade_in_led_13, ms_cyclotron_fade_in_led_14, ms_cyclotron_fade_in_led_15, ms_cyclotron_fade_in_led_16, ms_cyclotron_fade_in_led_17, ms_cyclotron_fade_in_led_18, ms_cyclotron_fade_in_led_19, ms_cyclotron_fade_in_led_20, ms_cyclotron_fade_in_led_21, ms_cyclotron_fade_in_led_22, ms_cyclotron_fade_in_led_23, ms_cyclotron_fade_in_led_24, ms_cyclotron_fade_in_led_25, ms_cyclotron_fade_in_led_26, ms_cyclotron_fade_in_led_27, ms_cyclotron_fade_in_led_28, ms_cyclotron_fade_in_led_29, ms_cyclotron_fade_in_led_30, ms_cyclotron_fade_in_led_31, ms_cyclotron_fade_in_led_32, ms_cyclotron_fade_in_led_33, ms_cyclotron_fade_in_led_34, ms_cyclotron_fade_in_led_35, ms_cyclotron_fade_in_led_36, ms_cyclotron_fade_in_led_37, ms_cyclotron_fade_in_led_38, ms_cyclotron_fade_in_led_39, ms_cyclotron_fade_in_led_40 };
int i_cyclotron_led_value[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int i_1984_counter = 0;

/*
 * Inner cyclotron NeoPixel ring ramp control.
 * This is for the 35 LED NeoPixel ring option.
 */
millisDelay ms_cyclotron_ring;
rampInt r_inner_ramp;
const int i_inner_delay = i_2021_inner_delay;
const int i_inner_ramp_delay = 300;
int i_led_cyclotron_ring = 0;
bool b_inner_ramp_up = true;
bool b_inner_ramp_down = false;
int i_inner_current_ramp_speed = i_inner_ramp_delay;

/*
 * Cyclotron Switch Plate LEDs
 */
const int cyclotron_sw_plate_led_r1 = 4;
const int cyclotron_sw_plate_led_r2 = 5;
const int cyclotron_sw_plate_led_y1 = 6;
const int cyclotron_sw_plate_led_y2 = 7;
const int cyclotron_sw_plate_led_g1 = 8;
const int cyclotron_sw_plate_led_g2 = 9;
const int cyclotron_switch_led_green = 10; // 1984/2021 mode switch led.
const int cyclotron_switch_led_yellow = 11; // Vibration on/off switch led.
millisDelay ms_cyclotron_switch_plate_leds;
const int i_cyclotron_switch_plate_leds_delay = 1000;

/* 
 * Alarm. Used during overheating and or ribbon cable removal.
 */
const int i_alarm_delay = 500;
bool b_alarm = false;
millisDelay ms_alarm;

/* 
 *  Switches
 */
ezButton switch_cyclotron_lid(51); // Second cyclotron ground pin (brown) that we feed to the Arduino to detect if the lid is removed or not.
ezButton switch_alarm(23); // Ribbon cable removal switch
ezButton switch_mode(25); // 1984 / 2021 mode toggle switch
ezButton switch_vibration(27); // Vibration toggle switch
ezButton switch_cyclotron_direction(29); // Newly added switch for controlling the direction of the cyclotron lights. Not required. Defaults to clockwise.
ezButton switch_power(31); // Red power switch under the ion arm.
ezButton switch_smoke(37); // Switch to enable smoke effects. Not required. Defaults to off/disabled.

/* 
 *  Wav trigger
 */
wavTrigger w_trig;
int i_music_count = 0;
int i_current_music_track = 0;
const int i_music_track_start = 100; // Music tracks start on file named 100_ and higher.
boolean b_playing_music = false;
boolean b_repeat_track = false;

/* 
 *  Volume (0 = loudest, -70 = quietest)
 */
int i_volume_percentage = STARTUP_VOLUME_EFFECTS; // Sound effects
int i_volume_master_percentage = STARTUP_VOLUME; // Master overall volume
int i_volume_music_percentage = STARTUP_VOLUME_MUSIC; // Music volume

int i_volume = MINIMUM_VOLUME - (MINIMUM_VOLUME * i_volume_percentage / 100); // Sound effects
int i_volume_master = MINIMUM_VOLUME - (MINIMUM_VOLUME * i_volume_master_percentage / 100); // Master overall volume
int i_volume_music = MINIMUM_VOLUME - (MINIMUM_VOLUME * i_volume_music_percentage / 100); // Music volume

millisDelay ms_volume_check; // Put some timing on the master volume gain to not overload the wav trigger serial communication.

/*
 * Vibration motor settings
 */
const int vibration = 45;
int i_vibration_level = 0;
int i_vibration_level_prev = 0;
boolean b_vibration = false;

/*
 * Smoke
 */
const int smoke_pin = 39;

/*
 * Smoke for a second smoke machine or motor. I use this in the booster tube.
 */
const int smoke_booster_pin = 35;

/*
 * Fan for smoke
 */
const int fan_pin = 33;
millisDelay ms_fan_stop_timer;
const int i_fan_stop_timer = 7000;

/* 
 * Overheating and smoke timers for smoke_pin. 
 */
millisDelay ms_overheating;
const int i_overheating_delay = 4000;
boolean b_overheating = false;
millisDelay ms_smoke_timer;
millisDelay ms_smoke_on;
const int i_smoke_timer[5] = { i_smoke_timer_mode_1, i_smoke_timer_mode_2, i_smoke_timer_mode_3, i_smoke_timer_mode_4, i_smoke_timer_mode_5 };
const int i_smoke_on_time[5] = { i_smoke_on_time_mode_1, i_smoke_on_time_mode_2, i_smoke_on_time_mode_3, i_smoke_on_time_mode_4, i_smoke_on_time_mode_5 };
const boolean b_smoke_continuous_mode[5] = { b_smoke_continuous_mode_1, b_smoke_continuous_mode_2, b_smoke_continuous_mode_3, b_smoke_continuous_mode_4, b_smoke_continuous_mode_5 };
const boolean b_smoke_overheat_mode[5] = { b_smoke_overheat_mode_1, b_smoke_overheat_mode_2, b_smoke_overheat_mode_3, b_smoke_overheat_mode_4, b_smoke_overheat_mode_5 };

/*
 * Vent light timers and delay for over heating.
 */
millisDelay ms_vent_light_on;
millisDelay ms_vent_light_off;
const int i_vent_light_delay = 50;
boolean b_vent_sounds; // A flag for playing smoke and vent sounds.
bool b_vent_light_on = false; // To know if the light is on or off.

/* 
 *  Wand communication
 */
enum FIRING_MODES { PROTON, SLIME, STASIS, MESON, SETTINGS };
enum FIRING_MODES FIRING_MODE;
boolean b_wand_firing = false;
boolean b_wand_connected = false;
millisDelay ms_wand_handshake;
const int i_wand_handshake_delay = 3000;
millisDelay ms_wand_handshake_checking;
int i_wand_power_level = 1; // Power level of the wand.
int rx_byte = 0;
int prev_byte = 0;

/*
 * Firing timers
 */
millisDelay ms_firing_length_timer;
const int i_firing_timer_length = 15000; // 15 seconds. Used by ms_firing_length_timer to determine which tail_end sound effects to play.

/* 
 * Rotary encoder for volume control 
 */
#define encoder_pin_a 2
#define encoder_pin_b 3
int i_encoder_pos = 0;
int i_val_rotary;
int i_last_val_rotary;

/*
 * Misc.
 */
int i_mode_year = 2021; // 1984 or 2021
bool b_pack_on = false;
bool b_pack_shutting_down = false;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600); // Communication to the wand.

  // Setup the Wav Trigger.
  setupWavTrigger();
  
  // Rotary encoder for volume control.
  pinMode(encoder_pin_a, INPUT_PULLUP);
  pinMode(encoder_pin_b, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoder_pin_a), readEncoder, CHANGE);

  // Configure the various switches on the pack.
  switch_cyclotron_lid.setDebounceTime(50);
  switch_alarm.setDebounceTime(50);
  switch_mode.setDebounceTime(50);
  switch_vibration.setDebounceTime(50);
  switch_cyclotron_direction.setDebounceTime(50);
  switch_smoke.setDebounceTime(50);
  
  // Adjust the pwm frequency of the vibration motor.
  TCCR5B = (TCCR5B & B11111000) | (B00000100);  // for PWM frequency of 122.55 Hz

  // Vibration motor
  pinMode(vibration, OUTPUT);

  // Smoke motor
  pinMode(smoke_pin, OUTPUT);

  // Second smoke motor (booster tube)
  pinMode(smoke_booster_pin, OUTPUT);

  // Fan pin for smoke
  pinMode(fan_pin, OUTPUT);
  
  // Powercell and cyclotron LEDs.
  FastLED.addLeds<NEOPIXEL, PACK_LED_PIN>(pack_leds, PACK_NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, CYCLOTRON_LED_PIN>(cyclotron_leds, CYCLOTRON_NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,500);  // Limit draw to 500mA at 5v of power.
   
  // Cyclotron Switch Panel LEDs
  pinMode(cyclotron_sw_plate_led_r1, OUTPUT);
  pinMode(cyclotron_sw_plate_led_r2, OUTPUT);
  pinMode(cyclotron_sw_plate_led_y1, OUTPUT);
  pinMode(cyclotron_sw_plate_led_y2, OUTPUT);
  pinMode(cyclotron_sw_plate_led_g1, OUTPUT);
  pinMode(cyclotron_sw_plate_led_g2, OUTPUT);
  pinMode(cyclotron_switch_led_green, OUTPUT);
  pinMode(cyclotron_switch_led_yellow, OUTPUT);
  
  // Misc configuration before startup.
  resetCyclotronLeds();

  // Bootup the pack into Proton mode, the same as the wand.
  FIRING_MODE = PROTON;

  switch(i_mode_year) {
    case 1984:
      i_current_ramp_speed = i_1984_delay * 1.3;
      i_inner_current_ramp_speed = i_inner_ramp_delay;
    break;

    case 2021:          
      i_current_ramp_speed = i_2021_ramp_delay;
      i_inner_current_ramp_speed = i_inner_ramp_delay;
    break;
  }

  // Start some timers
  ms_cyclotron.start(i_current_ramp_speed);
  ms_cyclotron_ring.start(i_inner_current_ramp_speed);
  ms_cyclotron_switch_plate_leds.start(i_cyclotron_switch_plate_leds_delay);
  ms_wand_handshake.start(1);
  ms_fast_led.start(i_fast_led_delay);

  // Tell the wand the pack is here.
  Serial2.write(0);
}

void loop() {
  w_trig.update();
  
  cyclotronSwitchPlateLEDs();

  wandHandShake();
  checkWand();
  checkFan();
  
  switch_cyclotron_lid.loop();
  switch_alarm.loop();
  switch_cyclotron_direction.loop();
  switch_mode.loop();
  switch_vibration.loop();
  switch_power.loop();
  switch_smoke.loop();

  checkSwitches();
  checkRotaryEncoder();

  switch (PACK_STATUS) {
    case MODE_OFF:
      if(b_pack_on == true) {
        b_2021_ramp_up = false;
        b_2021_ramp_up_start = false;
        b_inner_ramp_up = false;

        reset2021RampDown();

        b_pack_shutting_down = true;
      }
      
      if(b_2021_ramp_down == true && b_overheating == false && b_alarm == false) {
        cyclotronSwitchLEDLoop();        
        powercellLoop();
        cyclotronControl();
      }
      else {   
        powercellOff();     
        cyclotronSwitchLEDOff();

        // Reset the power cell timer.
        ms_powercell.stop();
        ms_powercell.start(i_powercell_delay);

        // Reset the cyclotron led switch timer.
        ms_cyclotron_switch_led.stop();
        ms_cyclotron_switch_led.start(i_cyclotron_switch_led_delay);
        
        // Need to reset the cyclotron timers.
        ms_cyclotron.start(i_2021_delay);
        ms_cyclotron_ring.start(i_inner_ramp_delay);
        
        b_overheating = false;
        b_2021_ramp_down = false;
        b_2021_ramp_down_start = false;
        b_reset_start_led = true; // reset the start led of the cyclotron.
        b_inner_ramp_down = false;
                
        resetCyclotronLeds();        
        reset2021RampUp();

        // Update Cyclotron LED timer delay and optional cyclotron led switch plate LED timers delays.
        switch(i_mode_year) {
          case 2021:
            i_powercell_delay = i_powercell_delay_2021;
            i_cyclotron_switch_led_delay = i_cyclotron_switch_led_delay_base;
          break;

          case 1984:
            i_powercell_delay = i_powercell_delay_1984;
            i_cyclotron_switch_led_delay = i_cyclotron_switch_led_delay_base * 4;
          break;
        }

        // Vibration motor off.
        vibrationPack(0);
        i_vibration_level = 0;

        if(b_pack_shutting_down == true) {
          b_pack_shutting_down = false;
        }
      }
      
      if(b_pack_on == true) {
        // Tell the wand the pack is off, so shut down the wand as well if it is still on.
        Serial2.write(2);
      }

      b_pack_on = false;
    break;
   
    case MODE_ON:
      if(b_pack_shutting_down == true) {
        b_pack_shutting_down = false;
      }
      
      if(b_pack_on == false) {
        // Tell the wand the pack is on.
        Serial2.write(1);
      }
      
      b_pack_on = true;
      
      if(b_2021_ramp_down == true) {
        b_2021_ramp_down = false;
        b_2021_ramp_down_start = false;
        b_inner_ramp_down = false;

        reset2021RampUp();
      }
        
      if(switch_alarm.getState() == LOW && b_overheating == false) {
        if(b_alarm == true) {
          // Tell the wand the pack alarm is off.
          Serial2.write(4);

          if(i_mode_year == 1984) {
            // Reset the LEDs before resetting the alarm flag.
            resetCyclotronLeds();
            ms_cyclotron.start(0);
          }
          else {
            ms_cyclotron.start(i_current_ramp_speed);
          }

          ms_cyclotron_ring.start(i_inner_current_ramp_speed);  

          ventLight(false);
          
          b_alarm = false;

          reset2021RampUp();
          
          packStartup(); 
        }
      }   

      // Play a little bit of smoke and n-filter vent lights while firing. Just a tiny bit....
      if(b_wand_firing == true) {
        if(ms_smoke_on.justFinished()) {
          ms_smoke_on.stop();
          ms_smoke_timer.start(i_smoke_timer[i_wand_power_level - 1]);
          b_vent_sounds = true;
        }
        
        if(ms_smoke_timer.justFinished()) {
          if(ms_smoke_on.isRunning() != true) {
            ms_smoke_on.start(i_smoke_on_time[i_wand_power_level - 1]);
          }
        }

        if(ms_smoke_on.isRunning() == true) {
          // Turn on some smoke and play some vent sounds if smoke is enabled.
          if(b_smoke_enabled == true) {
            // Turn on some smoke.
            smokeControl(true);
  
            // Play some sounds with the smoke and vent lighting.
            if(b_vent_sounds == true) {
              w_trig.trackGain(S_VENT_SMOKE, i_volume);
              w_trig.trackPlayPoly(S_VENT_SMOKE);
              w_trig.trackGain(S_SPARKS_LOOP, i_volume);
              w_trig.trackPlayPoly(S_SPARKS_LOOP);
  
              b_vent_sounds = false;
            }
          
            fanControl(true);
          }
          
          if(ms_vent_light_off.justFinished()) {
            ms_vent_light_off.stop();
            ms_vent_light_on.start(i_vent_light_delay);

            ventLight(true);
          }
          else if(ms_vent_light_on.justFinished()) {
            ms_vent_light_on.stop();
            ms_vent_light_off.start(i_vent_light_delay);
        
            ventLight(false);
          }
        }
        else {
          smokeControl(false);
          ventLight(false);
          fanControl(false);
        }
      }
      
      cyclotronSwitchLEDLoop();
      powercellLoop();
      cyclotronControl();
    break;
   }

  switch(PACK_ACTION_STATUS) {
    case ACTION_IDLE:
      // Do nothing.
    break;

    case ACTION_OFF:
      packShutdown();
    break;

    case ACTION_ACTIVATE:
      packStartup();
    break;
  }

  // Update the LEDs
  if(ms_fast_led.justFinished()) {
    //FastLED.delay(3);
    FastLED.show();
    ms_fast_led.start(i_fast_led_delay);
  }
}

void packStartup() {
  PACK_STATUS = MODE_ON;
  PACK_ACTION_STATUS = ACTION_IDLE;

  if(b_alarm == true) {
    if(i_mode_year == 1984) {
      ms_cyclotron.start(0);
      ms_alarm.start(0);
    }

    packAlarm();
  }
  else {
    w_trig.trackStop(S_PACK_SHUTDOWN_AFTERLIFE);
    
    switch(i_mode_year) {
      case 1984:
        w_trig.trackStop(S_PACK_RIBBON_ALARM_1);

        if(b_gb2_mode == true) {
          w_trig.trackGain(S_GB2_PACK_START, i_volume);
          w_trig.trackPlayPoly(S_GB2_PACK_START, true);

          w_trig.trackGain(S_GB2_PACK_LOOP, i_volume - 20);
          w_trig.trackPlayPoly(S_GB2_PACK_LOOP, true);
          w_trig.trackFade(S_GB2_PACK_LOOP, i_volume, 2000, 0);
          w_trig.trackLoop(S_GB2_PACK_LOOP, 1);
        }
        else {
          w_trig.trackGain(S_BOOTUP, i_volume);
          w_trig.trackPlayPoly(S_BOOTUP, true);

          w_trig.trackGain(S_IDLE_LOOP, i_volume - 20);
          w_trig.trackPlayPoly(S_IDLE_LOOP, true);
          w_trig.trackFade(S_IDLE_LOOP, i_volume, 2000, 0);
          w_trig.trackLoop(S_IDLE_LOOP, 1);
        }
      break;
  
      default:
        w_trig.trackStop(S_PACK_RIBBON_ALARM_2);
        w_trig.trackGain(S_AFTERLIFE_PACK_STARTUP, i_volume);
        w_trig.trackPlayPoly(S_AFTERLIFE_PACK_STARTUP, true);
  
        w_trig.trackGain(S_AFTERLIFE_PACK_IDLE_LOOP, i_volume - 20);
        w_trig.trackPlayPoly(S_AFTERLIFE_PACK_IDLE_LOOP, true);
        w_trig.trackFade(S_AFTERLIFE_PACK_IDLE_LOOP, i_volume, 15000, 0);
        w_trig.trackLoop(S_AFTERLIFE_PACK_IDLE_LOOP, 1);
      break;
    }
  }
}

void packShutdown() {
  PACK_STATUS = MODE_OFF;
  PACK_ACTION_STATUS = ACTION_IDLE;
  
  // Stop the firing if the pack is doing it.
  wandStoppedFiring();

  switch(i_mode_year) {
    case 1984:
      w_trig.trackStop(S_PACK_RIBBON_ALARM_1);
    break;

    default:
      w_trig.trackStop(S_PACK_RIBBON_ALARM_2);
    break;
  }

  w_trig.trackStop(S_BEEP_8);
  w_trig.trackStop(S_SHUTDOWN);

  w_trig.trackStop(S_GB2_PACK_START);
  w_trig.trackStop(S_GB2_PACK_LOOP);
  w_trig.trackStop(S_GB2_PACK_OFF);
  w_trig.trackStop(S_PACK_SHUTDOWN);
  w_trig.trackStop(S_PACK_SHUTDOWN_AFTERLIFE);
  w_trig.trackStop(S_IDLE_LOOP);
  w_trig.trackStop(S_BOOTUP);
  w_trig.trackStop(S_AFTERLIFE_PACK_STARTUP);
  w_trig.trackStop(S_AFTERLIFE_PACK_IDLE_LOOP);

  if(b_alarm != true) {
    switch(i_mode_year) {
      case 1984:      
        w_trig.trackGain(S_SHUTDOWN, i_volume);
        w_trig.trackPlayPoly(S_SHUTDOWN, true);

        if(b_gb2_mode == true) {
          w_trig.trackGain(S_GB2_PACK_OFF, i_volume);
          w_trig.trackPlayPoly(S_GB2_PACK_OFF, true);
        }
        else {
          w_trig.trackGain(S_PACK_SHUTDOWN, i_volume);
          w_trig.trackPlayPoly(S_PACK_SHUTDOWN, true);
        }
      break;
  
      default:
        w_trig.trackGain(S_PACK_SHUTDOWN_AFTERLIFE, i_volume);
        w_trig.trackPlayPoly(S_PACK_SHUTDOWN_AFTERLIFE, true);
      break;
    }
  }
  else {
    w_trig.trackGain(S_SHUTDOWN, i_volume);
    w_trig.trackPlayPoly(S_SHUTDOWN, true);
  }

  // Turn off the vent light if it is on.
  ventLight(false);
  ms_vent_light_off.stop();
  ms_vent_light_on.stop();

  // Turn off any smoke.
  smokeControl(false);
  ms_smoke_timer.stop();
  ms_smoke_on.stop();
  
  // Turn off the n-filter fan.
  ms_fan_stop_timer.stop();
  fanControl(false);
  
  // Reset vent sounds flag.
  b_vent_sounds = true;
}

void checkSwitches() {
  // Cyclotron direction toggle switch.
  if(switch_cyclotron_direction.isPressed() || switch_cyclotron_direction.isReleased()) {
    if(b_clockwise == true) {
      b_clockwise = false;

      w_trig.trackStop(S_BEEPS_ALT);    
      w_trig.trackGain(S_BEEPS_ALT, i_volume);
      w_trig.trackPlayPoly(S_BEEPS_ALT);
    }
    else {
      b_clockwise = true;

      w_trig.trackStop(S_BEEPS);
      w_trig.trackGain(S_BEEPS, i_volume);
      w_trig.trackPlayPoly(S_BEEPS);
    }
  }
  
  // Smoke
  if(switch_smoke.isPressed() || switch_smoke.isReleased()) {
    if(b_smoke_enabled == true) {
      b_smoke_enabled = false;

      w_trig.trackStop(S_VENT_DRY);
      w_trig.trackGain(S_VENT_DRY, i_volume);
      w_trig.trackPlayPoly(S_VENT_DRY);
    }
    else {
      b_smoke_enabled = true;

      w_trig.trackStop(S_VENT_SMOKE);
      w_trig.trackGain(S_VENT_SMOKE, i_volume);
      w_trig.trackPlayPoly(S_VENT_SMOKE);
    }
  }

  // Vibration toggle switch.
  if(switch_vibration.getState() == LOW) {
    if(b_vibration == false) {
      // Tell the wand to turn vibration on.
      Serial2.write(5);
      
      w_trig.trackStop(S_BEEPS_ALT);    
      w_trig.trackGain(S_BEEPS_ALT, i_volume);
      w_trig.trackPlayPoly(S_BEEPS_ALT);

      b_vibration = true;
    }
  }
  else {
    if(b_vibration == true) {
      // Tell the wand to turn vibration off.
      Serial2.write(6);

      w_trig.trackStop(S_BEEPS_ALT);    
      w_trig.trackGain(S_BEEPS_ALT, i_volume);
      w_trig.trackPlayPoly(S_BEEPS_ALT);

      b_vibration = false;
    }
  }  

  // Play sound when the mode switch is pressed or released.
  if(switch_mode.isPressed() || switch_mode.isReleased()) {
    w_trig.trackStop(S_BEEPS_BARGRAPH);    
    w_trig.trackGain(S_BEEPS_BARGRAPH, i_volume);
    w_trig.trackPlayPoly(S_BEEPS_BARGRAPH);
  }
  
  switch(PACK_STATUS) {
    case MODE_OFF:     
      if(switch_power.isPressed() || switch_power.isReleased()) {
        // Turn the pack on.
        PACK_ACTION_STATUS = ACTION_ACTIVATE;
      }

      // Year mode. Best to adjust it only when the pack is off.
      if(b_2021_ramp_down != true && b_pack_on == false) {        
        if(switch_mode.getState() == LOW) {
          if(i_mode_year == 2021) {
            // Tell the wand to switch to 1984 mode.
            Serial2.write(7);
          }
          
          // Reset the ramp speeds.
          i_current_ramp_speed = i_1984_delay * 1.3;
          i_inner_current_ramp_speed = i_inner_ramp_delay;

          i_mode_year = 1984;
        }
        else {
          if(i_mode_year == 1984) {
            // Tell the wand to switch to 2021 mode.
            Serial2.write(8);
          }
          
          // Reset the ramp speeds.
          i_current_ramp_speed = i_2021_ramp_delay;
          i_inner_current_ramp_speed = i_inner_ramp_delay;

          i_mode_year = 2021;
        }
      }
    break;

    case MODE_ON:
      if(switch_power.isReleased() || switch_power.isPressed()) {
        // Turn the pack off.
        PACK_ACTION_STATUS = ACTION_OFF;
      }
    break;
  }
}

void cyclotronSwitchLEDOff() {
  digitalWrite(cyclotron_sw_plate_led_r1, LOW);
  digitalWrite(cyclotron_sw_plate_led_r2, LOW);
  
  digitalWrite(cyclotron_sw_plate_led_y1, LOW);
  digitalWrite(cyclotron_sw_plate_led_y2, LOW);
  
  digitalWrite(cyclotron_sw_plate_led_g1, LOW);
  digitalWrite(cyclotron_sw_plate_led_g2, LOW);
  
  i_cyclotron_sw_led = 0;
}

void cyclotronSwitchLEDLoop() {
  if(ms_cyclotron_switch_led.justFinished()) {
    if(b_cyclotron_lid_on != true) {
      if(b_alarm == true) {
        if(i_cyclotron_sw_led > 0) {
          digitalWrite(cyclotron_sw_plate_led_r1, HIGH);
          digitalWrite(cyclotron_sw_plate_led_r2, HIGH);
          
          digitalWrite(cyclotron_sw_plate_led_y1, HIGH);
          digitalWrite(cyclotron_sw_plate_led_y2, HIGH);
          
          digitalWrite(cyclotron_sw_plate_led_g1, HIGH);
          digitalWrite(cyclotron_sw_plate_led_g2, HIGH);
            
          i_cyclotron_sw_led = 0;
        }
        else {
          digitalWrite(cyclotron_sw_plate_led_r1, LOW);
          digitalWrite(cyclotron_sw_plate_led_r2, LOW);
          
          digitalWrite(cyclotron_sw_plate_led_y1, LOW);
          digitalWrite(cyclotron_sw_plate_led_y2, LOW);
          
          digitalWrite(cyclotron_sw_plate_led_g1, LOW);
          digitalWrite(cyclotron_sw_plate_led_g2, LOW);
            
          i_cyclotron_sw_led++;
        }
      }
      else {
        switch(i_cyclotron_sw_led) {
          case 0:
            digitalWrite(cyclotron_sw_plate_led_r1, LOW);
            digitalWrite(cyclotron_sw_plate_led_r2, LOW);
            
            digitalWrite(cyclotron_sw_plate_led_y1, LOW);
            digitalWrite(cyclotron_sw_plate_led_y2, LOW);
            
            digitalWrite(cyclotron_sw_plate_led_g1, LOW);
            digitalWrite(cyclotron_sw_plate_led_g2, LOW);
      
            i_cyclotron_sw_led++;
          break;
      
          case 1:
            digitalWrite(cyclotron_sw_plate_led_r1, LOW);
            digitalWrite(cyclotron_sw_plate_led_r2, LOW);
            
            digitalWrite(cyclotron_sw_plate_led_y1, LOW);
            digitalWrite(cyclotron_sw_plate_led_y2, LOW);
            
            digitalWrite(cyclotron_sw_plate_led_g1, HIGH);
            digitalWrite(cyclotron_sw_plate_led_g2, HIGH);
      
            i_cyclotron_sw_led++;
          break;
      
          case 2:
            digitalWrite(cyclotron_sw_plate_led_r1, LOW);
            digitalWrite(cyclotron_sw_plate_led_r2, LOW);
            
            digitalWrite(cyclotron_sw_plate_led_y1, HIGH);
            digitalWrite(cyclotron_sw_plate_led_y2, HIGH);
            
            digitalWrite(cyclotron_sw_plate_led_g1, HIGH);
            digitalWrite(cyclotron_sw_plate_led_g2, HIGH);
      
            i_cyclotron_sw_led++;
          break;
      
          case 3:
            digitalWrite(cyclotron_sw_plate_led_r1, HIGH);
            digitalWrite(cyclotron_sw_plate_led_r2, HIGH);
            
            digitalWrite(cyclotron_sw_plate_led_y1, HIGH);
            digitalWrite(cyclotron_sw_plate_led_y2, HIGH);
            
            digitalWrite(cyclotron_sw_plate_led_g1, HIGH);
            digitalWrite(cyclotron_sw_plate_led_g2, HIGH);
      
            i_cyclotron_sw_led++;
          break;
      
          case 4:
            digitalWrite(cyclotron_sw_plate_led_r1, HIGH);
            digitalWrite(cyclotron_sw_plate_led_r2, HIGH);
            
            digitalWrite(cyclotron_sw_plate_led_y1, HIGH);
            digitalWrite(cyclotron_sw_plate_led_y2, HIGH);
            
            digitalWrite(cyclotron_sw_plate_led_g1, HIGH);
            digitalWrite(cyclotron_sw_plate_led_g2, HIGH);
      
            i_cyclotron_sw_led++;
          break;
          
          case 5:
            digitalWrite(cyclotron_sw_plate_led_r1, HIGH);
            digitalWrite(cyclotron_sw_plate_led_r2, HIGH);
            
            digitalWrite(cyclotron_sw_plate_led_y1, HIGH);
            digitalWrite(cyclotron_sw_plate_led_y2, HIGH);
            
            digitalWrite(cyclotron_sw_plate_led_g1, LOW);
            digitalWrite(cyclotron_sw_plate_led_g2, LOW);
      
            i_cyclotron_sw_led++;
          break;
      
          case 6:
            digitalWrite(cyclotron_sw_plate_led_r1, HIGH);
            digitalWrite(cyclotron_sw_plate_led_r2, HIGH);
            
            digitalWrite(cyclotron_sw_plate_led_y1, LOW);
            digitalWrite(cyclotron_sw_plate_led_y2, LOW);
            
            digitalWrite(cyclotron_sw_plate_led_g1, LOW);
            digitalWrite(cyclotron_sw_plate_led_g2, LOW);
      
            i_cyclotron_sw_led++;
          break;  
      
          case 7:
            digitalWrite(cyclotron_sw_plate_led_r1, LOW);
            digitalWrite(cyclotron_sw_plate_led_r2, LOW);
            
            digitalWrite(cyclotron_sw_plate_led_y1, LOW);
            digitalWrite(cyclotron_sw_plate_led_y2, LOW);
            
            digitalWrite(cyclotron_sw_plate_led_g1, LOW);
            digitalWrite(cyclotron_sw_plate_led_g2, LOW);
      
            i_cyclotron_sw_led = 0;
          break;
        }
      }
    }
    else {
      // No need to have the inner cyclotron switch plate LED's on when the lid is on.
     cyclotronSwitchLEDOff();
    }
    
    // Setup the delays again.
    int i_cyc_led_delay = i_cyclotron_switch_led_delay / i_cyclotron_switch_led_mulitplier;

    switch(i_mode_year) {
      case 2021:
        if(b_2021_ramp_up == true) {
          i_cyc_led_delay = i_cyclotron_switch_led_delay + (i_2021_ramp_delay - r_2021_ramp.update());
        }
        else if(b_2021_ramp_down == true) {
          i_cyc_led_delay = i_cyclotron_switch_led_delay + r_2021_ramp.update();
        }
      break;

      case 1984:
        if(b_2021_ramp_up == true) {
          i_cyc_led_delay = i_cyclotron_switch_led_delay + (r_2021_ramp.update() - i_1984_delay);
        }
        else if(b_2021_ramp_down == true) {
          i_cyc_led_delay = i_cyclotron_switch_led_delay / 6 + r_2021_ramp.update();
        }
      break;
    }

    if(b_alarm == true) {
      i_cyc_led_delay = i_cyclotron_switch_led_delay * 2;
    }

    ms_cyclotron_switch_led.start(i_cyc_led_delay);    
  }
}

void powercellLoop() {
  if(ms_powercell.justFinished()) {
    int i_extra_delay = 0;
    
    // Powercell
    if(i_powercell_led > cyclotron_led_start - 1) {
      powercellOff();
  
      i_powercell_led = 0;
    }
    else {
      pack_leds[i_powercell_led] = CRGB(0,0,255);

      // Add a small delay to pause the powercell when all powercell LEDs are lit up, to match the 2021 pack.
      if(i_mode_year == 2021 && b_alarm != true && i_powercell_led == cyclotron_led_start - 1) {
        i_extra_delay = 250;
      }
      
      i_powercell_led++;
    }
    
    // Setup the delays again.
    int i_pc_delay = i_powercell_delay;

    switch(i_mode_year) {
      case 1984:
        if(b_2021_ramp_up == true) {
          i_pc_delay = i_powercell_delay + (r_2021_ramp.update() - i_1984_delay);
        }
        else if(b_2021_ramp_down == true) {
          i_pc_delay = i_powercell_delay + (r_2021_ramp.update() - i_1984_delay);
        }
      break;

      case 2021:
        if(b_2021_ramp_up == true) {
          i_pc_delay = i_powercell_delay + r_2021_ramp.update();
        }
        else if(b_2021_ramp_down == true) {
          i_pc_delay = i_powercell_delay + r_2021_ramp.update();
        }
      break;
    }

    if(b_alarm == true) {
      i_pc_delay = i_powercell_delay * 5;
    }

    ms_powercell.start(i_pc_delay + i_extra_delay);
  }
}

void powercellOff() {
  for(int i = 0; i <= cyclotron_led_start - 1; i++) {
    pack_leds[i] = CRGB(0,0,0);
  }
   
  i_powercell_led = 0;
}

void cyclotronControl() {
  // Only reset the starting led when the pack is first started up.
  if(b_reset_start_led == true) {
    b_reset_start_led = false;
    if(b_clockwise == false) {
      if(i_mode_year == 2021) {
        i_led_cyclotron = cyclotron_led_start + 2; // Start on LED #2 in anti-clockwise mode in 2021 mode.
      }
      else {
        i_1984_counter = 1;
        i_led_cyclotron = cyclotron_led_start + i_1984_cyclotron_leds[i_1984_counter] - 2;      
      }
    }
    else {
      if(i_mode_year == 1984) {
        i_1984_counter = 3;
        i_led_cyclotron = cyclotron_led_start + i_1984_cyclotron_leds[i_1984_counter] - 2;
        
      }
      else {
        i_led_cyclotron = cyclotron_led_start;
      }
    }
  }
        
  if(switch_alarm.getState() == HIGH && PACK_STATUS != MODE_OFF && b_2021_ramp_down_start != true && b_overheating == false) {
    if(b_alarm == false) {
      w_trig.trackStop(S_BEEP_8);

      b_2021_ramp_up = false;
      b_inner_ramp_up = false;
      b_alarm = true;
      
      if(i_mode_year == 1984) {
        resetCyclotronLeds();
        ms_cyclotron.start(0);
        ms_alarm.start(0);
      }
      else {
        ms_alarm.start(i_alarm_delay);
      }

      packAlarm();

      // Tell the wand the pack alarm is on.
      Serial2.write(3);
    }
    
    // Ribbon cable has been removed.
    cyclotronNoCable();
  }
  else if(b_overheating == true) {
    if(b_alarm == false) {
      w_trig.trackStop(S_BEEP_8);

      b_2021_ramp_up = false;
      b_inner_ramp_up = false;
      
      if(i_mode_year == 1984) {
        resetCyclotronLeds();
        ms_cyclotron.start(0);
        ms_alarm.start(0);
      }
      else {
        ms_alarm.start(i_alarm_delay);
      }

      b_alarm = true;

      packAlarm();
    }
    
    cyclotronOverHeating();
  }
  else {
    if(b_2021_ramp_up_start == true) {
      b_2021_ramp_up_start = false;

      if(i_mode_year == 1984) {
        r_2021_ramp.go(i_current_ramp_speed); // Reset the ramp.
        r_2021_ramp.go(i_1984_delay, i_1984_ramp_length, CIRCULAR_OUT);

        r_inner_ramp.go(i_inner_current_ramp_speed); // Inner cyclotron ramp reset.
        r_inner_ramp.go(i_1984_inner_delay, i_1984_ramp_length, CIRCULAR_OUT);
      }
      else {
        r_2021_ramp.go(i_current_ramp_speed); // Reset the ramp.
        r_2021_ramp.go(i_2021_delay, i_2021_ramp_length, CIRCULAR_OUT);
        r_inner_ramp.go(i_inner_current_ramp_speed);
        r_inner_ramp.go(i_2021_inner_delay, i_2021_ramp_length, CIRCULAR_OUT);
      }
    }
    else if(b_2021_ramp_down_start == true) {
      b_2021_ramp_down_start = false;

      r_2021_ramp.go(i_current_ramp_speed); // Reset the ramp.
      r_inner_ramp.go(i_inner_current_ramp_speed); // Reset the inner cyclotron ramp.

      if(i_mode_year == 1984) {
        r_2021_ramp.go(i_1984_delay * 1.3, i_1984_ramp_down_length, CIRCULAR_IN); 

        r_inner_ramp.go(i_inner_ramp_delay, i_1984_ramp_down_length, CIRCULAR_IN); 
      }
      else {
        r_2021_ramp.go(i_2021_ramp_delay, i_2021_ramp_down_length, CIRCULAR_IN);
        r_inner_ramp.go(i_inner_ramp_delay, i_2021_ramp_down_length, CIRCULAR_IN);
      }
    }
  
    if(i_mode_year == 1984) {
      cyclotron1984(i_current_ramp_speed);
      innerCyclotronRing(i_inner_current_ramp_speed);
    }
    else {
      cyclotron2021(i_current_ramp_speed);
      innerCyclotronRing(i_inner_current_ramp_speed);
    }
  }

  cyclotronFade();
}

void cyclotronFade() {
  // Colour control for the inner cyclotron leds. (red,green,blue)
  int r = 0;
  int g = 0;
  int b = 0;

  switch (i_mode_year) {
    case 2021:
      for(int i = 0; i < PACK_NUM_LEDS - 7 - cyclotron_led_start; i++) {
        if(ms_cyclotron_led_fade_in[i].isRunning()) {
          i_cyclotron_led_on_status[i] = true;
          int i_led = ms_cyclotron_led_fade_in[i].update();

          switch(FIRING_MODE) {
            case PROTON:
              r = i_led;
              g = 0;
              b = 0;
            break;
        
            case SLIME:
              r = 0;
              g = i_led;
              b = 0;
            break;
        
            case STASIS:
              r = 0;
              g = 0;
              b = i_led;
            break;
        
            case MESON:
              r = i_led;
              g = i_led;
              b = 0;
            break;   
        
            default:
              r = i_led;
              g = 0;
              b = 0;
            break;
          }

          pack_leds[i + cyclotron_led_start] = CRGB(r,g,b);
          i_cyclotron_led_value[i] = i_led;
        }

        if(ms_cyclotron_led_fade_in[i].isFinished() && i_cyclotron_led_value[i] > 254 && i_cyclotron_led_on_status[i] == true) {
          switch(FIRING_MODE) {
            case PROTON:
              r = 255;
              g = 0;
              b = 0;
            break;
        
            case SLIME:
              r = 0;
              g = 255;
              b = 0;
            break;
        
            case STASIS:
              r = 0;
              g = 0;
              b = 255;
            break;
        
            case MESON:
              r = 255;
              g = 255;
              b = 0;
            break;   
        
            default:
              r = 255;
              g = 0;
              b = 0;
            break;
          }

          pack_leds[i + cyclotron_led_start] = CRGB(r,g,b);
          i_cyclotron_led_value[i] = 255;
          i_cyclotron_led_on_status[i] = false;

          ms_cyclotron_led_fade_out[i].go(255);
          ms_cyclotron_led_fade_out[i].go(0, i_current_ramp_speed, CIRCULAR_OUT); 
        }

        if(ms_cyclotron_led_fade_out[i].isRunning() && i_cyclotron_led_on_status[i] == false) {
          int i_led = ms_cyclotron_led_fade_out[i].update();
          
          switch(FIRING_MODE) {
            case PROTON:
              r = i_led;
              g = 0;
              b = 0;
            break;
        
            case SLIME:
              r = 0;
              g = i_led;
              b = 0;
            break;
        
            case STASIS:
              r = 0;
              g = 0;
              b = i_led;
            break;
        
            case MESON:
              r = i_led;
              g = i_led;
              b = 0;
            break;   
        
            default:
              r = i_led;
              g = 0;
              b = 0;
            break;
          }

          pack_leds[i + cyclotron_led_start] = CRGB(r,g,b);
          i_cyclotron_led_value[i] = i_led;
        }
      
        if(ms_cyclotron_led_fade_out[i].isFinished() && i_cyclotron_led_on_status[i] == false) {
          pack_leds[i + cyclotron_led_start] = CRGB(0,0,0);
          i_cyclotron_led_value[i] = 0;
          i_cyclotron_led_on_status[i] = true;
        }
      }
    break;

    case 1984:
      if(b_fade_cyclotron_led == true) {
        for(int i = 0; i < PACK_NUM_LEDS - 7 - cyclotron_led_start; i++) {
          if(ms_cyclotron_led_fade_in[i].isRunning()) {
            i_cyclotron_led_on_status[i] = true;
            int i_led = ms_cyclotron_led_fade_in[i].update();

            switch(FIRING_MODE) {
              case PROTON:
                r = i_led;
                g = 0;
                b = 0;
              break;
          
              case SLIME:
                r = 0;
                g = i_led;
                b = 0;
              break;
          
              case STASIS:
                r = 0;
                g = 0;
                b = i_led;
              break;
          
              case MESON:
                r = i_led;
                g = i_led;
                b = 0;
              break;   
          
              default:
                r = i_led;
                g = 0;
                b = 0;
              break;
            }

            pack_leds[i + cyclotron_led_start] = CRGB(r,g,b);
            i_cyclotron_led_value[i] = i_led;
          }

          if(ms_cyclotron_led_fade_in[i].isFinished() && i_cyclotron_led_value[i] > 254 && i_cyclotron_led_on_status[i] == true) {
            switch(FIRING_MODE) {
              case PROTON:
                r = 255;
                g = 0;
                b = 0;
              break;
          
              case SLIME:
                r = 0;
                g = 255;
                b = 0;
              break;
          
              case STASIS:
                r = 0;
                g = 0;
                b = 255;
              break;
          
              case MESON:
                r = 255;
                g = 255;
                b = 0;
              break;   
          
              default:
                r = 255;
                g = 0;
                b = 0;
              break;
            }

            pack_leds[i + cyclotron_led_start] = CRGB(r,g,b);
            i_cyclotron_led_value[i] = 255;
          }

          if(ms_cyclotron_led_fade_out[i].isRunning()) {
            int i_led = ms_cyclotron_led_fade_out[i].update();

            switch(FIRING_MODE) {
            case PROTON:
              r = i_led;
              g = 0;
              b = 0;
            break;
        
            case SLIME:
              r = 0;
              g = i_led;
              b = 0;
            break;
        
            case STASIS:
              r = 0;
              g = 0;
              b = i_led;
            break;
        
            case MESON:
              r = i_led;
              g = i_led;
              b = 0;
            break;   
        
            default:
              r = i_led;
              g = 0;
              b = 0;
            break;
          }

            pack_leds[i + cyclotron_led_start] = CRGB(r,g,b);
            i_cyclotron_led_value[i] = i_led;              
            i_cyclotron_led_on_status[i] = false;
          }
        
          if(ms_cyclotron_led_fade_out[i].isFinished() && i_cyclotron_led_on_status[i] == false) {
            pack_leds[i + cyclotron_led_start] = CRGB(0,0,0);
            i_cyclotron_led_value[i] = 0;
            i_cyclotron_led_on_status[i] = true;
          }
        }
      }
    break;
  }
}

void cyclotron2021(int cDelay) {
  if(ms_cyclotron.justFinished()) {
    if(b_2021_ramp_up == true) {
      if(r_2021_ramp.isFinished()) {
        b_2021_ramp_up = false;
        ms_cyclotron.start(cDelay);
        
        i_current_ramp_speed = cDelay;
      }
      else {     
        ms_cyclotron.start(r_2021_ramp.update());
        i_current_ramp_speed = r_2021_ramp.update();

        if(i_vibration_level > r_2021_ramp.update() - 85) {
          // Do nothing.
        }
        else {
          i_vibration_level = r_2021_ramp.update() - 85;
          
          if(i_vibration_level < 0) {
            i_vibration_level = 0;
          }
        }
        
        vibrationPack(i_vibration_level);
      }
    }
    else if(b_2021_ramp_down == true) {
      if(r_2021_ramp.isFinished()) {
        b_2021_ramp_down = false;
      }
      else {
        ms_cyclotron.start(r_2021_ramp.update());

        i_current_ramp_speed = r_2021_ramp.update();

        if(r_2021_ramp.update() < 100) {
          i_vibration_level = i_vibration_level - 2;
        }
        else {
          i_vibration_level = i_vibration_level - 3;
        }
        
        if(i_vibration_level < 20) {
          i_vibration_level = 0;
        }
        
        vibrationPack(i_vibration_level);
      }
    }
    else {
      i_current_ramp_speed = cDelay;
      
      if(i_cyclotron_multiplier > 1) {
        cDelay = cDelay / i_cyclotron_multiplier + 6;
      }

      ms_cyclotron.start(cDelay);

      if(b_wand_firing != true && b_overheating != true && b_alarm != true) {
        vibrationPack(i_vibration_level);
      }
    }

    if(i_cyclotron_multiplier > 1) {
      cDelay = cDelay / i_cyclotron_multiplier + 6;
    }
    else {
      cDelay = cDelay / i_cyclotron_multiplier;
      
      cDelay = cDelay * 2;
    }

    if(cDelay < 1) {
      cDelay = 1;
    }
    
    if(b_clockwise == true) {
      if(i_cyclotron_led_value[i_led_cyclotron - cyclotron_led_start] == 0) {
        ms_cyclotron_led_fade_in[i_led_cyclotron - cyclotron_led_start].go(0);
        ms_cyclotron_led_fade_in[i_led_cyclotron - cyclotron_led_start].go(255, cDelay, CIRCULAR_IN);
      }

      i_led_cyclotron++;
        
      if(i_led_cyclotron > PACK_NUM_LEDS - 7 - 1) {
        i_led_cyclotron = cyclotron_led_start;
      }   
    }
    else {
      if(i_cyclotron_led_value[i_led_cyclotron - cyclotron_led_start] == 0) {
       ms_cyclotron_led_fade_in[i_led_cyclotron - cyclotron_led_start].go(0);
        ms_cyclotron_led_fade_in[i_led_cyclotron - cyclotron_led_start].go(255, cDelay, CIRCULAR_IN);
      }

      i_led_cyclotron--;
      
      if(i_led_cyclotron < cyclotron_led_start) {
        i_led_cyclotron = PACK_NUM_LEDS - 7 - 1;
      }
    }    
  }
}

void cyclotron1984(int cDelay) {
  int i_pack_vibration = 100;
  
  if(ms_cyclotron.justFinished()) {    
    cDelay = cDelay / i_cyclotron_multiplier;
    
    if(b_1984_led_start != true) {
      cyclotron84LightOff(i_led_cyclotron);
    }

    if(b_1984_led_start == true) {
      b_1984_led_start = false;
    }

    if(b_clockwise == true) {
      i_1984_counter++;
    }
    else {
      i_1984_counter--;
    }

    if(i_1984_counter > 3) {
      i_1984_counter = 0;
    }
    else if(i_1984_counter < 0) {
      i_1984_counter = 3;
    }

    i_led_cyclotron = cyclotron_led_start + i_1984_cyclotron_leds[i_1984_counter] - 2;
    
    cyclotron84LightOn(i_led_cyclotron);

    if(b_2021_ramp_up == true) {
      if(r_2021_ramp.isFinished()) {
        b_2021_ramp_up = false;
        ms_cyclotron.start(cDelay);
        
        i_current_ramp_speed = cDelay;
      }
      else {       
        ms_cyclotron.start(r_2021_ramp.update());
        i_current_ramp_speed = r_2021_ramp.update();        
      }
    }
    else if(b_2021_ramp_down == true) {
      if(r_2021_ramp.isFinished()) {
        b_2021_ramp_down = false;
        i_pack_vibration = 0;
      }
      else {
        ms_cyclotron.start(r_2021_ramp.update());
        
        i_current_ramp_speed = r_2021_ramp.update();

        i_pack_vibration = r_2021_ramp.update() / 13;
      }
    }
    else {
      ms_cyclotron.start(cDelay);
    } 

    if(b_wand_firing != true && b_overheating != true && b_alarm != true) {
      vibrationPack(i_pack_vibration);
    }
  }
}

void cyclotron1984Alarm() {
  int led1 = cyclotron_led_start + i_1984_cyclotron_leds[0] - 1;
  int led2 = cyclotron_led_start + i_1984_cyclotron_leds[1] - 1;
  int led3 = cyclotron_led_start + i_1984_cyclotron_leds[2] - 1;
  int led4 = cyclotron_led_start + i_1984_cyclotron_leds[3] - 1;

  if(b_fade_cyclotron_led != true) {
    pack_leds[led1] = CRGB(255,0,0);
    pack_leds[led2] = CRGB(255,0,0);
    pack_leds[led3] = CRGB(255,0,0);
    pack_leds[led4] = CRGB(255,0,0);
  }
  else {
    if(i_cyclotron_led_value[led1 - cyclotron_led_start] == 0) {
      ms_cyclotron_led_fade_in[led1 - cyclotron_led_start].go(0);
      ms_cyclotron_led_fade_in[led1 - cyclotron_led_start].go(255, i_1984_fade_in_delay, CIRCULAR_IN);
    }
    
    if(i_cyclotron_led_value[led2 - cyclotron_led_start] == 0) {      
      ms_cyclotron_led_fade_in[led2 - cyclotron_led_start].go(0);
      ms_cyclotron_led_fade_in[led2 - cyclotron_led_start].go(255, i_1984_fade_in_delay, CIRCULAR_IN);
    }

    if(i_cyclotron_led_value[led3 - cyclotron_led_start] == 0) {
      ms_cyclotron_led_fade_in[led3 - cyclotron_led_start].go(0);
      ms_cyclotron_led_fade_in[led3 - cyclotron_led_start].go(255, i_1984_fade_in_delay, CIRCULAR_IN);      
    }

    if(i_cyclotron_led_value[led4 - cyclotron_led_start] == 0) {      
      ms_cyclotron_led_fade_in[led4 - cyclotron_led_start].go(0);
      ms_cyclotron_led_fade_in[led4 - cyclotron_led_start].go(255, i_1984_fade_in_delay, CIRCULAR_IN);      
    }
  }
}

void cyclotron84LightOn(int cLed) {
  if(b_fade_cyclotron_led != true) {
    pack_leds[cLed+1] = CRGB(255,0,0);
  }
  else {
    if(i_cyclotron_led_value[cLed + 1 - cyclotron_led_start] == 0) {
      ms_cyclotron_led_fade_in[cLed + 1 - cyclotron_led_start].go(0);
      ms_cyclotron_led_fade_in[cLed + 1 - cyclotron_led_start].go(255, i_1984_fade_in_delay / i_cyclotron_multiplier, CIRCULAR_IN);    
    }
  }
}

void cyclotron84LightOff(int cLed) {
  if(b_fade_cyclotron_led != true) {
    pack_leds[cLed+1] = CRGB(0,0,0);
  }
  else {
    if(i_cyclotron_led_value[cLed + 1 - cyclotron_led_start] == 255) {
      ms_cyclotron_led_fade_out[cLed + 1 - cyclotron_led_start].go(255);
      ms_cyclotron_led_fade_out[cLed + 1 - cyclotron_led_start].go(0, i_1984_fade_out_delay / i_cyclotron_multiplier, CIRCULAR_OUT);
    }
  }
}

void cyclotronOverHeating() {  
  vibrationPack(50);
  smokeControl(true);

  if(ms_overheating.justFinished()) {
    w_trig.trackGain(S_VENT_SMOKE, i_volume);
    w_trig.trackPlayPoly(S_VENT_SMOKE, true);
    smokeControl(false);
  }

  switch (i_mode_year) {
    case 2021:
      cyclotron2021(i_2021_delay * 10);
      innerCyclotronRing(i_2021_inner_delay * 14);
    break;

    case 1984:
      innerCyclotronRing(i_2021_inner_delay * 14);

      if(ms_alarm.justFinished()) {
        ms_alarm.start(i_1984_delay / 2);

        if(b_fade_cyclotron_led != true) {
          resetCyclotronLeds();
        }
        else {
          cyclotron84LightOff(i_1984_cyclotron_leds[0] + cyclotron_led_start - 2);
          cyclotron84LightOff(i_1984_cyclotron_leds[1] + cyclotron_led_start - 2);
          cyclotron84LightOff(i_1984_cyclotron_leds[2] + cyclotron_led_start - 2);
          cyclotron84LightOff(i_1984_cyclotron_leds[3] + cyclotron_led_start - 2);
        }
      }
      else {
        if(ms_alarm.remaining() < i_1984_delay / 4) {
          cyclotron1984Alarm();
        }
      }
    break;
  }

  // Time the n-light to when the fan is running.
  if(ms_fan_stop_timer.isRunning() && ms_fan_stop_timer.remaining() < 3000) {    
    // For strobing the vent light.
    if(ms_vent_light_off.justFinished()) {
      ms_vent_light_off.stop();
      ms_vent_light_on.start(i_vent_light_delay);
  
      if(b_overheat_strobe == true) {
        ventLight(true);
      }
    }
    else if(ms_vent_light_on.justFinished()) {
      ms_vent_light_on.stop();
      ms_vent_light_off.start(i_vent_light_delay);
  
      if(b_overheat_strobe == true) {
        ventLight(false);
      }
    }
  
    // For non strobing vent light option.
    if(b_overheat_strobe != true) {
      if(b_vent_light_on != true) {
        // Solid light on if strobe option turned off.
        ventLight(true);
      }
    }
  }
}
      
void cyclotronNoCable() {  
  vibrationPack(50);

  switch (i_mode_year) {
    case 2021:
      cyclotron2021(i_2021_delay * 10);      
      innerCyclotronRing(i_2021_inner_delay * 14);

      if(ms_alarm.justFinished()) {
        ventLight(false);
        ms_alarm.start(i_1984_delay);
      }
      else {
        if(ms_alarm.remaining() < i_1984_delay / 2) {
          ventLight(true);
        }
      }
    break;

    case 1984:
      innerCyclotronRing(i_2021_inner_delay * 14);

      if(ms_alarm.justFinished()) {
        ms_alarm.start(i_1984_delay / 2);

        if(b_fade_cyclotron_led != true) {
          resetCyclotronLeds();
        }
        else {
          cyclotron84LightOff(i_1984_cyclotron_leds[0] + cyclotron_led_start - 2);
          cyclotron84LightOff(i_1984_cyclotron_leds[1] + cyclotron_led_start - 2);
          cyclotron84LightOff(i_1984_cyclotron_leds[2] + cyclotron_led_start - 2);
          cyclotron84LightOff(i_1984_cyclotron_leds[3] + cyclotron_led_start - 2);
        }

        // Turn off the n-filter light.
        ventLight(false);
      }
      else {
        if(ms_alarm.remaining() < i_1984_delay / 4) {
          cyclotron1984Alarm();
          
          // Turn off the n-filter light.
          ventLight(true);
        }
      }
    break;
  }
}

void resetCyclotronLeds() {
  for(int i = cyclotron_led_start; i < PACK_NUM_LEDS; i++) {
    pack_leds[i] = CRGB(0,0,0);
  }

  for(int i = 0; i < PACK_NUM_LEDS - 7 - cyclotron_led_start; i++) {
      ms_cyclotron_led_fade_out[i].go(0);
      ms_cyclotron_led_fade_in[i].go(0);

      i_cyclotron_led_on_status[i] = false;
  }
  
  // Only reset the start led if the pack is off or just started.
  if(b_reset_start_led == true) {
    i_led_cyclotron = cyclotron_led_start;
  }

  // Keep the fade control fading out a light that is not on during startup.
  if(PACK_STATUS == MODE_OFF){ 
    if(b_1984_led_start != true) {
      b_1984_led_start = true;
    }
  }

  // Tell the inner cyclotron to turn off the leds.
  if(b_cyclotron_lid_on == true) {
    innerCyclotronOff();
  }
  else if(b_alarm != true || PACK_STATUS == MODE_OFF) {
    innerCyclotronOff();
  }

  cyclotronSpeedRevert();
}

void innerCyclotronOff() {
  for(int i = 0; i < CYCLOTRON_NUM_LEDS; i++) {
    cyclotron_leds[i] = CRGB(0,0,0);
  }
}

void innerCyclotronShowAll() {
  if(b_cyclotron_lid_on != true) {
    for(int i = 0; i < CYCLOTRON_NUM_LEDS; i++) {
      cyclotron_leds[i] = CRGB(255,0,0);
    }
  }
}

// For NeoPixel rings, ramp up and ramp down the LEDs in the ring and set the speed. (optional)
void innerCyclotronRing(int cDelay) {
  if(ms_cyclotron_ring.justFinished()) {
    if(b_inner_ramp_up == true) {      
      if(r_inner_ramp.isFinished()) {
        b_inner_ramp_up = false;
        ms_cyclotron_ring.start(cDelay);
        
        i_inner_current_ramp_speed = cDelay;
      }
      else {     
        ms_cyclotron_ring.start(r_inner_ramp.update());
        i_inner_current_ramp_speed = r_inner_ramp.update();
      }
    }
    else if(b_inner_ramp_down == true) {
      if(r_inner_ramp.isFinished()) {
        b_inner_ramp_down = false;
      }
      else {
        ms_cyclotron_ring.start(r_inner_ramp.update());

        i_inner_current_ramp_speed = r_inner_ramp.update();
      }
    }
    else {
      i_inner_current_ramp_speed = cDelay;

      if(i_cyclotron_multiplier > 1) {
        if(i_cyclotron_multiplier > 4) {
          cDelay = cDelay - 4;
        }
        else {
          cDelay = cDelay - i_cyclotron_multiplier;
        }
      }

      if(cDelay < 1) {
        cDelay = 1;
      }

      ms_cyclotron_ring.start(cDelay);
    }  

    // Colour control for the inner cyclotron leds. (red,green,blue)
    int r = 0;
    int g = 0;
    int b = 0;
  
    switch(FIRING_MODE) {
      case PROTON:
        r = 255;
        g = 0;
        b = 0;
      break;
  
      case SLIME:
        r = 0;
        g = 255;
        b = 0;
      break;
  
      case STASIS:
        r = 0;
        g = 0;
        b = 255;
      break;
  
      case MESON:
        r = 255;
        g = 255;
        b = 0;
      break;   
  
      default:
        r = 255;
        g = 0;
        b = 0;
      break;
    }
  
    if(i_cyclotron_multiplier > 1) {
      switch(i_cyclotron_multiplier) {
        case 6:
          cDelay = cDelay - 4;
        break;
        
        case 5:
          cDelay = cDelay - 3;
        break;
        
        case 4:
          cDelay = cDelay - 3;
        break;
        
        case 3:
          cDelay = cDelay - 2;
        break;
        
        case 2:
          cDelay = cDelay - 2;
        break;
          
        default:
          cDelay = cDelay - 1;
        break;
      }
    }
    else {
      cDelay = cDelay / i_cyclotron_multiplier;
    }

    if(cDelay < 2) {
      cDelay = 2;
    }
    
    if(b_clockwise == true) {
      if(b_cyclotron_lid_on != true) {
        if(CYCLOTRON_NUM_LEDS == 35) {
          cyclotron_leds[i_led_cyclotron_ring] = CRGB(r,g,b);
  
          if(i_led_cyclotron_ring == 0) {
            cyclotron_leds[CYCLOTRON_NUM_LEDS - 1] = CRGB(0,0,0);
          }
          else {
            cyclotron_leds[i_led_cyclotron_ring - 1] = CRGB(0,0,0);
          }
        }
      }

      i_led_cyclotron_ring++;
      
      if(i_led_cyclotron_ring > CYCLOTRON_NUM_LEDS - 1) {
        i_led_cyclotron_ring = 0;
      }
    }
    else {
      if(b_cyclotron_lid_on != true) {
        if(CYCLOTRON_NUM_LEDS == 35) {
          cyclotron_leds[i_led_cyclotron_ring] = CRGB(r,g,b);
          
          if(i_led_cyclotron_ring + 1 > CYCLOTRON_NUM_LEDS - 1) {
            cyclotron_leds[0] = CRGB(0,0,0);
          }
          else {
            cyclotron_leds[i_led_cyclotron_ring + 1] = CRGB(0,0,0);
          }
        }
      }
      
      i_led_cyclotron_ring--;

      if(i_led_cyclotron_ring < 0) {
        i_led_cyclotron_ring = CYCLOTRON_NUM_LEDS -1;
      }
    }
  }
}

void reset2021RampUp() {
  b_2021_ramp_up = true;
  b_2021_ramp_up_start = true;

  // Inner cyclotron ring.
  b_inner_ramp_up = true;
}

void reset2021RampDown() {
  b_2021_ramp_down = true;
  b_2021_ramp_down_start = true;

  // Inner cyclotron ring.
  b_inner_ramp_down = true;
}

void ventLight(boolean b_on) {
  b_vent_light_on = b_on;

  if(b_on == true) {
    int r = 255;
    int g = 255;
    int b = 255;

    // If doing firing smoke effects, lets change the light colours.
    if(b_wand_firing == true) {
      switch(FIRING_MODE) {
        case PROTON:
          // Adjust the n-filter light colours during firing.
          switch(i_wand_power_level) {
            case 1:
              r = 200;
              g = 255;
              b = 255;
            break;

            case 2:
              r = 150;
              g = 255;
              b = 255;
            break;

            case 3:
              r = 100;
              g = 255;
              b = 230;
            break;

            case 4:
              r = 50;
              g = 255;
              b = 255;
            break;

            case 5:
              r = 0;
              g = 255;
              b = 255;
            break;

            default:
              r = 0;
              g = 255;
              b = 255;
            break;
          }
        break;
        
        case SLIME:
          r = 0;
          g = 255;
          b = 0;
        break;
        
        case STASIS:
          r = 0;
          g = 0;
          b = 255;
        break;
        
        case MESON:
          r = 255;
          g = 255;
          b = 0;
        break;   
        
        default:
          r = 50;
          g = 255;
          b = 255;
        break;
      }
    }
    else if(b_alarm == true && b_overheating != true) {
      r = 255;
      g = 0;
      b = 0;
    }
    
    for(int i = VENT_LIGHT_START; i < PACK_NUM_LEDS; i++) {
      pack_leds[i] = CRGB(r,g,b);
    }
  }
  else {
    for(int i = VENT_LIGHT_START; i < PACK_NUM_LEDS; i++) {
      pack_leds[i] = CRGB(0,0,0);
    }
  }
}

void stopMusic() {
  w_trig.trackStop(i_current_music_track);

  w_trig.update();
}

void playMusic() {
  w_trig.trackGain(i_current_music_track, i_volume_music);
  w_trig.trackPlayPoly(i_current_music_track);

  if(b_repeat_track == true) {
    w_trig.trackLoop(i_current_music_track, 1);
  }
  else {
    w_trig.trackLoop(i_current_music_track, 0);
  }

  w_trig.update();
}

void wandFiring() {
  b_wand_firing = true;

  // Turn off any smoke.
  smokeControl(false);

  // Start a smoke timer to play a little bit of smoke while firing.
  ms_smoke_timer.start(i_smoke_timer[i_wand_power_level - 1]);
  ms_smoke_on.stop();       
  
  vibrationPack(255);

  w_trig.trackGain(S_FIRE_START_SPARK, i_volume);
  w_trig.trackPlayPoly(S_FIRE_START_SPARK);

  switch(FIRING_MODE) {
    case PROTON:
      w_trig.trackGain(S_FIRE_START, i_volume);
      w_trig.trackPlayPoly(S_FIRE_START, true);
    
      w_trig.trackGain(S_FIRE_LOOP_GUN, i_volume);
      w_trig.trackPlayPoly(S_FIRE_LOOP_GUN, true);
      w_trig.trackFade(S_FIRE_LOOP_GUN, i_volume, 1000, 0);
      w_trig.trackLoop(S_FIRE_LOOP_GUN, 1);
    
      w_trig.trackGain(S_FIRE_LOOP, i_volume);
      w_trig.trackPlayPoly(S_FIRE_LOOP, true);
      w_trig.trackFade(S_FIRE_LOOP, i_volume, 1000, 0);
      w_trig.trackLoop(S_FIRE_LOOP, 1); 
    break;

    case SLIME:
      w_trig.trackGain(S_SLIME_START, i_volume);
      w_trig.trackPlayPoly(S_SLIME_START);
      
      w_trig.trackGain(S_SLIME_LOOP, i_volume);
      w_trig.trackPlayPoly(S_SLIME_LOOP);
      w_trig.trackFade(S_SLIME_LOOP, i_volume, 1500, 0);
      w_trig.trackLoop(S_SLIME_LOOP, 1);
    break;

    case STASIS:
      w_trig.trackGain(S_STASIS_START, i_volume);
      w_trig.trackPlayPoly(S_STASIS_START);
      
      w_trig.trackGain(S_STASIS_LOOP, i_volume);
      w_trig.trackPlayPoly(S_STASIS_LOOP);
      w_trig.trackFade(S_STASIS_LOOP, i_volume, 1000, 0);
      w_trig.trackLoop(S_STASIS_LOOP, 1);
    break;

    case MESON:
      w_trig.trackGain(S_MESON_START, i_volume);
      w_trig.trackPlayPoly(S_MESON_START);
      
      w_trig.trackGain(S_MESON_LOOP, i_volume);
      w_trig.trackPlayPoly(S_MESON_LOOP);
      w_trig.trackFade(S_MESON_LOOP, i_volume, 5500, 0);
      w_trig.trackLoop(S_MESON_LOOP, 1);
    break;

    case SETTINGS:
      // Nothing.
    break;
  }

  // Reset some vent light timers.
  ms_vent_light_off.stop();
  ms_vent_light_on.stop();
  ms_vent_light_off.start(i_vent_light_delay);

  // Reset vent sounds flag.
  b_vent_sounds = true;

  ms_firing_length_timer.start(i_firing_timer_length);
}

void wandStoppedFiring() {
  // Stop all other firing sounds.
  wandStopFiringSounds();      
  
  if(b_wand_firing == true) {
    switch(FIRING_MODE) {
      case PROTON:
        // Play different firing end stream sound depending on how long we have been firing for.
        if(ms_firing_length_timer.remaining() < 5000) {
          // Long tail end.
          w_trig.trackGain(S_FIRING_END, i_volume);
          w_trig.trackPlayPoly(S_FIRING_END, true);
        }
        else if(ms_firing_length_timer.remaining() < 10000) {
          // Mid tail end.
          w_trig.trackGain(S_FIRING_END_MID, i_volume);
          w_trig.trackPlayPoly(S_FIRING_END_MID, true);
        }
        else {
          // Short tail end.
          w_trig.trackGain(S_FIRING_END_GUN, i_volume);
          w_trig.trackPlayPoly(S_FIRING_END_GUN, true);
        }
      break;
  
      case SLIME:
        w_trig.trackGain(S_SLIME_END, i_volume);
        w_trig.trackPlayPoly(S_SLIME_END, true);
      break;
  
      case STASIS:
        w_trig.trackGain(S_STASIS_END, i_volume);
        w_trig.trackPlayPoly(S_STASIS_END, true);
        
      break;
  
      case MESON:
        w_trig.trackGain(S_MESON_END, i_volume);
        w_trig.trackPlayPoly(S_MESON_END, true);
      break;

      case SETTINGS:
        // Nothing
      break;
    }
  }
  
  b_wand_firing = false;

  // Reset some vent light timers.
  ms_vent_light_off.stop();
  ms_vent_light_on.stop();
  ventLight(false);

  // Reset vent sounds flag.
  b_vent_sounds = true;

  // Turn off any smoke.
  smokeControl(false);
  
  // Turn off the n-filter fan.
  fanControl(false); 

  ms_firing_length_timer.stop();
  ms_smoke_timer.stop();
  ms_smoke_on.stop();
}

void wandStopFiringSounds() {
  // Firing sounds.
  w_trig.trackStop(S_FIRE_START);
  w_trig.trackStop(S_FIRE_START_SPARK);
  w_trig.trackStop(S_FIRE_LOOP);
  w_trig.trackStop(S_FIRE_LOOP_GUN);
  w_trig.trackStop(S_FIRE_LOOP_IMPACT);
  w_trig.trackStop(S_SLIME_START);
  w_trig.trackStop(S_SLIME_LOOP);
  w_trig.trackStop(S_SLIME_END);
  w_trig.trackStop(S_STASIS_START);
  w_trig.trackStop(S_STASIS_LOOP);
  w_trig.trackStop(S_STASIS_END);
  w_trig.trackStop(S_MESON_START);
  w_trig.trackStop(S_MESON_LOOP);
  w_trig.trackStop(S_MESON_END);
}

void packAlarm() {
  wandStopFiringSounds();

  // Pack sounds.
  if(b_gb2_mode == true && i_mode_year == 1984) {
    w_trig.trackStop(S_GB2_PACK_START);
    w_trig.trackStop(S_GB2_PACK_LOOP);
  }
  else {
    w_trig.trackStop(S_AFTERLIFE_PACK_STARTUP);
    w_trig.trackStop(S_AFTERLIFE_PACK_IDLE_LOOP);
    w_trig.trackStop(S_IDLE_LOOP);
    w_trig.trackStop(S_BOOTUP);
  }

  w_trig.trackGain(S_SHUTDOWN, i_volume);
  w_trig.trackPlayPoly(S_SHUTDOWN, true);

  if(b_gb2_mode == true && i_mode_year == 1984) {
    w_trig.trackGain(S_GB2_PACK_OFF, i_volume);
    w_trig.trackPlayPoly(S_GB2_PACK_OFF, true);
  }
  else {
    w_trig.trackGain(S_PACK_SHUTDOWN, i_volume);
    w_trig.trackPlayPoly(S_PACK_SHUTDOWN, true);
  }

  if(b_overheating != true) {
    switch(i_mode_year) {
      case 1984:
        w_trig.trackGain(S_PACK_RIBBON_ALARM_1, i_volume);
        w_trig.trackPlayPoly(S_PACK_RIBBON_ALARM_1, true);
        w_trig.trackLoop(S_PACK_RIBBON_ALARM_1, 1);
      break;

      default:
        w_trig.trackGain(S_PACK_RIBBON_ALARM_2, i_volume);
        w_trig.trackPlayPoly(S_PACK_RIBBON_ALARM_2, true);
        w_trig.trackLoop(S_PACK_RIBBON_ALARM_2, 1);
      break;
    }
  }
}

// LED's for the 1984/2021 and vibration switches.
void cyclotronSwitchPlateLEDs() {   
  if(switch_cyclotron_lid.isReleased()) {
    // Play sounds when lid is removed.
    w_trig.trackStop(S_VENT_SMOKE);
    w_trig.trackStop(S_MODE_SWITCH);
    w_trig.trackStop(S_CLICK);
    w_trig.trackStop(S_SPARKS_LOOP);
    w_trig.trackStop(S_BEEPS_BARGRAPH);

    w_trig.trackGain(S_MODE_SWITCH, i_volume);
    w_trig.trackPlayPoly(S_MODE_SWITCH);
    
    w_trig.trackGain(S_VENT_SMOKE, i_volume);
    w_trig.trackPlayPoly(S_VENT_SMOKE);

    // Play some spark sounds if the pack is running and the lid is removed.
    if(PACK_STATUS == MODE_ON) {
      w_trig.trackGain(S_SPARKS_LOOP, i_volume);
      w_trig.trackPlayPoly(S_SPARKS_LOOP);
    }
  }

  if(switch_cyclotron_lid.isPressed()) {
    // Play sounds when lid is mounted.
    w_trig.trackStop(S_CLICK);    
    w_trig.trackStop(S_VENT_DRY);

    w_trig.trackGain(S_CLICK, i_volume);
    w_trig.trackPlayPoly(S_CLICK);

    w_trig.trackGain(S_VENT_DRY, i_volume);
    w_trig.trackPlayPoly(S_VENT_DRY);

    // Play some spark sounds if the pack is running and the lid is put back on                          .
    if(PACK_STATUS == MODE_ON) {
      w_trig.trackGain(S_SPARKS_LOOP, i_volume);
      w_trig.trackPlayPoly(S_SPARKS_LOOP);
    }
  }

  if(switch_cyclotron_lid.getState() == LOW) {
    if(b_cyclotron_lid_on != true) {
      // The cyclotron lid is now on.
      b_cyclotron_lid_on = true;
      
      // Turn off inner cyclotron LEDs.
      innerCyclotronOff();
    }
  }
  else {
    if(b_cyclotron_lid_on == true) {
      // The cyclotron lid is now off.
      b_cyclotron_lid_on = false;
    }
  }

  if(b_cyclotron_lid_on != true) {
    if(i_mode_year == 1984) {        
      if(ms_cyclotron_switch_plate_leds.remaining() < i_cyclotron_switch_plate_leds_delay / 2) {
        digitalWrite(cyclotron_switch_led_green, HIGH);
      }
      else {
        digitalWrite(cyclotron_switch_led_green, LOW);
      }
    }
    else {
      digitalWrite(cyclotron_switch_led_green, HIGH);
    }
    
    if(b_vibration == true) {
      if(ms_cyclotron_switch_plate_leds.remaining() < i_cyclotron_switch_plate_leds_delay / 2) {
        digitalWrite(cyclotron_switch_led_yellow, HIGH);
      }
      else {
        digitalWrite(cyclotron_switch_led_yellow, LOW);
      }
    }
    else {
      digitalWrite(cyclotron_switch_led_yellow, HIGH);
    }  
  }
  else {
    // Keep the cyclotron switch LED's off when the lid is on.
    digitalWrite(cyclotron_switch_led_green, LOW);
    digitalWrite(cyclotron_switch_led_yellow, LOW);
  }
  
  if(ms_cyclotron_switch_plate_leds.justFinished()) {
    ms_cyclotron_switch_plate_leds.start(i_cyclotron_switch_plate_leds_delay);
  }
}

void vibrationPack(int i_level) {
  if(b_vibration == true) {
    if(i_level != i_vibration_level_prev) {
      i_vibration_level_prev = i_level;
      analogWrite(vibration, i_level);
    }
  }
  else {
    analogWrite(vibration, 0);
  }
}

void cyclotronSpeedRevert() {
  // Stop overheat beeps.
  w_trig.trackStop(S_BEEP_8);
  
  i_cyclotron_multiplier = 1;
  i_cyclotron_switch_led_mulitplier = 1;
}

void cyclotronSpeedIncrease() {
  switch(i_mode_year) {
    case 2021:
      i_cyclotron_multiplier++;
    break;
    
    case 1984:
      i_cyclotron_multiplier++;
    break;
  }

  i_cyclotron_switch_led_mulitplier++;
}

void adjustVolumeEffectsGain() {
  /*
   * If the pack is running, there will be a slight pause in the LEDs due to serial communication to the wav trigger.
   */
  for(int i=0; i <= i_last_effects_track; i++) {
    w_trig.trackGain(i, i_volume);
  }
}

void increaseVolumeEffects() {
  if(i_volume_percentage + VOLUME_EFFECTS_MULTIPLIER > 100) {
    i_volume_percentage = 100;
  }
  else {
    i_volume_percentage = i_volume_percentage + VOLUME_EFFECTS_MULTIPLIER;
  }

  i_volume = MINIMUM_VOLUME - (MINIMUM_VOLUME * i_volume_percentage / 100);

  adjustVolumeEffectsGain();
}

void decreaseVolumeEffects() {
  if(i_volume_percentage - VOLUME_EFFECTS_MULTIPLIER < 0) {
    i_volume_percentage = 0;
  }
  else {
    i_volume_percentage = i_volume_percentage - VOLUME_EFFECTS_MULTIPLIER;
  }

  i_volume = MINIMUM_VOLUME - (MINIMUM_VOLUME * i_volume_percentage / 100);

  adjustVolumeEffectsGain();
}

void increaseVolume() { 
  if(i_volume_master_percentage + VOLUME_MULTIPLIER > 100) {
    i_volume_master_percentage = 100;
  }
  else {
    i_volume_master_percentage = i_volume_master_percentage + VOLUME_MULTIPLIER;
  }

  i_volume_master = MINIMUM_VOLUME - (MINIMUM_VOLUME * i_volume_master_percentage / 100);

  w_trig.masterGain(i_volume_master);
}

void decreaseVolume() {
  if(i_volume_master_percentage - VOLUME_MULTIPLIER < 0) {
    i_volume_master_percentage = 0;
  }
  else {
    i_volume_master_percentage = i_volume_master_percentage - VOLUME_MULTIPLIER;
  }

  i_volume_master = MINIMUM_VOLUME - (MINIMUM_VOLUME * i_volume_master_percentage / 100);
     
  w_trig.masterGain(i_volume_master);       
}

void readEncoder() {
  if (digitalRead(encoder_pin_a) == digitalRead(encoder_pin_b)) {
    i_encoder_pos++;
  }
  else {
    i_encoder_pos--;
  }
  
  i_val_rotary = i_encoder_pos / 2.5;
}

void checkRotaryEncoder() {
  if(i_val_rotary > i_last_val_rotary) {
    if(ms_volume_check.isRunning() != true) {
      increaseVolume();
      
      // Tell wand to increase volume.
      Serial2.write(9);

      ms_volume_check.start(50);
    }
  }
  
  if(i_val_rotary < i_last_val_rotary) {
    if(ms_volume_check.isRunning() != true) {
      decreaseVolume();
        
      // Tell wand to decrease the volume.
      Serial2.write(10);

      ms_volume_check.start(50);
    }
  }
  
  i_last_val_rotary = i_val_rotary;

  if(ms_volume_check.justFinished()) {
    ms_volume_check.stop();
  }
}

/*
 * Smoke # 1. I put this one in my n-filter cone outlet.
 */
void smokeControl(boolean b_smoke_on) {  
  if(b_smoke_enabled == true) {
    if(b_smoke_on == true) {
      if(b_wand_firing == true && b_overheating != true && b_smoke_1_continuous_firing == true && b_smoke_continuous_mode[i_wand_power_level - 1] == true) {
        digitalWrite(smoke_pin, HIGH);
      }
      else if(b_overheating == true && b_wand_firing != true && b_smoke_1_overheat == true && b_smoke_overheat_mode[i_wand_power_level - 1] == true) {
        digitalWrite(smoke_pin, HIGH);
      }
      else {
        digitalWrite(smoke_pin, LOW);
      }
    }
    else {
      digitalWrite(smoke_pin, LOW);
    }

    smokeBooster(b_smoke_on);
  }
}

/* 
 *  Smoke # 2. I put this one in my booster tube.
 */
void smokeBooster(boolean b_smoke_on) {
  if(b_smoke_enabled == true) {
    if(b_smoke_on == true) {
      if(b_wand_firing == true && b_overheating != true && b_smoke_2_continuous_firing == true && b_smoke_continuous_mode[i_wand_power_level - 1] == true) {
        digitalWrite(smoke_booster_pin, HIGH);
      }
      else if(b_overheating == true && b_smoke_2_overheat == true && b_wand_firing != true && b_smoke_overheat_mode[i_wand_power_level - 1] == true) {
        digitalWrite(smoke_booster_pin, HIGH);
      }
      else {
        digitalWrite(smoke_booster_pin, LOW);
      }
    }
    else {
      digitalWrite(smoke_booster_pin, LOW);
    }
  }
}
 
/*
 * Fan control. You can use this to switch on any device when properly hooked up with a transistor etc
 * A fan is a good idea for the n-filter for example.
 */
void fanControl(boolean b_fan_on) {
  if(b_smoke_enabled == true) {
    if(b_fan_on == true) {
      if(b_wand_firing == true && b_overheating != true && b_fan_continuous_firing == true && b_smoke_continuous_mode[i_wand_power_level - 1] == true) {
        digitalWrite(fan_pin, HIGH);
      }
      else if(b_overheating == true && b_wand_firing != true && b_fan_overheat == true && b_smoke_overheat_mode[i_wand_power_level - 1] == true) {
        digitalWrite(fan_pin, HIGH);
      }
      else {
        digitalWrite(fan_pin, LOW);
      }
    }
    else {
      digitalWrite(fan_pin, LOW);
    }
  }
}

/* 
 *  Another optional 5V pin that goes high during overheat sequences. 
 *  Perhaps this one would be good for a fan or dc motor to push some smoke into the n-filter with more force.
 */
void checkFan() {
  if(ms_fan_stop_timer.justFinished()) {
    // Turn off fan.
    fanControl(false);
    ms_fan_stop_timer.stop();
  }
  else if(ms_fan_stop_timer.isRunning() && ms_fan_stop_timer.remaining() < 3000) {
    fanControl(true);
  }
}

/*
 * Check if the wand is still connected.
 */
void wandHandShake() {  
  if(b_wand_connected == true) {    
    if(ms_wand_handshake.justFinished()) {     
          
      if(b_wand_firing == true) {
        wandStoppedFiring();
        cyclotronSpeedRevert();
      }
      
      ms_wand_handshake.start(i_wand_handshake_delay);
      
      b_wand_connected = false;

      // Where are you wand?
      Serial2.write(11);
    }
    else if(ms_wand_handshake_checking.justFinished()) {  
      if(b_diagnostic == true) {
        // Play a beep sound to know if the wand is connected, while in diagnostic mode.
        w_trig.trackGain(S_VENT_BEEP, i_volume);
        w_trig.trackPlayPoly(S_VENT_BEEP);
      }
      
      ms_wand_handshake_checking.stop();
      
      // Ask the wand if it is still connected.
      Serial2.write(11);
    }
  }
  else {
    if(b_wand_firing == true) {
      wandStoppedFiring();
      cyclotronSpeedRevert();
    }

    if(ms_wand_handshake.justFinished()) {
      // Ask the wand if it is connected.
      Serial2.write(11);

      ms_wand_handshake.start(i_wand_handshake_delay / 5);
    }
  }
}

/*
 * Incoming messages from the wand.
 */
void checkWand() {  
  if(Serial2.available() > 0) {
    prev_byte = rx_byte;  
    rx_byte = Serial2.read();

    if(b_wand_connected == true) {
      switch(rx_byte) {
        case 1:
          // The wand has been turned on.
          // Turn the pack on.
          if(PACK_STATUS != MODE_ON) {
            PACK_ACTION_STATUS = ACTION_ACTIVATE;
          }
        break;
    
        case 2:
          // The wand has been turned off.
          // Turn the pack off.
          if(PACK_STATUS != MODE_OFF) {
            PACK_ACTION_STATUS = ACTION_OFF;
          }
        break;
    
        case 3:
          // Wand is firing.
          wandFiring();
        break;
    
        case 4:
          // Wand just stopped firing.
          wandStoppedFiring();
          cyclotronSpeedRevert();
        break;
    
        case 5:
          // Proton mode
          FIRING_MODE = PROTON;
          w_trig.trackGain(S_CLICK, i_volume);
          w_trig.trackPlayPoly(S_CLICK);
          
          if(PACK_STATUS == MODE_ON) {
            w_trig.trackGain(S_FIRE_START_SPARK, i_volume);
            w_trig.trackPlayPoly(S_FIRE_START_SPARK);
          }
        break;
    
        case 6:
          // Slime mode
          FIRING_MODE = SLIME;
          w_trig.trackGain(S_CLICK, i_volume);
          w_trig.trackPlayPoly(S_CLICK);
          
          if(PACK_STATUS == MODE_ON) {
            w_trig.trackGain(S_PACK_SLIME_OPEN, i_volume);
            w_trig.trackPlayPoly(S_PACK_SLIME_OPEN);
          }
        break;
    
        case 7:
          // Stasis mode
          FIRING_MODE = STASIS;
          w_trig.trackGain(S_CLICK, i_volume);
          w_trig.trackPlayPoly(S_CLICK);
          
          if(PACK_STATUS == MODE_ON) {
            w_trig.trackGain(S_STASIS_OPEN, i_volume);
            w_trig.trackPlayPoly(S_STASIS_OPEN);
          }
        break;
    
        case 8:
          // Meson mode
          FIRING_MODE = MESON;
          w_trig.trackGain(S_CLICK, i_volume);
          w_trig.trackPlayPoly(S_CLICK);
          
          if(PACK_STATUS == MODE_ON) {
            w_trig.trackGain(S_MESON_OPEN, i_volume);
            w_trig.trackPlayPoly(S_MESON_OPEN);
          }
        break;
    
        case 9:
          // Settings mode
          FIRING_MODE = SETTINGS;
          w_trig.trackGain(S_CLICK, i_volume);
          w_trig.trackPlayPoly(S_CLICK);
        break;
    
        case 10:
          // Overheating
          w_trig.trackStop(S_BEEP_8);
          
          w_trig.trackGain(S_VENT_SLOW, i_volume);
          w_trig.trackPlayPoly(S_VENT_SLOW);
          b_overheating = true;
    
          // Start timer for a second smoke sound.
          ms_overheating.start(i_overheating_delay);

          // Reset some vent light timers.
          ms_vent_light_off.stop();
          ms_vent_light_on.stop();
          ms_fan_stop_timer.stop();
          ms_vent_light_off.start(i_vent_light_delay);
          ms_fan_stop_timer.start(i_fan_stop_timer);
          
          // Reset the inner cyclotron speed.
          if(i_mode_year == 1984) {
            i_inner_current_ramp_speed = i_inner_ramp_delay;
          }
        break;
    
        case 11:
          // Overheating finished
          w_trig.trackGain(S_VENT_DRY, i_volume);
          b_overheating = false;

          // Stop the fan.
          ms_fan_stop_timer.stop();

          // Turn off the n-filter fan.
          fanControl(false);
  
          // Turn off the smoke.
          smokeControl(false);

          // Reset the LEDs before resetting the alarm flag.
          if(i_mode_year == 1984) {
            resetCyclotronLeds();
          }
      
          b_alarm = false;
    
          reset2021RampUp();
    
          packStartup();
    
          // Turn off the vent light
          ventLight(false);
          ms_vent_light_off.stop();
          ms_vent_light_on.stop();
          
          ms_cyclotron.start(i_2021_delay); 
        break;
    
        case 12:
          // Reset cyclotron speed.
          cyclotronSpeedRevert();
        break;
    
        case 13:
          // Speed up cyclotron.
          cyclotronSpeedIncrease();  
        break;

        case 14:
          // The wand is still here.
          ms_wand_handshake.start(i_wand_handshake_delay);
          ms_wand_handshake_checking.start(i_wand_handshake_delay / 2);
          b_wand_connected = true;
        break;
        
        case 15:
          // Play 8 overheat beeps before we overheat.
          w_trig.trackGain(S_BEEP_8, i_volume);
          w_trig.trackPlayPoly(S_BEEP_8);
        break;
    
        case 16:
          // Wand power level 1
          i_wand_power_level = 1;

          // Reset the smoke timer if the wand is firing.
          if(b_wand_firing == true) {
            if(ms_smoke_timer.isRunning() == true) {
              ms_smoke_timer.start(i_smoke_timer[i_wand_power_level - 1]);
            }
          }
        break;
    
        case 17:
          // Wand power level 2
          i_wand_power_level = 2;

          // Reset the smoke timer if the wand is firing.
          if(b_wand_firing == true) {
            if(ms_smoke_timer.isRunning() == true) {
              ms_smoke_timer.start(i_smoke_timer[i_wand_power_level - 1]);
            }
          }
        break;
    
        case 18:
          // Wand power level 3
          i_wand_power_level = 3;
          
          // Reset the smoke timer if the wand is firing.
          if(b_wand_firing == true) {
            if(ms_smoke_timer.isRunning() == true) {
              ms_smoke_timer.start(i_smoke_timer[i_wand_power_level - 1]);
            }
          }
        break;
    
        case 19:
          // Wand power level 4
          i_wand_power_level = 4;

          // Reset the smoke timer if the wand is firing.
          if(b_wand_firing == true) {
            if(ms_smoke_timer.isRunning() == true) {
              ms_smoke_timer.start(i_smoke_timer[i_wand_power_level - 1]);
            }
          }
        break;
    
        case 20:
          // Wand power level 5
          i_wand_power_level = 5;

          // Reset the smoke timer if the wand is firing.
          if(b_wand_firing == true) {
            if(ms_smoke_timer.isRunning() == true) {
              ms_smoke_timer.start(i_smoke_timer[i_wand_power_level - 1]);
            }
          }
        break;        

        case 89:
          // Lower music volume.
          if(b_playing_music == true) {    
            if(i_volume_music_percentage - VOLUME_MUSIC_MULTIPLIER < 0) {
              i_volume_music_percentage = 0;
            }
            else {
              i_volume_music_percentage = i_volume_music_percentage - VOLUME_MUSIC_MULTIPLIER;
            }

            i_volume_music = MINIMUM_VOLUME - (MINIMUM_VOLUME * i_volume_music_percentage / 100);

            w_trig.trackGain(i_current_music_track, i_volume_music);
          }
        break;
    
        case 90:
          // Increase music volume.
         if(b_playing_music == true) {
            if(i_volume_music_percentage + VOLUME_MUSIC_MULTIPLIER > 100) {
              i_volume_music_percentage = 100;
            }
            else {
              i_volume_music_percentage = i_volume_music_percentage + VOLUME_MUSIC_MULTIPLIER;
            }

            i_volume_music = MINIMUM_VOLUME - (MINIMUM_VOLUME * i_volume_music_percentage / 100);
    
            w_trig.trackGain(i_current_music_track, i_volume_music);
          } 
        break;
        
        case 91:
          // Lower the sound effects volume.
          decreaseVolumeEffects();
        break;
    
        case 92:
          // Increase the sound effects volume.
          increaseVolumeEffects();
        break;
        
        case 93:
          // Loop the music track.
          if(b_repeat_track == false) {
            b_repeat_track = true;
            w_trig.trackLoop(i_current_music_track, 1);
          }
          else {
            b_repeat_track = false;
            w_trig.trackLoop(i_current_music_track, 0);
          }
        break;
        
        case 94:
          // Lower overall pack volume.
          decreaseVolume();
        break;
    
        case 95:
          // Increase overall pack volume.
          increaseVolume();
        break;
          
        case 96:
          // Lower music volume.
          if(b_playing_music == true) {
            if(i_volume_music_percentage - VOLUME_MUSIC_MULTIPLIER < 0) {
              i_volume_music_percentage = 0;
            }
            else {
              i_volume_music_percentage = i_volume_music_percentage - VOLUME_MUSIC_MULTIPLIER;
            }

            i_volume_music = MINIMUM_VOLUME - (MINIMUM_VOLUME * i_volume_music_percentage / 100);
    
            w_trig.trackGain(i_current_music_track, i_volume_music);
          }
        break;
    
        case 97:
          // Increase music volume.
         if(b_playing_music == true) {
            if(i_volume_music_percentage + VOLUME_MUSIC_MULTIPLIER > 100) {
              i_volume_music_percentage = 100;
            }
            else {
              i_volume_music_percentage = i_volume_music_percentage + VOLUME_MUSIC_MULTIPLIER;
            }

            i_volume_music = MINIMUM_VOLUME - (MINIMUM_VOLUME * i_volume_music_percentage / 100);
    
            w_trig.trackGain(i_current_music_track, i_volume_music);
          } 
        break;
        
        case 98:
          // Stop music.
          b_playing_music = false;
          stopMusic();
        break;
    
        case 99:
          // Play music.
          b_playing_music = true;
          playMusic();
        break;
        
        default:
          // Music track number to be played.
          if(rx_byte > 99) {
            if(b_playing_music == true) {
              stopMusic();
              i_current_music_track = rx_byte;
              playMusic();
            }
            else {
              i_current_music_track = rx_byte;
            }
          }
        break;
      }
    }
    else {
      /* 
       *  Check if the wand is telling us it is here after connecting it to the pack.
       *  Then Synchronise some settings between the pack and the wand.
       */
      if(rx_byte == 14 && prev_byte == rx_byte) {        
        // Tell the wand that the pack is here.
        Serial2.write(11);

        if(i_mode_year == 1984) {
          Serial2.write(7);
        }
        else {
          Serial2.write(8);
        }

        // Stop any music.
        Serial2.write(99);
        b_playing_music = false;
        stopMusic();

        Serial2.write(i_current_music_track);

        if(b_repeat_track == true) {
          Serial2.write(12);
        }
        else {
          Serial2.write(13);
        }
        
        // Vibration on
        if(b_vibration == true) {
          Serial2.write(5);
        }
        else {
          Serial2.write(6);
        }

        // Ribbon cable alarm.
        if(b_alarm == true) {
          Serial2.write(3);
        }
        else {
          Serial2.write(4);
        }

        // Pack status
        if(PACK_STATUS != MODE_OFF) {
          Serial2.write(1);
        }
        else {
          Serial2.write(2);
        }

        // Put the wand into volume sync mode.
        Serial2.write(15);

        // Sequence here is important. Synchronise the volume settings.
        Serial2.write(i_volume_percentage);
        Serial2.write(i_volume_master_percentage);
        Serial2.write(i_volume_music_percentage);
                 
        b_wand_connected = true;
      }
    }
  }
}

void setupWavTrigger() {
  // If the controller is powering the WAV Trigger, we should wait for the WAV trigger to finish reset before trying to send commands.
  delay(1000);
  
  // WAV Trigger's startup at 57600
  w_trig.start();
  
  delay(10);

  w_trig.stopAllTracks();
  w_trig.samplerateOffset(0); // Reset our sample rate offset        
  w_trig.masterGain(i_volume_master); // Reset the master gain db. Range is -70 to 0.
  w_trig.setAmpPwr(b_onboard_amp_enabled);
  
  // Enable track reporting from the WAV Trigger
  w_trig.setReporting(false);

  // Allow time for the WAV Triggers to respond with the version string and number of tracks.
  delay(350);
  
  char w_trig_version[VERSION_STRING_LEN]; // Firmware version.
  int w_num_tracks = w_trig.getNumTracks();
  w_trig.getVersion(w_trig_version, VERSION_STRING_LEN);

  // Build the music track count.
  i_music_count = w_num_tracks - i_last_effects_track;
  
  if(i_music_count > 0) {
    i_current_music_track = i_music_track_start; // Set the first track of music as file 100_
  }
}

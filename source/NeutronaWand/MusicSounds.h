/**
 *   gpstar Neutrona Wand - Ghostbusters Proton Pack & Neutrona Wand.
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
 *  SD Card sound files in order. If you have no sound, your SD card might be too slow. 
 *  Try a faster one. File naming 000_ is important as well. For music, it is 100_ and higher.
 *  Also note if you add more sounds to this list, you need to update the i_last_effects_track variable located at the bottom of this file to the last enum definition.
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
  S_PACK_RIBBON_ALARM_2,
  S_VOICE_1984,
  S_VOICE_1989,
  S_VOICE_2021,
  S_VOICE_VIBRATION_ENABLED,
  S_VOICE_VIBRATION_DISABLED,
  S_VOICE_VIBRATION_FIRING_ENABLED,
  S_VOICE_VIBRATION_FIRING_DISABLED,
  S_VOICE_SMOKE_ENABLED,
  S_VOICE_SMOKE_DISABLED,
  S_VOICE_CYCLOTRON_CLOCKWISE,
  S_VOICE_CYCLOTRON_COUNTER_CLOCKWISE,
  S_VOICE_CROSS_THE_STREAMS,
  S_VOICE_VIDEO_GAME_MODES,
  S_VOICE_SINGLE_LED,
  S_VOICE_THREE_LED,
  S_VOICE_OVERHEAT_ENABLED,
  S_VOICE_OVERHEAT_DISABLED,
  S_VOICE_AFTERLIFE
};

/*
 * Need to keep track which is the last sound effect, so we can iterate over the effects to adjust volume gain on them.
 */
const int i_last_effects_track = S_VOICE_AFTERLIFE;
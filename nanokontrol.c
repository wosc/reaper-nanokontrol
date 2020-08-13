// symlink to ~/Library/Application\ Support/OSCII-bot/nanokontrol.txt
// Docs: <http://www.cockos.com/oscii-bot/>

@input osc_in OSC "localhost:7050"
// localhost works fine, even though Reaper may display a local network address
// as its "Local IP" in the OSC preferences dialog.
@output osc_out OSC "localhost:7051"

// XXX Even though the device name is "nanoKONTROL2", its ports(?) are named
// in this strange and unhelpful manner, at least under macOS.
@input midi_in MIDI "SLIDER/KNOB"
@output midi_out MIDI "CTRL"


@init  // ------------------------------------------------------------

DEBUG = 0;

MIDI_CC = 176;
MIDI_ON = 127;

// These CC numbers are the Korg KONTROL Editor factory defaults.
MIDI_VOLUME = 0;
MIDI_PAN = 16;
MIDI_SOLO = 32;
MIDI_MUTE = 48;
MIDI_RECARM = 64;

MIDI_PLAY = 41;
MIDI_STOP = 42;
MIDI_RECORD = 45;
MIDI_REWIND = 43;
MIDI_FORWARD = 44;

MIDI_CYCLE = 46;
MIDI_MARKER_SET = 60;
MIDI_MARKER_PREV = 61;
MIDI_MARKER_NEXT = 62;
MIDI_TRACK_PREV = 58;
MIDI_TRACK_NEXT = 59;

// Find IDs with "Copy selected action command ID" after right-clicking an
// action in the "Actions" menu.
REAPER_GOTO_PROJECT_START = 40042;
REAPER_GOTO_PROJECT_END = 40043;
REAPER_GOTO_PREV_MARKER = 40172;
REAPER_GOTO_NEXT_MARKER = 40173;
REAPER_INSERT_MARKER = 40157;


function midisend_track(control, track) (
  msg1 = MIDI_CC;
  msg2 = control + track - 1;
  value = oscparm(0);
  msg3 = value * MIDI_ON;
  midisend(midi_out);
);


function midisend_toggle(control) (
  msg1 = MIDI_CC;
  msg2 = control;
  state = oscparm(0);
  msg3 = state * MIDI_ON;
  midisend(midi_out);
);


function oscsend_track(control, verb, value) (
  track = msg2 - control + 1;
  sprintf(#url, "n/track/%%d/%s", verb);
  oscsend(osc_out, #url, value, track);
);


function oscsend_action(action) (
  oscsend(osc_out, "i/action", action);
);


function midibool2osc() (
  msg3 == MIDI_ON ? 1 : 0;
);


/**
 * Receives MIDI messages from the hardware and then triggers Reaper commands.
 */
@midimsg

DEBUG ? printf("midi %d %d %d\n", msg1, msg2, msg3);

// Tracks
msg2 >= MIDI_VOLUME && msg2 <= MIDI_VOLUME + 8 ? (
  oscsend_track(MIDI_VOLUME, "volume", msg3 / MIDI_ON);
);
// XXX The second pan knob seems to randomly/all the time send midi messages,
// regardless of which CC is assigned to it. Maybe my hardware is broken?
msg2 >= MIDI_PAN && msg2 <= MIDI_PAN + 8 && msg2 != MIDI_PAN + 1 ? (
  oscsend_track(MIDI_PAN, "pan", msg3 / MIDI_ON);
);
msg2 >= MIDI_SOLO && msg2 <= MIDI_SOLO + 8 ? (
  oscsend_track(MIDI_SOLO, "solo", midibool2osc());
);
msg2 >= MIDI_MUTE && msg2 <= MIDI_MUTE + 8 ? (
  oscsend_track(MIDI_MUTE, "mute", midibool2osc());
);
msg2 >= MIDI_RECARM && msg2 <= MIDI_RECARM + 8 ? (
  oscsend_track(MIDI_RECARM, "recarm", midibool2osc());
);


// Transport
msg3 == MIDI_ON ? (
  msg2 == MIDI_PLAY ? oscsend(osc_out, "t/play");
  msg2 == MIDI_STOP ? oscsend(osc_out, "t/stop");
  msg2 == MIDI_RECORD ? oscsend(osc_out, "t/record");
  msg2 == MIDI_TRACK_PREV ? oscsend_action(REAPER_GOTO_PROJECT_START);
  msg2 == MIDI_TRACK_NEXT ? oscsend_action(REAPER_GOTO_PROJECT_END);
  msg2 == MIDI_MARKER_PREV ? oscsend_action(REAPER_GOTO_PREV_MARKER);
  msg2 == MIDI_MARKER_NEXT ? oscsend_action(REAPER_GOTO_NEXT_MARKER);
  msg2 == MIDI_MARKER_SET ? oscsend_action(REAPER_INSERT_MARKER);
);

// cycle button is configured as "toggle"
msg2 == MIDI_CYCLE ? oscsend(osc_out, "b/repeat", midibool2osc());

// rew/fwd are configured as "momentary", so we can hold them down for moving
msg2 == MIDI_REWIND ? oscsend(osc_out, "b/rewind", midibool2osc());
msg2 == MIDI_FORWARD ? oscsend(osc_out, "b/forward", midibool2osc());


/**
 * Receives OSC messages from Reaper and then turns hardware LEDs on/off.
 */
@oscmsg

DEBUG ? printf("osc %s\n", oscstr);

// Tracks
oscmatch("/track/%d/solo", track) ? midisend_track(MIDI_SOLO, track);
oscmatch("/track/%d/mute", track) ? midisend_track(MIDI_MUTE, track);
oscmatch("/track/%d/recarm", track) ? midisend_track(MIDI_RECARM, track);

// Transport
oscmatch("/play") ? midisend_toggle(MIDI_PLAY);
oscmatch("/record") ? midisend_toggle(MIDI_RECORD);
oscmatch("/repeat") ? midisend_toggle(MIDI_CYCLE);

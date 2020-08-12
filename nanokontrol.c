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

MIDI_CC = 176;
MIDI_ON = 127;

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


@oscmsg  // ------------------------------------------------------------
//printf("osc %s\n", oscstr);  // DEBUG


// Tracks

oscmatch("/track/%d/solo", track) ? (
  state = oscparm(0);
  msg1 = MIDI_CC;
  msg2 = MIDI_SOLO + track - 1;
  msg3 = state * MIDI_ON;
  midisend(midi_out);
);

oscmatch("/track/%d/mute", track) ? (
  state = oscparm(0);
  msg1 = MIDI_CC;
  msg2 = MIDI_MUTE + track - 1;
  msg3 = state * MIDI_ON;
  midisend(midi_out);
);

oscmatch("/track/%d/recarm", track) ? (
  state = oscparm(0);
  msg1 = MIDI_CC;
  msg2 = MIDI_RECARM + track - 1;
  msg3 = state * MIDI_ON;
  midisend(midi_out);
);


// Transport

oscmatch("/play") ? (
  state = oscparm(0);
  msg1 = MIDI_CC;
  msg2 = MIDI_PLAY;
  msg3 = state * MIDI_ON;
  midisend(midi_out);
);

oscmatch("/record") ? (
  state = oscparm(0);
  msg1 = MIDI_CC;
  msg2 = MIDI_RECORD;
  msg3 = state * MIDI_ON;
  midisend(midi_out);
);

oscmatch("/repeat") ? (
  state = oscparm(0);
  msg1 = MIDI_CC;
  msg2 = MIDI_CYCLE;
  msg3 = state * MIDI_ON;
  midisend(midi_out);
);


@midimsg  // ------------------------------------------------------------
//printf("midi %d %d %d\n", msg1, msg2, msg3);  // DEBUG


// Tracks

msg2 >= MIDI_VOLUME && msg2 <= MIDI_VOLUME + 8 ? (
  track = msg2 - MIDI_VOLUME + 1;
  value = msg3 / MIDI_ON;
  oscsend(osc_out, "n/track/%d/volume", value, track);
);

// XXX The second pan knob seems to randomly/all the time send midi messages,
// regardless of which CC is assigned to it. Maybe my hardware is broken?
msg2 >= MIDI_PAN && msg2 <= MIDI_PAN + 8 && msg2 != MIDI_PAN + 1 ? (
  track = msg2 - MIDI_PAN + 1;
  value = msg3 / MIDI_ON;
  oscsend(osc_out, "n/track/%d/pan", value, track);
);

msg2 >= MIDI_SOLO && msg2 <= MIDI_SOLO + 8 ? (
  track = msg2 - MIDI_SOLO + 1;
  state = msg3 == MIDI_ON ? 1 : 0;
  oscsend(osc_out, "b/track/%d/solo", state, track);
);

msg2 >= MIDI_MUTE && msg2 <= MIDI_MUTE + 8 ? (
  track = msg2 - MIDI_MUTE + 1;
  state = msg3 == MIDI_ON ? 1 : 0;
  oscsend(osc_out, "b/track/%d/mute", state, track);
);

msg2 >= MIDI_RECARM && msg2 <= MIDI_RECARM + 8 ? (
  track = msg2 - MIDI_RECARM + 1;
  state = msg3 == MIDI_ON ? 1 : 0;
  oscsend(osc_out, "b/track/%d/recarm", state, track);
);


// Transport

msg2 == MIDI_PLAY && msg3 == MIDI_ON ? (
  oscsend(osc_out, "t/play");
);

msg2 == MIDI_STOP && msg3 == MIDI_ON ? (
 oscsend(osc_out, "t/stop");
);

msg2 == MIDI_RECORD && msg3 == MIDI_ON ? (
  oscsend(osc_out, "t/record");
);

msg2 == MIDI_REWIND ? (
  state = msg3 == MIDI_ON ? 1 : 0;
  oscsend(osc_out, "b/rewind", state);
);

msg2 == MIDI_FORWARD ? (
  state = msg3 == MIDI_ON ? 1 : 0;
  oscsend(osc_out, "b/forward", state);
);

msg2 == MIDI_CYCLE ? (
  state = msg3 == MIDI_ON ? 1 : 0;
  oscsend(osc_out, "b/repeat", state);
);

msg2 == MIDI_TRACK_PREV && msg3 == MIDI_ON ? (
  oscsend(osc_out, "i/action", 40042);  // goto project start
);

msg2 == MIDI_TRACK_NEXT && msg3 == MIDI_ON ? (
  oscsend(osc_out, "i/action", 40043);  // goto project end
);

msg2 == MIDI_MARKER_PREV && msg3 == MIDI_ON ? (
 oscsend(osc_out, "i/action", 40172);  // goto prev marker
);

msg2 == MIDI_MARKER_NEXT && msg3 == MIDI_ON ? (
  oscsend(osc_out, "i/action", 40173);  // goto next marker
);

msg2 == MIDI_MARKER_SET && msg3 == MIDI_ON ? (
   oscsend(osc_out, "i/action", 40157);  // insert marker at current position
);

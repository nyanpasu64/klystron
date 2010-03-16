#ifndef MUSIC_H
#define MUSIC_H

/*
Copyright (c) 2009-2010 Tero Lindeman (kometbomb)

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/


#include "cyd.h"
#include "cydfx.h"

#define MUS_PROG_LEN 32
#define MUS_MAX_CHANNELS CYD_MAX_CHANNELS

#define MUS_VERSION 11

#define MUS_SONG_TITLE_LEN 64
#define MUS_INSTRUMENT_NAME_LEN 32

typedef struct
{
	Uint32 flags;
	Uint32 cydflags;
	CydAdsr adsr;
	Uint8 sync_source, ring_mod; // 0xff == self
	Uint16 pw;
	Uint8 volume;
	Uint16 program[MUS_PROG_LEN];
	Uint8 prog_period; 
	Uint8 vibrato_speed, vibrato_depth, slide_speed, pwm_speed, pwm_depth;
	Uint8 base_note;
	Uint16 cutoff;
	Uint8 resonance;
	Uint8 flttype;
	Uint8 ym_env_shape;
	Sint16 buzz_offset;
	Uint8 fx_bus, vib_shape, vib_delay, pwm_shape;
	char name[MUS_INSTRUMENT_NAME_LEN + 1];
} MusInstrument;

enum
{
	MUS_INST_PROG_SPEED_RELATIVE = 0, // chn.current_tick / mus.tick_period * ins.prog_period
	MUS_INST_PROG_SPEED_ABSOLUTE = 1, // absolute number of ticks
	MUS_INST_DRUM = 2,
	MUS_INST_INVERT_VIBRATO_BIT = 4,
	MUS_INST_LOCK_NOTE = 8,
	MUS_INST_SET_PW = 16,
	MUS_INST_SET_CUTOFF = 32,
	MUS_INST_YM_BUZZ = 64,
	MUS_INST_RELATIVE_VOLUME = 128
};

typedef struct
{
	MusInstrument *instrument;
	Uint16 note;
	// ------
	Uint8 arpeggio_note;
	Uint16 target_note, last_note, fixed_note;
	volatile Uint32 flags;
	Uint32 current_tick;
	Uint8 program_counter, program_tick, program_loop, prog_period;
	Sint16 buzz_offset;
} MusChannel;

typedef struct
{
	Uint8 note, instrument, ctrl;
	Uint16 command;
} MusStep;

typedef struct
{
	Uint16 position; 
	Uint16 pattern;
	Sint8 note_offset;
} MusSeqPattern;

typedef struct
{
	MusStep *step;
	Uint16 num_steps;
} MusPattern;

typedef struct
{
	MusInstrument *instrument;
	Uint8 num_instruments;
	MusPattern *pattern;
	Uint16 num_patterns;
	MusSeqPattern *sequence[MUS_MAX_CHANNELS];
	Uint16 num_sequences[MUS_MAX_CHANNELS];
	Uint16 song_length, loop_point;
	Uint8 song_speed, song_speed2, song_rate;
	Uint16 time_signature;
	Uint32 flags;
	Uint8 num_channels;
	Uint8 multiplex_period;
	char title[MUS_SONG_TITLE_LEN + 1];
	CydFxSerialized fx[CYD_MAX_FX_CHANNELS];
} MusSong;


typedef struct
{
	int channel;
	MusInstrument *instrument;
	Uint8 note;
} MusDelayedTrigger;

typedef struct
{
	MusDelayedTrigger delayed;
	MusPattern *pattern;
	Uint8 last_ctrl;
	Uint16 pw, pattern_step, sequence_position, slide_speed;
	Uint16 vibrato_position, pwm_position;
	Sint8 note_offset;
	Uint16 filter_cutoff;
	Uint8 extarp1, extarp2;
	Uint8 volume;
	Uint8 vib_delay;
} MusTrackStatus;

typedef struct
{
	MusChannel channel[MUS_MAX_CHANNELS];
	Uint8 tick_period; // 1 = at the rate this is polled
	// ----
	MusTrackStatus song_track[MUS_MAX_CHANNELS];
	MusSong *song;
	Uint8 song_counter;
	Uint16 song_position;
	CydEngine *cyd;
	Uint8 current_tick;
	Uint8 volume; // 0..128
	Uint8 multiplex_ctr;
} MusEngine;


enum
{
	MUS_CHN_PLAYING = 1,
	MUS_CHN_PROGRAM_RUNNING = 2,
	MUS_CHN_DISABLED = 4
};

enum
{
	MUS_NOTE_NONE = 0xff,
	MUS_NOTE_RELEASE = 0xfe
};

enum
{
	MUS_PAK_BIT_NOTE = 1,
	MUS_PAK_BIT_INST = 2,
	MUS_PAK_BIT_CTRL = 4,
	MUS_PAK_BIT_CMD = 8
};

#define MIDDLE_C (12*4)
#define MUS_NOTE_NO_INSTRUMENT 0xff
#define MUS_CTRL_BIT 1
#define MAX_VOLUME 128

enum
{
	MUS_FX_ARPEGGIO = 0x0000,
	MUS_FX_ARPEGGIO_ABS = 0x4000,
	MUS_FX_SET_EXT_ARP = 0x1000,
	MUS_FX_PORTA_UP = 0x0100,
	MUS_FX_PORTA_DN = 0x0200,
	MUS_FX_SLIDE = 0x0300,
	MUS_FX_VIBRATO = 0x0400,
	MUS_FX_FADE_VOLUME = 0x0a00,
	MUS_FX_SET_VOLUME = 0x0c00,
	MUS_FX_EXT = 0x0e00,
	MUS_FX_EXT_PORTA_UP = 0x0e10,
	MUS_FX_EXT_PORTA_DN = 0x0e20,
	MUS_FX_EXT_RETRIGGER = 0x0e90,
	MUS_FX_EXT_FADE_VOLUME_DN = 0x0ea0,
	MUS_FX_EXT_FADE_VOLUME_UP = 0x0eb0,
	MUS_FX_EXT_NOTE_CUT = 0x0ec0,
	MUS_FX_EXT_NOTE_DELAY = 0x0ed0,
	MUS_FX_SET_SPEED = 0x0f00,
	MUS_FX_SET_RATE = 0x1f00,
	MUS_FX_PORTA_UP_SEMI = 0x1100,
	MUS_FX_PORTA_DN_SEMI = 0x1200,
	MUS_FX_SET_PANNING = 0x1800,
	MUS_FX_PAN_LEFT = 0x1700,
	MUS_FX_PAN_RIGHT = 0x1900,
	MUS_FX_CUTOFF_UP = 0x2100,
	MUS_FX_CUTOFF_DN = 0x2200,
	MUS_FX_CUTOFF_SET = 0x2900,
	MUS_FX_FILTER_TYPE = 0x2b00,
	MUS_FX_BUZZ_UP = 0x3100,
	MUS_FX_BUZZ_DN = 0x3200,
	MUS_FX_BUZZ_SHAPE = 0x3f00,
	MUS_FX_BUZZ_SET = 0x3900,
	MUS_FX_BUZZ_SET_SEMI = 0x3a00,
	MUS_FX_PW_DN = 0x0700,
	MUS_FX_PW_UP = 0x0800,
	MUS_FX_PW_SET = 0x0900,
	MUS_FX_SET_WAVEFORM = 0x0b00,
	MUS_FX_CUTOFF_FINE_SET = 0x6000,
	MUS_FX_END = 0xffff,
	MUS_FX_JUMP = 0xff00,
	MUS_FX_LABEL = 0xfd00,
	MUS_FX_LOOP = 0xfe00,
	MUS_FX_TRIGGER_RELEASE = 0x7c00,
	MUS_FX_NOP = 0xfffe
};

enum
{
	MUS_CTRL_LEGATO = MUS_CTRL_BIT,
	MUS_CTRL_SLIDE = MUS_CTRL_BIT << 1,
	MUS_CTRL_VIB = MUS_CTRL_BIT << 2
	
};

enum
{
	MUS_ENABLE_REVERB = 1,
	MUS_ENABLE_CRUSH = 2,
	MUS_ENABLE_MULTIPLEX = 4,
	MUS_NO_REPEAT = 8
};

enum
{
	MUS_SHAPE_SINE,
	MUS_SHAPE_RAMP_UP,
	MUS_SHAPE_RAMP_DN,
	MUS_SHAPE_RANDOM,
	MUS_SHAPE_SQUARE,
	MUS_NUM_SHAPES
};

#define MUS_INST_SIG "cyd!inst"
#define MUS_SONG_SIG "cyd!song"

int mus_advance_tick(void* udata);
int mus_trigger_instrument(MusEngine* mus, int chan, MusInstrument *ins, Uint8 note);
void mus_release(MusEngine* mus, int chan);
void mus_init_engine(MusEngine *mus, CydEngine *cyd);
void mus_set_song(MusEngine *mus, MusSong *song, Uint16 position);
int mus_poll_status(MusEngine *mus, int *song_position, int *pattern_position, MusPattern **pattern, MusChannel *);
int mus_load_instrument_file(Uint8 version, FILE *f, MusInstrument *inst);
int mus_load_instrument_file2(FILE *f, MusInstrument *inst);
int mus_load_instrument(const char *path, MusInstrument *inst);
void mus_get_default_instrument(MusInstrument *inst);
int mus_load_song(const char *path, MusSong *song);
int mus_load_song_file(FILE *f, MusSong *song);
void mus_free_song(MusSong *song);
void mus_set_fx(MusEngine *mus, MusSong *song);

#endif

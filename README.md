# Korg nanoKONTROL2 as a Reaper OSC control surface

Enables using a [Korg nanoKONTROL2](https://www.korg.com/products/computergear/nanokontrol2/) to control [Reaper](https://reaper.fm/).

Tested under macOS 10.13 with Reaper 6.13.


## Setup

**NOTE**: Before using this, see if one of the (less complex) alternatives mentioned below works for your use case!

### Set hardware to "MIDI Control Change" mode

Hold the set-marker and cycle buttons before plugging in the USB cable.

### Configure hardware buttons to be toggles

* Download the [KORG KONTROL Editor](https://www.korg.com/us/support/download/product/0/159/) program
* Set the S/M/R buttons and the "cycle" button behavior to "toggle" (instead of the default "momentary")
* Set the LED mode to "external"
* Write changes to device (Communication::Write Scene Data)

See these [screenshots](https://github.com/Pierousseau/reaper_generic_control/blob/master/README.md#how-do-i-use-it-) for details, and/or use my `toggle.nktrl2_data` file.

### Set up OSC device for Reaper

* Copy `nanokokontor.ReaperOSC` to the `$REAPER/OSC` folder (Options::Show Reaper resource path)

### Set up OSC/MIDI converter

* Download oscii-bot from https://www.cockos.com/oscii-bot/
* Copy `nanokontrol.c` to `~/Library/Application\ Support/OSCII-bot/nanokontrol.txt`
* Start oscii-bot

### Add control surface

* Go to Preferences::Control/OSC/web and click "add"
* Set mode to "OSC" and the ports to match the ones specified in `nanokontrol.c`

![](control-surface.png)


## Alternatives

### Reaper built-in control modes

Before you use this, first see if the simpler, Reaper built-in mechanisms works for you, as recommended e.g. [here](https://songwritingandrecordingtips.wordpress.com/2012/04/21/recording-using-a-korg-nanokontrol-2-with-reaper/),
namely add a "control surface" in the Preferences::Control/OSC/web section, using

* "Mackie Control Universal" mode (set the hardware to "Sonar" by holding set-marker+record button s before plugging it in)
* "HUI (partial)" mode (set hardware to "Protools" with set+play)

In HUI mode, the "cycle" or "marker" buttons were non-functional for me, which is a pity.

In Mackie mode moving one fader sometimes also changed the volume of *other* tracks for me, which makes it completely unusable for me.


Windows users may have better luck with the ["Klinke" enhanced Mackie mode plugin](https://bitbucket.org/Klinkenstecker/csurf_klinke_mcu/downloads/)
(see also [the introductory forum post](https://forum.cockos.com/showthread.php?t=81818)).


### Reaper MIDI input

* Set the device up as a MIDI controller, by enabling "input" and "control" messages under Preferences::Audio::MIDI.
* Assign functionality to the individual controls by using the "Actions" menu, and/or import a [ready-made keymap](https://forum.cockos.com/showthread.php?t=196669)

This setup has the **major drawback** of being input only, i.e. Reaper does not send changes performed in its UI to the device, so the LED status on the hardware will likely diverge rather quickly.
So this is only an option if you *only ever* are going to use the hardware controls.


### Custom MIDI control mode (Windows)

To combat the "input only" design of the built-in MIDI control functionality, Pierre Rousseau created a plugin that provides a ["generic" control surface mode](https://github.com/Pierousseau/reaper_generic_control) ([announcement](https://forum.cockos.com/showthread.php?t=192466)) that allows mapping/customizing of MIDI input/output commands.

However, this plugin currently only works under Windows, not macOS.


### Prior art: OSC control mode

Since I did not think porting the "generic" control mode to macOS would be feasible for me, I went looking for a bidirectional(!) Reaper built-in alternative.

[This forum post](https://forum.cockos.com/showpost.php?p=1413930&postcount=5) confirms that MIDI input is unidirectonal, and points to OSC.
For this we need a bidirectional(!) OSC-MIDI converter.
The directly linked posts were kind of dead ends, but another [forum thread](https://forum.cockos.com/showpost.php?p=1401937&postcount=284) had a complete example, although for a different device, using [oscii-bot](https://www.cockos.com/oscii-bot/).

I guess [Python with liblo/mididings](https://github.com/noedigcode/ArdourSongSwitcher/blob/master/ardourSongSwitcher.py) would also be an acceptable platform,
but oscii-bot already has working macOS integration (and is written in C, thus hopefully less the cause of performance issues), so I went with that.


## Implementation notes


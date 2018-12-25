## Euclid

#### [Overview](#overview) | [Design](#design)

### Overview

This module uses an Arduino Mega as the backbone for a 6 channel, 16 step Euclidian trigger sequencer. Euclidian patterns are based around a used defined pattern length and number of triggers. These triggers are distrubuted as evenly as possible over the pattern length.

### Design

This module is currently in it's protoype phase so it has been built on protoboard. A Neopixel ring shows the pattern of the currently selected channel. A pushbutton encoder allows the pattern length, trigger count, and rotation to be adjusted. Each channel has a seperate output jack. A potentiometer allows the playback speed to be adjusted and an input jack allows it to be clocked to en external source. Three buttons allow the module to be paused, patterns to be roatated, and sequences to be written to the internal EEPROM for later playback.

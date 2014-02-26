# My computer corner

This repo describes my
"home office automation", a.k.a. the "wire up some cool gadgets" project.

Please feel free to get inspired,
contact me if something's not explained adequately,
and reuse the code, all of which is freely licensed.
I'm not much of a giant, but I try to make it easy to stand on my shoulders :)

All Python code is available under the MIT license,
All Arduino/C++ code is available under the GPL
(due to used libraries;
if you need some part under a more permissive license let me know).

# Major parts

## A computer

I have an aging Thinkpad,
confined to a docking station due to some disabilities.
It runs Fedora, because I'm familiar with that system.
It's hooked up to the network, some USB gadgets, and a couple of monitors.
It has some heating problems (which were even worse on windows, fwiw),
which means the fan gives a lot of noise, so I prefer to let it sleep
when I'm not using it.

## A Raspberry PI

A small, always-on computer for the project's network communication and
light-duty computation needs.
The Pi runs Arch Linux.

## The Encuino

The I/O is handled by a DYI arduino-compatible board:
an ATmega328 chip on a solderless breadboard,
with some supporting electronics around it.
It is talks to the Pi via the serial interface cable
to receive and transmit commands.

# Functions

## The Clockface

The Encuino is connected to a WS2812 LED strip,
arranged in a square.
The Clockface allows displaying data such as time or pie graphs.

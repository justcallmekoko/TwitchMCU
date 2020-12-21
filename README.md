# TwitchMCU
 A Twitch Chat reactive LED controller

## Table of Contents
- [About](#about)
- [Commands](#commands)
- [Patterns](#patterns)

## About
This is a thing that does stuff. Type commands in Twitch Chat during the stream and the LEDs will respond accordingly.

## Commands
#### All commands are identified by "$!"
| Command           | Notes                   | Result                                |
| ----------------- | ----------------------- | ------------------------------------- |
| `$!help`          |                         | Provides URL to this repo             |
| `$!{"pattern":x}` | Replace 'x' with number | Displays the pattern specified by 'x' |
| `$!{"msg":"x"}`   | Replace 'x' with string | Displays message on LED panel         |

## Patterns
| Index   | Pattern      |
| ------- | ------------ |
| 0       | LOAD_RING    |
| 1       | TWINKLE      |
| 2       | PULSE_RED    |
| 3       | PULSE_PURPLE |
| 4       | RAINBOW      |

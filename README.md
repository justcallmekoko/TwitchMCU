# TwitchMCU
 A Twitch Chat reactive LED controller

## Table of Contents
- [About](#about)
- [Commands](#commands)
- [Patterns](#patterns)

## About
This is a thing that does stuff. Type commands in Twitch Chat during the stream and the LEDs will respond accordingly.

## Thermal Printer Stream
If the camera is pointed at a thermal receipt printer, the following commands apply

### Commands
| Command          | Notes                  | Result                   |
| ---------------- | ---------------------- | ------------------------ |
| `Cheer10 msg`    | Replace 'msg' with your cheer message | Thermal printer will print the cheer message (Cheer must be >10) |

## LED Panel Stream
If the camera is pointed at an LED panel, the following commands apply
### Commands
#### All commands are identified by "$!"
| Command           | Notes                     | Result                                |
| ----------------- | ------------------------- | ------------------------------------- |
| `$!help`          |                           | Provides URL to this repo             |
| `$!{"pattern":x}` | Replace 'x' with number   | Displays the pattern specified by 'x' |
| `$!msg`           | Replace 'msg' with string | Displays message on LED panel         |

### Patterns
| Index   | Pattern      |
| ------- | ------------ |
| 0       | LOAD_RING    |
| 1       | TWINKLE      |

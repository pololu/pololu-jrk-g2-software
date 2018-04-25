# Jrk G2 software

Version: 1.1.0<br>
Release date: 2018-04-25<br>
[www.pololu.com](https://www.pololu.com/)

This repository contains the source code of the configuration and control software for
the [Jrk G2 USB Motor Controllers with Feedback](jrk).
There are two programs:
the Pololu Jrk G2 Configuration Utility (jrk2gui)
and the Pololu Jrk G2 Command-line Utility (jrk2gui).

## Installation

Installers for this software are available for download from the
[Jrk G2 user's guide][guide].

See [BUILDING.md](BUILDING.md) for information about how to compile the software
from source.

## Version history

- 1.1.0 (2018-04-25):
  - Added the ability to change the GUI font size using the
    `JRK2GUI_FONT_SIZE` variable.
  - Fixed compilation issues on Ubuntu and macOS.
  - Changed the GUI to use Qt's Fusion theme on non-Windows platforms.
  - Added scripts to build Linux and macOS installers.
- 1.0.0 (2018-04-20): Original release.

[jrk]: https://www.pololu.com/jrk
[guide]: https://www.pololu.com/docs/0J73
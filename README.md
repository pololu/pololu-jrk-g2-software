# Jrk G2 software

Version: 1.3.0<br>
Release date: 2018-07-20<br>
[www.pololu.com](https://www.pololu.com/)

This repository contains the source code of the configuration and control software for
the [Jrk G2 USB Motor Controllers with Feedback][jrk].
There are two programs:
the Pololu Jrk G2 Configuration Utility (jrk2gui)
and the Pololu Jrk G2 Command-line Utility (jrk2cmd).

## Installation

Installers for this software are available for download from the
[Jrk G2 user's guide][guide].

See [BUILDING.md](BUILDING.md) for information about how to compile the software
from source.

## Version history

- 1.3.0 (2018-07-20):
  - Added support for the new Jrk G2 21v3.
  - Added `jrk_reinitialize_and_reset_errors` which behaves like the
    reinitialize command from the original Jrks: it clears all the latched
    errors and sets the "Awaiting command" error if the input mode is serial.
  - Changed `jrk_restore_defaults` to use `jrk_reinitialize_and_reset_errors`
    so that the software still stop the motor when you restore to the
    default settings.
- 1.2.0 (2018-06-25):
  - Jrk G2 Configuration Utility (jrk2gui):
    - Major changes to the graph window:
      - You can now set the scale and vertical position.
      - Each visible plot now has an arrow indicating its zero position, and
        arrows to indicate if it is overflowing above or below the graph.
      - You can click on a plot or its arrows in order to select it, and then:
        - Use the mouse wheel to zoom that plot in or out, or
        - Use the mouse to drag it up and down.
      - New buttons for resetting the position and scale.
      - You can change plot colors.
      - You can change the graph to use a dark theme.
      - You can save and load graph settings files now.
      - The Y axis is no longer labeled.
      - Fixed a bug that would cause memory leaks while the graph is paused.
    - Change the 'Manually set target box':
      - Made it visible from any tab.
      - Made is use a slider control instead of a scrollbar.
      - Added a "Center" button.
    - Input wizard:
      - Added a "Skip" button to skip the step of learning the neutral point.
      - Changed it to be much more like the feedback wizard.
    - Display the dead zone in the input scaling tab.
    - Added the "Variables" window.
    - Added the "Info" tab and moved some information from the status tab to there.
    - Show two digits after the decimal point for the "Raw current" variable
      in the Status tab and allow its graph to show more resolution as well.
    - Bug fixes:
      - Fix some bugs in the controls for setting the hard current limits and soft
        current limits.
      - Fix a bug where the "Graph" command in the "Window" menu would not succeed
        if the graph was minimized.
      - Fix a bug where the range of the target scroll bar and the corresponding
        number entry would not get updated when you click "Apply settings",
        changing the feedback mode.
      - Fix a bug where the target scroll bar was not being updated when you
        type a target in and press Enter.
      - Fix a bug where you could enter "0" as a deceleration limit (but the GUI
        would warn you and prevent that setting from being applied).
    - Several other minor changes and bug fixes.
  - Jrk G2 Command-line Utility (jrk2cmd):
    - Make the `--run` and `--target` options work together properly.
    - Expand the allowed range of `--speed` arguments to -2048 to 2047, making
      it useful for frequency feedback.
- 1.1.0 (2018-04-25):
  - Added the ability to change the GUI font size using the
    `JRK2GUI_FONT_SIZE` variable.
  - Fixed compilation issues on Ubuntu and macOS.
  - Changed the GUI to use Qt's Fusion theme on non-Windows platforms.
  - Increased the width of the Feedback Wizard.
  - Added scripts to build Linux and macOS installers.
- 1.0.0 (2018-04-20): Original release.

[jrk]: https://www.pololu.com/jrk
[guide]: https://www.pololu.com/docs/0J73

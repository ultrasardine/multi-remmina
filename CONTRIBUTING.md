# Contributing to Multi-Remmina

Welcome, we are a kind, open-minded, simple community. All help welcome, in any way you can think of.

## Coding
**Multi-Remmina** uses the [fork and pull](https://stackoverflow.com/questions/11582995/what-is-the-fork-pull-model-in-github) model.

If you'd like to make changes to this project, the following steps will get you there!

  1. Fork [this repo](https://github.com/ultrasardine/multi-remmina/fork)
  2. Make your changes
  3. [Submit a pull request](https://github.com/ultrasardine/multi-remmina/pulls)

The [wiki](https://github.com/ultrasardine/multi-remmina/wiki) has details on how to compile Multi-Remmina and FreeRDP.

Look for issues labeled ["help wanted"](https://github.com/ultrasardine/multi-remmina/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22).

### Fix existing bugs
In general, this is the most important task.
To see all the issues already labeled as bugs, you can use [this query](https://github.com/ultrasardine/multi-remmina/issues?q=is%3Aissue+is%3Aopen+label%3Abug)

### Implement features
We have a list of feature requests labeled as [enhancement](https://github.com/ultrasardine/multi-remmina/issues?q=is%3Aissue+is%3Aopen+label%3Aenhancement)

### Internal projects
  * Refactoring `remmina_connection_window.c` as it is big, complicated and hard to extend.
  * Simplify the user interface.
  * Separate plugin processes from the main process to improve stability.
  * Write an additional plugin system to write plugins also in Python and eventually Ruby.

## Documentation
Submit user and developer guides in any (accessible) format you want (screencasts, PDF, HTML, Open Document, .gs, .ps, LaTeX, GitHub wiki, etc).
* [Wiki](https://github.com/ultrasardine/multi-remmina/wiki)

Commenting the source code and writing checks is also an ongoing process.

## Report bugs, ideas, issues
Install it, use it and report what doesn't work, is missing, or ugly. Don't hesitate to let us know.
For bug and feature requests, use [GitHub issues](https://github.com/ultrasardine/multi-remmina/issues).

## Screenshots
Send in pictures or screengrabs of your setup.

## Translation
Help translate Multi-Remmina to your language.
Translation contributions are welcome via pull requests.

## Videos
- As short as you can. Around 2 minutes to maximum 4 minutes.
- Avoid showing copyrighted and distracting content, even custom/other desktop icons should not be shown.

Videos demonstrating different ways to use Multi-Remmina are welcome!

These would make good topics for videos:
- Configuration and use of a basic RDP profile, quickly showing the most common controls (resizes, toolbar placement, screenshots, …).
- Creation of a default profile.
- Creation of a connection to a VNC server.
- Using a basic SSH tunnel.

### Desktop/screen video recorder
- [SimpleScreenRecorder](https://www.maartenbaert.be/simplescreenrecorder/)
- [OBS Studio](https://obsproject.com/)

### Video editors
There are many video editors for Linux|GNU. [OpenShot](https://www.openshot.org/) is good.
To produce good looking videos choose a 1080p profile.

## Upstream Relationship
Multi-Remmina maintains compatibility with the original [Remmina project](https://gitlab.com/Remmina/Remmina). We aim to:
- Keep protocol compatibility
- Share bug fixes where applicable
- Maintain plugin API compatibility

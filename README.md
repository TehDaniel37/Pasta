# PaSta
<b>Pa</b>nel<b>Sta</b>tus is a work-in-progress program for generating information (date/time, WiFi status, CPU temperature, etc.) for your Linux window manager status panel/bar. The program is developed primarily to learn about processes in Linux.

What sets it apart from similar project, like [i3status](https://github.com/i3/i3status), is that you are expected to write your status module scripts yourself (Though example modules will be provided). It will be possible to write them in any language, compiled or interpreted, as long as the script is executable on your system. This makes PaSta very flexible - if there is something you want to display in your status panel, like the number of available AUR upgrades on your Arch Linux setup or the test coverage on your currently opened development project - you will be able to display it as long as you can write a script that echoes it.

The modules are executed as seperate processes, which will make sure modules that take longer to execute do not block the other modules and that each module is immediately updated when it's script has finished running.

PaSta has two types of modules: 

  - Polled, which gets executed on an interval, for example every second or every 30 minutes.
  - Subscribed, which get executed immediately when a command sends a notification. Examples of such programs can be `bspc subscribe desktop`, which reports whenever your active workspace changes in [bspwm](https://github.com/baskerville/bspwm).

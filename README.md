# Schedr <img align="right" src="./doc/tests.svg"> <img align="right" src="./doc/coverage.svg">
Schedr is a work-in-progress program for scheduling commands/jobs on Linux. Commands can be schedule to run at an interval (e.g. `every minute`), run at specific times (e.g. `every friday at 12 am`) or in response to events (e.g. ``when `nmcli device monitor` outputs ".*: connected" ``)

A first MVP version can now be tested. This is a early version that only supports interval type jobs (`every X seconds/minutes/hours`).  

## Installing
1. Clone or download repository
2. cd to directory where Schedr was cloned/downloaded
3. run `make install`

## Configuring
Add jobs to the configuration file at `$HOME/.config/schedr/schedr.conf`

The configuration file uses the following format:
```
Job "<JOB NAME>"
    run `<COMMAND>`
    every <VAL> <seconds/minutes/hours>
```

For example, here is a job entry which changes the desktop wallpaper every 30 minutes using `feh`
```
Job "Change wallpaper"
    run `feh --randomize --no-fehbg --bg-scale $HOME/Pictures/Wallpapers`
    every 30 minutes
```

#### Schedule scripts
You can also schedule scripts (or any other type of executable file) to be run, simply by writing the file name as command. Like this:
```
Job "Backup"
    run `my_backup_script.sh`
    every 6 hours
```
You can store scripts exclusive to Schedr in `~/.config/schedr/bin`.

#### Redirecting output
For the MVP, only output to stdout is supported. For now, you can circument this by appending a standard redirection operator to the job command, like so:

```
Job "Backup"
    run `my_backup_script.sh 2>&1 >> ~/.backup.log`
    every 6 hours
```

## Running
Add an entry to `schedr &` in your autostart configuration. For example in your [Xinitrc](https://wiki.archlinux.org/index.php/Xinitrc), [i3-config](https://wiki.archlinux.org/index.php/I3#Configuration) or [bspwmrc](https://wiki.archlinux.org/index.php/Bspwm#Configuration).

## Uninstalling
Run `make uninstall` in the folder where Schedr was cloned downloaded. Alternativly you can just remove the file `/usr/local/bin/schedr`. To purge all configurations you also need to remove `~/.config/schedr`.

---

What sets Schedr apart from similar project, [(see Similar Projects)](#similar-projects), is a focus on expressive, easy to read DSL syntax as well as scheduling of commands in response to events. The jobs are maintained as seperate processes which runs and monitors the commands, which makes sure that jobs which take longer to execute do not block the other jobs.

The program is written in plain C and is developed primarily to learn about processes in Linux as well as Test Driven Development (TDD).

## Similar projects
- [Cron](https://en.wikipedia.org/wiki/Cron)
- [anacron](http://anacron.sourceforge.net/)
- [dcron](https://github.com/dubiousjim/dcron)
- [fcron](http://fcron.free.fr/)
- [Jobber](https://github.com/dshearer/jobber)

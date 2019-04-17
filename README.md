# Dodee <img align="right" src="./doc/tests.svg"> <img align="right" src="./doc/coverage.svg">

## Installing
1. Clone or download repository
2. cd to directory where Dodee was cloned/downloaded
3. run `make install`

See [Dependencies](#dependencies) if make complains about missing commands.

You can also [download a binary](https://github.com/almgru/Dodee/releases/download/mvp-1711/dodee-171126.tar.gz) if you don't want to deal with building. Then you need to manually copy the `dodee` binary to a directory in your `$PATH`, like `/usr/local/bin`, to install it.

## Configuring
Add tasks to the configuration file at `$HOME/.config/dodee/dodee.conf`

## Running
Add an entry to `dodee &` in your autostart configuration. For example in your [Xinitrc](https://wiki.archlinux.org/index.php/Xinitrc), [i3-config](https://wiki.archlinux.org/index.php/I3#Configuration) or [bspwmrc](https://wiki.archlinux.org/index.php/Bspwm#Configuration).

## Uninstalling
Run `make uninstall` in the folder where Dodee was cloned/downloaded. Alternativly you can just remove the file `/usr/local/bin/dodee`. To purge all configurations you also need to remove `~/.config/dodee`.

## Dependencies

#### Running
- _GNU C Library_ (_glibc_) >= 2.15

#### Building
- _GCC_ >= 6.3.0
- _GNU make_ >= 4.1
- _Gcovr_ >= 3.3 (for `make cov`)
- _Valgrind_ >= 3.13 (for `make memcheck`)
- _Cppcheck_ >=1.8.1 (for `make check`)
- _sudo_ (for `make install` / `make uninstall`)
- _ldconfig_ (for `make install`)

---

What sets Dodee apart from similar project, [(see Similar Projects)](#similar-projects), is a focus on expressive, easy to read DSL syntax as well as scheduling of commands in response to events. The jobs are maintained as seperate processes which runs and monitors the commands, which makes sure that jobs which take longer to execute do not block the other jobs.

The program is written in plain C and is developed primarily to learn about processes in Linux as well as Test Driven Development (TDD).

## Similar projects
- [Cron](https://en.wikipedia.org/wiki/Cron)
- [anacron](http://anacron.sourceforge.net/)
- [dcron](https://github.com/dubiousjim/dcron)
- [fcron](http://fcron.free.fr/)
- [Jobber](https://github.com/dshearer/jobber)

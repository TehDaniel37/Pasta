# Schedr
Schedr is a work-in-progress program for scheduling commands/jobs on Linux. Commands can be schedule to run at an interval (e.g. `every minute`, run at specific times (e.g. `every friday at 12 am`) or in response to events (e.g. ``when `nmcli device monitor` `` )

What sets Schedr apart from similar project, [(see Similar Projects)](#similar-projects), is a focus on expressive, easy to read DSL syntax as well as scheduling of commands in response to events. For example, here is a Schedr module which runs a backup of your home directory every wednesday at noon:

```
Module "backup"
  run `rsync -a --delete --quiet /folder/to/backup /location/of/backup`
  every monday at 12:00
```

The modules are maintained as seperate processes which runs and monitors the commands, which makes sure that modules which take longer to execute do not block the other modules.

The program is written in plain C and is developed primarily to learn about processes in Linux as well as Test Driven Development (TDD).

## Similar projects
- [Cron](https://en.wikipedia.org/wiki/Cron)
- [anacron](http://anacron.sourceforge.net/)
- [dcron](https://github.com/dubiousjim/dcron)
- [fcron](http://fcron.free.fr/)
- [Jobber](https://github.com/dshearer/jobber)

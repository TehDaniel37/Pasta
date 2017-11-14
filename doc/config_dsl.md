# Config DSL Example

## File location
Config file should be named `schedr.conf` and be loaded from `$HOME/.config/schedr`, `$HOME/.schedr.conf` or a custom location specified via a flag.

## Module syntax iteration 0

```
Job "<job name>" 
	run `<command>`|<executable file>
	every <interval>
```

Where `<interval>` is in the format `<value> <unit>`. 

Available values for `unit` are:

+ `s`/`sec`/`second(s)`
+ `m`/`min`/`minute(s)`
+ `h`/`hour(s)`

### Example running a command every second

```
Job "datetime"
    run `date '+%F %T'`
    every 1 sec
```

### Example running a script every 30 minutes
```
Job "upgrades"
    run pasta_upgrades.sh
    every 30 minutes
```

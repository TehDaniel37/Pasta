# Config DSL Example

## File location
Config file should be named `pasta.conf` and be loaded from `$HOME/.config/pasta`, `$HOME/.pasta.conf` or a custom location specified via a flag.

## Module syntax iteration 0
`[<Name of module>] `

`run: <command>`

`every: <interval to execute the script with>`

### Example running a command every second
`[Date/Time]`

`run: date '+%F %T'`

`every: 1 sec`

### Example running a script every 30 minutes
`[Upgrades]`

`run: pasta_upgrades.sh`

`every: 30 min`
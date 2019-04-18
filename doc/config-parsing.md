# Algorithm for parsing config file

collect scripts
    define "scripts"
    collect executable files in "$XDG_CONFIG_HOME/dodee/{intervals,events,actions}" into "scripts"
collect tasks
    define "tasks"
    split config file into task sections
        define var "task sections"
        foreach line in config file
            add line to "task sections"
    split task sections into "task scripts"
        define var "task scripts"
        foreach task section in "task sections"
            define "task"
            set "task.hash" to md5sum of task section
            split task section by "if|when|and|or|then"
            foreach script definition in split
                compile regex pattern
                    define "pattern"
                    add "^" (beginning of line) to "pattern"
                    foreach word in in script definition
                        add "(?:$word|(ARG\d+))" to "pattern"
                    add "$" (end of line) to "pattern"
                match "pattern" against "scripts"
                    if match:
                        define "script"
                        set "script.path" to matching file in "scripts"
                        foreach captured group in match:
                            add captured group to "script.args"
                        if matching script is in directory "*/intervals/":
                            set "task.interval" to "script"
                        else if matching script is in directory "*/events/":
                            add "script" to "task.events"
                        else if matching script is in directory "*/actions/":
                            add "script" to "task.actions"
            add "task" to "tasks"
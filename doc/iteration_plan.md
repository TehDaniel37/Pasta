# PaSta iteration plan

## Iteration 0
After this iteration the program should be able to:

- Load polled type modules from the configuration file
- Implement functions for starting a module and stopping a module
- The output from the modules should be visible on stdout

The program should run the loaded modules in parallell. Each module should be executed, and the program shall then wait `interval` number of seconds and then execute the module again, until the function to stop the module is called or the program exits.

### Time estimation

####Loading of configuration files
| Task                                    | estimation             |
| --------------------------------------- | ---------------------- |
| Decide on configuration format          | 1 hour                 |
| Implement *Module*                      | 1 hour                 |
| Implement loading of configuration file | 30 minutes             |
| Implement parsing of configuration file | 2 hours                |
| Write tests for *ConfigParser*          | 1 hour                 |
| **Total**                               | **5 hours 30 minutes** |

#### Implement starting and stopping modules
| Task                                    | estimation             |
| --------------------------------------- | ---------------------- |
| Design *Scheduler*                      | 2 hour                 |
| Implement *Scheduler*                   | 2 hour                 |
| Write tests for *Scheduler*             | 1 hour                 |
| **Total**                               | **5 hours**            |

#### Total: 10 hours 30 minutes
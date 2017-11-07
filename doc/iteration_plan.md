# PaSta iteration plan

## Iteration 0
After this iteration the program should be able to:

- Load polled type modules from the configuration file
- Implement functions for starting a module and stopping a module
- The output from the modules should be visible on stdout

The program should run the loaded modules in parallell. Each module should be executed, and the program shall then wait `interval` number of seconds and then execute the module again, until the function to stop the module is called or the program exits. 

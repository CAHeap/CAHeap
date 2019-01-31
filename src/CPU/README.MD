# CAHeap CPU Project
***
## Enviroment
- `Linux`

## How to Install and Test

1. Choose measurement tasks and use the command `make *` under the directory `demo` to generate executable files and run them with user-defined experimental parameters

- `testflowsize`: perform flow size estimation
- `testheavychange`: perform heavy change finding 
- `testflowsizedistribution`: perform flow size distribution estimation
- `testentropy`: perform entropy estimation
- `testcard`: perform cardinality estimation
- `testheavyhitter`: perform heavy hitter finding
- `testddos`: perform DDos attack finding
- `testspreader`: perform Super-Spreader finding
- `testrecentflow`: perform recent flow finding
- `testpersistent`: perform persistent flow finding
- `testflowlet`: perform flowlet finding
- `testburst`: perform burst finding
- `testspeed`: perform speed test

2. Or use corresponding scripts `*.sh` to perform the tasks with default experimental parameters (`./all` for all)

	**Note that all the experimental results are stored in the directory `results`.**
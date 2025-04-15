## Usage
```bash
$ ./smc_d -h
SMC Driver - Helper script for SMC project

Usage: smc_d -c|-b|-r|-t [mode]

Options:
  -c [mode]       Configure project 
  -b [mode]       Build project 
  -r [mode]       Run the project 
  -t [mode]       Run tests 
  -h              Display this help message

Modes:
  release                 Release mode (optimized)
  debug                   Debug mode (with debug symbols)

Examples:
  smc_d -c debug             Configure project in debug mode
  smc_d -b release           Build project in release mode
  smc_d -r debug             Run the debug binary
  smc_d -t release           Run tests with release build
```
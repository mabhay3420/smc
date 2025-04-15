## Usage
We need llvm installed and set `CT_LLVM_INSTALL_DIR` variable in
[CMakePresets.json](CMakePresets.json) file. 

I built llvm using following command ( On apple silicon):
```bash
# Inside llvm-project directory ( assuming you have cloned llvm-project repo )
cmake -GNinja -Bbuild -Hllvm \
    -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD="arm64" \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_ENABLE_ASSERTIONS=true \
    -DLLVM_CCACHE_BUILD=true \
    -DLLVM_USE_LINKER=lld

ninja -Cbuild
```

`lld` might not be installed by default - but is recommended for faster
building.

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
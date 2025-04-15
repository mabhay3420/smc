

## Configure and Build

configure:
```
cmake --preset release
```

build:
```
cmake --build --preset release
```

For debug build:
```
cmake --preset debug
cmake --build --preset debug
```

## Run
Run the binary:
```
./build/release/smc
```

## Test

Build binaries:
```
cmake --preset release
cmake --build --preset release
```

Run tests:
```
ctest --test-dir build/release
```

## Debug
Debug binary location: `build/debug/smc`

1. Using lldb:
```
lldb build/debug/smc
```

set breakpoint at `main` function:
```
break main
```
and so on

2. Using VSCode:
    a. Install `Codelldb` extension.
    b.  Use `Debug` configuration.
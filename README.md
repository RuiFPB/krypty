# krypty
 - [Build](#build) (Before building anything, you need to clone the repository)
   - [Linux](#linux)
   - [Windows](#windows)
   - [MacOS](#macos)

# Build

## Linux
Compile with: (libedit is needed)

To build using the default compiler:
```sh
make
```

If you want to choose the compiler, do:
```sh
make CC="[compiler]"
```

## Windows
### Mingw
```sh
make LIBEDIT=""
```
(libedit is not supported on Windows, so don't try to use it during compilation)

## MacOS
I cannot test on macOS, but it should be similar to linux

# riptop

A simple `htop` clone to accustom myself to modern C++.

![riptop_basic](doc/riptop_basic.png)

## Operating systems

* Windows 10 x64 only.


## Build instructions:
```powershell
mkdir build
cd build
cmake ..
cmake --build .
```

## External dependencies

* [FTXUI library](https://github.com/ArthurSonzogni/ftxui) for terminal UI.
* [WIL (Windows Implementation Libraries)](https://github.com/microsoft/wil) for accessing Windows API in a modern safe C++ way.
* [Catch2](https://github.com/catchorg/Catch2) for unit tests.


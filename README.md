# Decomp Training

Welcome to decomp!

This series of (currently WIP) example files is designed to teach you the basics of decompilation for the Gamecube and Wii. It is recommended to read [this](https://wiki.decomp.dev/en/resources/decomp-intro) accompanying article on the wiki before getting started.

To get started, follow the [build](#building) steps and download [objdiff](https://github.com/encounter/objdiff). After building, set the active project in objdiff by selecting your project directory in File -> Project -> Project directory. All lessons are contained in [src/training_template](src/training_template). Avoid looking at files in `src/training_answers` to prevent accidentally getting spoiled on the solutions. 

## Technical information

This repository builds a DOL with CodeWarrior for the purpose of loading it into analysis software. It is intended to be compiled with minimal runtime, and is not intended to play back on console. The build script is fairly simple for now, but more features could be added in the future to support things like relocatables or splitting the executable with dtk to demonstrate linking behavior. 

## Planned content:

see [here](https://github.com/thefoxcam/decomp-training/wiki)

## Dependencies

- ninja 1.3
- Python 3.6

The configure script will pull all other dependencies.

## Building

```bash
python configure.py
ninja
```

# eydis

A lightweight (basic and slow) `ARM` (`64-bit, little endianness`) reverse engineering tool in progress.

<hr />

## I.  Requierements
1. Basic compiling tools,
2. An ARM64 Image.

## II.  Usage
```
usage: eydis <image> [args]
   -d, --database [file]   specify a database,             # "-d image.eydis"
   -r, --rebase [addr]     specify the rebase address,     # "-r 0x100000000"

   -s, --start [addr]      start from a specified address, # "-s 0x30"
   -e, --end [addr]        end to a specified address.     # "-e 0x200"
```

> Eydis owns a built-in shell, please use the 'help' command to see the available ones.

## III.  Planified tasks
1. Rename subroutines,
2. Make a 32-bit support,
3. Implement a way to create plugins/loaders/[...],
4. Optimize the most possible everything that can be optimized,
5. Make an interactive `GUI` (using `Qt5` / `NCurses` / `SDL` / [...] / whatever).

## IV.  Notes
- `Eydis` was made for learning purpose, is not really meant to be a complete disassembler and aims to be working with Apple's bootloaders (but any other `ARM64` images are supported).

- If there is any issues / wishes, please report them in the `Issues` section!!

- This C++ version is the result of me trying new things, however, some features of the C version were removed for the time being (with the hope to improve it and making this tool useful for the future), so if anyone noticed any improvements, please open a PR!

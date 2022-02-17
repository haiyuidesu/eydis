# eydis

A lightweight (basic and slow) ARM (little endianness) reverse engineering tool.

## I.  Requierements
1. macOS/Linux,
2. Basics compiling tools,
3. The [SQLite3](https://www.sqlite.org/download.html) + [readline](https://ftp.gnu.org/gnu/readline/) frameworks,

## II.  Usage

```
usage: eydis [-d] <image> [args]
   -w [file]	specify a database,
   -b [addr]	specify the rebase address,    # "-b 0x100000000"

   -s [offset]	start from a specified offset, # "-s 0x30"
   -e [offset]	end to a specified offset,     # "-e 0x200"

   -a    analyze the aera until an ending offset. # use it with the '-e' flag!
                                                  # also, be careful when this flag is used since it can skip some routine beginnings. 
```

By the way, eydis has a built-in shell:

```
eydis > help

[shell_help]: exit				exit safely eydis,
[shell_help]: save				save a patched file,
[shell_help]: reload 				reload the actual disassemblage,

[shell_help]: patch [addr] [hex] 		patch a certain instruction of the image,
[shell_help]: name [old_name] [new_name]	rename an existing subroutine,
[shell_help]: set [start_addr] [end_addr]	set new disassembling limits.
```

### Examples
1. If you own a database (with the original file corresponding to) with renamed subroutines, you can specify it before running eydis with the `-w` flag:
```
$ eydis -d original_arm_file.bin -w db_with_renamed_functions
```

<hr />

2. If you ever need to patch one of the file that you are disassembling, you can use the `patch` command:

_The `base_addr` will be `0x240000000` here_

```
eydis > patch 0x24000d4b0 0x00008052 (< The patch in hexadecimal, not swapped)
                 (^ Address where the patch has to be)
```
_I advise to use this [website](https://armconverter.com/)._

<hr />

3. If you ever need to rename a subroutine, you can use the `name` command:

```
eydis > name sub_2400112C8 _DERImg4Decode (< Its new name)
                 (^ the actual name of the subroutine)
```

<hr />

4. Disassembling a whole file may be boring (because waiting for the end can be very long) but you can re-specify (either before running eydis or after with the shell) the disassembling limits, here is an example with the shell:

```
eydis > set 0x30 0x7c (< Where the disassemblage will ends)
            (^ Where the disassemblage will starts)
```

<hr />

Then when everything is done, you can use the `reload` command to display or `save` to save your modified file!

## III.  The 'TODO' list
```
> Optimize the most possible the whole program,
> Just make an interactive GUI (help.......).
```
~~Make a better user experience (using the 'ncurses' library),~~
~~Avoid to re-print the whole image every times that the 'reload' command is called,~~
~~Avoid to use GNU readline (I use it because it is more advanced than a simple stdin read).~~

(they all are long and hard tasks and I don't even know if I can do everything...)

## IV.  Notes
- Eydis was made for learning purpose and is not really meant to be a complete disassembler,

- If there is any bugs | issues | problems | wishes, please, report it in the `Issues` section!!

- A 32bit support is planned but not for now (however, if it is absolutely necessary I will do my best),

- This project is still under development so please enjoy at least the time you will spend with it (^v^)

- The 32bit support is incomplete and may output wrong instructions, I leave it there anyways in the hope to fix it later.

Small command-line tool for testing of the folllowing:
* Pass unicode characters as command-line arguments. The executable filename already has a `Æ大` suffix which should automatically show up in `argv[0]`.
* Print >2 byte "surrogate" unicode characters like `🤍` (U+1F90D white heart) back to the console.

![screenshot](figures/screenshot.png)

### Known issues

|         Issue             |       Workaround       |
|---------------------------|------------------------|
| Non-ASCII characters in `argv[]` are truncated to `?` if using `int main(int argc, char* argv[])` as entry point. | Instead use `int wmain(int argc, wchar_t* argv[])` as entry point. |
| Windows command prompt (`cmd.exe`) seem unable to display >2 byte "surrogate" characters written to console. | Use bash shell instead. |

Please contact the repo author if you discover more problems or are aware of better solutions or workarounds.

## UTF-8 display in Command Prompt
First need to change the active code page with `chcp 65001` to activate UTF-8 mode.
```
>chcp 65001
Active code page: 65001

>type UTF-8_BOM.txt
﻿Kåre Sæther er en ørn 🤍.
>type UTF-8.txt
Kåre Sæther er en ørn 🤍.
```

## Links
* [Windows Command-Line: Unicode and UTF-8 Output Text Buffer](https://devblogs.microsoft.com/commandline/windows-command-line-unicode-and-utf-8-output-text-buffer/)

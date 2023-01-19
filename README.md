Small command-line tool for testing of the folllowing:
* Pass unicode characters as command-line arguments. The executable filename has a `Æ大` suffix which should automatically show up in `argv[0]`.
* Print unicode characters back to the console.


### Known issues
|         Issue             |       Workaround       |
|---------------------------|------------------------|
| Non-ASCII characters in arguments are truncated to `?` if using `int main(int argc, char* argv[])` as entry point. | Instead use `int wmain(int argc, wchar_t* argv[])` as entry point. |
| Windows command prompt (`cmd.exe`) unable to display non-ASCII characters written to console. | Instead use bash shell. |

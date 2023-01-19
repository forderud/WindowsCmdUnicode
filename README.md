Unicode console and command-line processing on Windows

|         Issue             |       Workaround       |
|---------------------------|------------------------|
| Non-ASCII characters are truncated to `?` if using the `int main(int argc, char* argv[])` entry point. | Instead use `int wmain(int argc, wchar_t* argv[])` as entry-point. |
| Windows command prompt (`cmd.exe`) unable to display non-ASCII characters. | Instead use bash shell. |

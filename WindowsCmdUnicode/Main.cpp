#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <memory>
#include <iostream>
#include <streambuf>
#include <stdarg.h>

/* 
ISSUE: Windows command prompt (cmd.exe) seem unable to display >2 byte "surrogate" characters written to console.
WORK-AROUND: Use bash shell instead.
*/

void PrintArguments(int argc, wchar_t* argv[]) {
    for (int i = 0; i < argc; i++)
        wprintf(L"ARGUMENT[%i]: %s\n", i, argv[i]);
}
void PrintArguments(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++)
        printf("ARGUMENT[%i]: %s\n", i, argv[i]);
}

#ifdef UNICODE

// UCRT writes wide console output one wchar_t at a time, which splits surrogate pairs.
// Work-around: write whole strings with WriteConsoleW (CRT stream if redirected).
void WriteStdout(const wchar_t* str, size_t len) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    static const bool isConsole = [out] { DWORD mode = 0; return GetConsoleMode(out, &mode) != 0; }();
    if (isConsole) {
        fflush(stdout);
        WriteConsoleW(out, str, (DWORD)len, nullptr, nullptr);
    } else {
        fwprintf(stdout, L"%.*s", (int)len, str);
    }
}

// wprintf replacement that formats into a buffer before writing
int wprintf_console(const wchar_t* format, ...) {
    va_list args;
    va_start(args, format);
    int len = _vscwprintf(format, args);
    va_end(args);
    if (len < 0)
        return len;

    std::wstring buf(len, L'\0');
    va_start(args, format);
    vswprintf_s(&buf[0], buf.size() + 1, format, args);
    va_end(args);

    WriteStdout(buf.data(), buf.size());
    return len;
}

// std::wcout stream buffer that writes whole chunks
class ConsoleStreambuf : public std::wstreambuf {
public:
    ConsoleStreambuf() {
        setp(m_buf, m_buf + _countof(m_buf) - 1); // reserve 1 slot for overflow char
    }
    ~ConsoleStreambuf() override {
        sync();
    }

protected:
    int_type overflow(int_type ch) override {
        if (ch != traits_type::eof()) {
            *pptr() = traits_type::to_char_type(ch);
            pbump(1);
        }
        Flush();
        return traits_type::not_eof(ch);
    }
    int sync() override {
        Flush();
        return 0;
    }

private:
    void Flush() {
        size_t len = pptr() - pbase();
        wchar_t carry = 0; // keep trailing high surrogate for next chunk
        if (len > 0 && IS_HIGH_SURROGATE(pbase()[len - 1]))
            carry = pbase()[--len];

        WriteStdout(pbase(), len);
        setp(m_buf, m_buf + _countof(m_buf) - 1);
        if (carry) {
            *pptr() = carry;
            pbump(1);
        }
    }

    wchar_t m_buf[1024];
};

// Unicode entry-point
int wmain(int argc, wchar_t* argv[]) {
    wprintf(L"# Unicode mode\n");
    //SetConsoleOutputCP(CP_UTF8);

    const wchar_t headphone[] = L"Headphone: \U0001F3A7\n"; // non-BMP character https://en.wikibooks.org/wiki/Unicode/Character_reference/1F000-1FFFF
    const wchar_t heart[] = L"White heart: \U0001F90D\n"; // non-BMP character

    // bypass standard output and write directly to console
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), headphone, (DWORD)wcslen(headphone), nullptr, nullptr); // works in all consoles

    // configuring either _O_U8TEXT, _O_U16TEXT or _O_WTEXT fixes unicode output to bash
    fflush(stdout); // flush before switching
    //_setmode(_fileno(stdout), _O_U8TEXT); //
    _setmode(_fileno(stdout), _O_U16TEXT); //
    //_setmode(_fileno(stdout), _O_WTEXT); //

    // UTF-16 console output
    wprintf(headphone); // only works from bash
    wprintf(heart); // only works from bash
    wprintf(L"\n");

    wprintf_console(L"%s", headphone); // works in all consoles
    wprintf_console(L"%s", heart); // works in all consoles

    ConsoleStreambuf consoleBuf;
    std::wstreambuf* prevBuf = std::wcout.rdbuf(&consoleBuf);
    std::wcout << headphone << heart << std::endl; // works in all consoles
    std::wcout.rdbuf(prevBuf);
    wprintf(L"\n");

    // print command-line arguments
    wprintf(L"Regular command-line arguments:\n");
    PrintArguments(argc, argv);
    wprintf(L"\n");

    wprintf(L"__argv command-line arguments:\n");
    PrintArguments(__argc, __wargv);
    wprintf(L"\n");

    // retrieve command-line arguments programatically
    std::unique_ptr<wchar_t*, HLOCAL(*)(HLOCAL)> wargv(CommandLineToArgvW(GetCommandLineW(), &argc), LocalFree);

    // print command-line arguments again
    wprintf(L"GetCommandLine command-line arguments:\n");
    PrintArguments(argc, wargv.get());

    return 0;
}

#else

// ASCII entry-point
int main(int argc, char* argv[]) {
    printf("# Non-Unicode mode\n");
    // ISSUE: Non-ASCII argv characters are truncated to '?'

    // UTF-8 console output
    printf(u8"Headphone: \U0001F3A7\n"); // https://en.wikibooks.org/wiki/Unicode/Character_reference/1F000-1FFFF (works after "chcp 65001")
    printf(u8"White heart: \U0001F90D\n"); // (works after "chcp 65001")
    printf(u8"\n");

    // print command-line arguments
    printf("Regular command-line arguments:\n");
    PrintArguments(argc, argv);
    printf("\n");

    printf("__argv command-line arguments:\n");
    PrintArguments(__argc, __argv);
    printf("\n");

    // UTF-8 console output (flush before switching)
    fflush(stdout);
    _setmode(_fileno(stdout), _O_U8TEXT); // confirmed to work from bash

    // retrieve arguments programatically (there's no ASCII variant of CommandLineToArgvW)
    argc = 0;
    std::unique_ptr<wchar_t*, HLOCAL(*)(HLOCAL)> wargv(CommandLineToArgvW(GetCommandLineW(), &argc), LocalFree);

    // print command-line arguments again
    wprintf(L"GetCommandLine command-line arguments:\n");
    PrintArguments(argc, wargv.get());

    return 0;
}

#endif

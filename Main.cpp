#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

/* 
ISSUE: Windows command prompt (cmd.exe) seem unable to display non-ASCII characters written to console.
WORK-AROUND: Use bash shell instead.
*/

#ifdef UNICODE

// Unicode entry-point
int wmain(int argc, wchar_t* argv[]) {
    // UTF-8 console output
    _setmode(_fileno(stdout), _O_U8TEXT); // confirmed to work from bash

    // print command-line arguments
    wprintf(L"Regular command-line arguments:\n");
    for (int i = 0; i < argc; i++)
        wprintf(L"ARGUMENT[%i]: %s\n", i, argv[i]);
    wprintf(L"\n");

    // retrieve arguments programatically
    argc = 0;
    wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &argc);

    // print command-line arguments again
    wprintf(L"GetCommandLine command-line arguments:\n");
    for (int i = 0; i < argc; i++)
        wprintf(L"ARGUMENT[%i]: %s\n", i, wargv[i]);

    LocalFree(wargv);

    return 0;
}

#else

// ASCII entry-point
int main(int argc, char* argv[]) {
    // ISSUE: Non-ASCII argv characters are truncated to '?'

    // print command-line arguments
    printf("Regular command-line arguments:\n");
    for (int i = 0; i < argc; i++)
        printf("ARGUMENT[%i]: %s\n", i, argv[i]);
    printf("\n");

    // UTF-8 console output (flush before switching)
    fflush(stdout);
    _setmode(_fileno(stdout), _O_U8TEXT); // confirmed to work from bash

    // retrieve arguments programatically (there's no ASCII variant of CommandLineToArgvW)
    argc = 0;
    wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &argc);

    // print command-line arguments again
    wprintf(L"GetCommandLine command-line arguments:\n");
    for (int i = 0; i < argc; i++)
        wprintf(L"ARGUMENT[%i]: %s\n", i, wargv[i]);

    LocalFree(wargv);

    return 0;
}

#endif

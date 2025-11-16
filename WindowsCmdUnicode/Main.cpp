#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <memory>

/* 
ISSUE: Windows command prompt (cmd.exe) seem unable to display non-ASCII characters written to console.
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

// Unicode entry-point
int wmain(int argc, wchar_t* argv[]) {
    // UTF-8 console output
    //SetConsoleOutputCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_U8TEXT); // confirmed to work from bash

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
    // ISSUE: Non-ASCII argv characters are truncated to '?'

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

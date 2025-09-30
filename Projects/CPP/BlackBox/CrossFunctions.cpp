#include "CrossFunctions.h"

#if defined(_WIN32) || defined(_WIN64)

#include <conio.h>
#include <windows.h>

char cross_getch()
{
    return static_cast<char>(_getch());
}

short cross_get_width()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(h, &csbi)) {
        return static_cast<short> (csbi.srWindow.Right - csbi.srWindow.Left + 1);
    }
    return 80;
}

short cross_get_length()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) {
        return -1; // error
    }
    return static_cast <short> (csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
}

#else  // POSIX (Linux/macOS)

#include <termios.h>
#include <unistd.h>
#include <cstdio>   // perror
#include <cstdlib>  // exit, EXIT_FAILURE
#include <sys/ioctl.h> // library to read 

static struct termios orig_termios;

static void disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

static void enable_raw_mode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) < 0) //save orig terminal to struct
    {
        perror("tcgetattr");
        std::exit(EXIT_FAILURE);
    }
    std::atexit(disable_raw_mode);// at exit return default term settings
    struct termios raw = orig_termios; //read orig terminal settings 
    raw.c_lflag &= ~(ECHO | ICANON); //disable echo and canonical mode
    raw.c_cc[VMIN] = 1; //exact one char to read
    raw.c_cc[VTIME] = 0; //no time limit to read
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0)
    {
        perror("tcsetattr");
        std::exit(EXIT_FAILURE);
    }
}

char cross_getch()
{
    enable_raw_mode();
    char c;
    if (read(STDIN_FILENO, &c, 1) < 0) {
        perror("read");
        std::exit(EXIT_FAILURE);
    }
    return c;
}

short cross_get_width()
{
    struct winsize window;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window) == 0) //getting terminal informations
    {
        return static_cast<short>(window.ws_col); 
    }
    return 80;
}

short cross_get_length()
{
    struct winsize window;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window) == -1) {
        return -1; // error
    }
    return window.ws_row;
}

#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// terminal, low level

struct termios origTermios;

void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios) == -1) die("tcsetattr");
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &origTermios) == -1) die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  // c_cc == control characters, array of bytes that control various terminal
  // settings VMIN == min number of bytes of input needed before read() can
  // return VTIME == max amount of time to wait before read() returns
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

char editorReadKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }
  return c;
}

// output

/**
 * Clears the terminal screen.
 */
void editorRefreshScreen() {
  write(STDOUT_FILENO, "\x1b[2J", 4);
}

// input handling, higher level

[[nodiscard]] constexpr int extractCtrlKey(char k) { return k & 0x1f; }

void editorProcessKeypress() {
  char c = editorReadKey();

  switch (c) {  
    case extractCtrlKey('q'): exit(0); break;

  }
}

int main() {
  enableRawMode();
  while (true) {
    editorRefreshScreen();
    editorProcessKeypress();
  }
  disableRawMode();
  return 0;
}
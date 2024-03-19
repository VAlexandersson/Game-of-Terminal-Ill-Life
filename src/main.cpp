#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios origTermios;

constexpr int CTRL_KEY(char k) { return k & 0x1f; }

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

int main() {
  enableRawMode();

  while (true) {
    char c = '\0';
    int nread = read(STDIN_FILENO, &c, 1);

    if (nread == -1 && errno != EAGAIN) die("read");

    if (nread == 0) continue;

    iscntrl(c) ? printf("%d\r\n", c) : printf("%d ('%c')\r\n", c, c);
    if (c == CTRL_KEY('q')) break;
  }
  disableRawMode();

  return 0;
}
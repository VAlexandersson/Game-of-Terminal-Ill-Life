#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

void clearScreenAndCursor() {
  write(STDOUT_FILENO, "\x1b[2J", 4); // clear screen
  write(STDOUT_FILENO, "\x1b[H", 3); // move cursor to top left
}


/*** data***/

struct editorConfig {
  int screenrows;
  int screencols;
  struct termios origTermios;
};

struct editorConfig E;

/*** terminal, low level ***/ 

void die(const char *s) {
  clearScreenAndCursor();

  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.origTermios) == -1) die("tcsetattr");
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &E.origTermios) == -1) die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = E.origTermios;
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

int getCursorPosition(int *row, int *col) {
  char buf[32];
  unsigned int i = 0;

  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
    if (buf[i] == 'R') break;
    i++;
  }
  buf[i] = '\0';

  if (buf[0] != '\x1b' || buf[1] != '[') return -1;
  if (sscanf(&buf[2], "%d;%d", row, col) != 2) return -1;

  return 0;
}

int getWindowSize(int *row, int *col) {
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
    return getCursorPosition(row, col);
  } else {
    *col = ws.ws_col;
    *row = ws.ws_row;
    return 0;
  }

}
/*** output ***/ 

void editorDrawRows() {
  int y;
  for (y = 0; y < E.screenrows; y++) {
    write(STDOUT_FILENO, "~", 1);

    if (y < E.screenrows - 1) {
      write(STDOUT_FILENO, "\r\n", 2);
    }
  }
}

void editorRefreshScreen() {
  clearScreenAndCursor();

  editorDrawRows();
  write(STDOUT_FILENO, "\x1b[H", 3);
}


/*** input handling, higher level ***/ 

[[nodiscard]] constexpr int extractCtrlKey(char k) { return k & 0x1f; }

void editorProcessKeypress() {
  char c = editorReadKey();

  switch (c) {  
    case extractCtrlKey('q'): 
      clearScreenAndCursor();
      exit(0); 
      break;

  }
}


/*** init ***/

void initEditor() {
  if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}


int main() {
  enableRawMode();
  initEditor();
  while (true) {
    editorRefreshScreen();
    editorProcessKeypress();
  }
  disableRawMode();
  return 0;
}
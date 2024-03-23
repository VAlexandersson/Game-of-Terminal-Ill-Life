#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <string>

const char* GOL_VERSION =  "0.0.0.0.1";

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

/*** append buffer ***/
struct abuf {
  char *b;
  int len;
};

#define ABUF_INIT {NULL, 0}

void abAppend(struct abuf *ab, const char *s, int len) {
  char *anew = (char *)realloc(ab->b, ab->len + len);

  if (anew == NULL) return;
  memcpy(&anew[ab->len], s, len);
  ab->b = anew;
  ab->len += len;
}

void abFree(struct abuf *ab) {
  free(ab->b);
}

/*** output ***/ 

void editorDrawRows(struct abuf *ab) {
  int y;
  for (y = 0; y < E.screenrows; y++) {
    if (y == E.screenrows / 3) {
      std::string welcome = "GOL -- version ";
      welcome += GOL_VERSION;
      if (welcome.size() > E.screencols) welcome = welcome.substr(0, E.screencols);

      int padding = (E.screencols - welcome.size()) / 2;

      if (padding) {
        abAppend(ab, "~", 1);
        padding--;
      }

      while (padding--) abAppend(ab, " ", 1);
      abAppend(ab, welcome.c_str(), welcome.size());
    } else {
      abAppend(ab, "~", 1);
    }

    abAppend(ab, "\x1b[K", 3); // clear line
    if (y < E.screenrows - 1) {
      abAppend(ab, "\r\n", 2);
    }
  }
}

void editorRefreshScreen() {
  struct abuf ab = ABUF_INIT;

  abAppend(&ab, "\x1b[?25l", 6); // hide cursor
  //abAppend(&ab, "\x1b[2J", 4); // clear screen 
  abAppend(&ab, "\x1b[H", 3); // move cursor to top left

  editorDrawRows(&ab);

  abAppend(&ab, "\x1b[H", 3); // move cursor to top left
  abAppend(&ab, "\x1b[?25h", 6); // show cursor

  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab);
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
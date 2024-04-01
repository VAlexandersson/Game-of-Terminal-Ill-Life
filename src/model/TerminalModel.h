#pragma once
#include <termios.h>
#include <iostream>

class TerminalModel {
public:
  TerminalModel();
  ~TerminalModel();
  size_t getScreenRows() { return screenrows; }
  size_t getScreenCols() { return screencols; }
  size_t getCx() { return cx; }
  size_t getCy() { return cy; }
  void editorMoveCursor(char key);
private:
  size_t screenrows, screencols;
  size_t cx, cy;
  termios origTermios;
  void enableRawMode();
  void disableRawMode();
  int getWindowSize();
};

TerminalModel::TerminalModel() : cx(0), cy(0) {
  enableRawMode();
  if(getWindowSize() == -1) {
    std::cout << "getWindowSize failed\r\n";
  }
}

TerminalModel::~TerminalModel() {
  disableRawMode();
  std::cout << "cx: " << cx << " cy: " << cy << "\r\n";
  std::cout << "screenrows: " << screenrows << " screencols: " << screencols << "\r\n";
  std::cout << "TerminalModel destroyed\r\n";
}

void TerminalModel::enableRawMode() {
  tcgetattr(STDIN_FILENO, &origTermios);
  termios raw = origTermios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void TerminalModel::disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios);
}

int TerminalModel::getWindowSize() {
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    return -1; 
  } 
  else {
    screencols = ws.ws_col;
    screenrows = ws.ws_row;
    return 0;
  }
}



void TerminalModel::editorMoveCursor(char key) {
  switch (key) {
    case 'w':
      if (cy != 0) {
        cy--;
      }
      break;
    case 's':
      if (cy != screenrows - 1) {
        cy++;
      }
      break;
    case 'a':
      if (cx != 0) {
        cx--;
      }
      break;
    case 'd':
      if (cx != screencols - 1) {
        cx++;
      }
      break;
  }
}

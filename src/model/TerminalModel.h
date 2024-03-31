#pragma once
#include <termios.h>
#include <iostream>


class TerminalModel {

public:
  TerminalModel();
  ~TerminalModel();
  
  int getScreenRows() { return screenrows; }
  int getScreenCols() { return screencols; }

  void editorMoveCursor(char key);

private:
  termios origTermios;
  std::string buffer;
  int cx, cy;
  int screenrows, screencols;

  void enableRawMode();
  void disableRawMode();
  int getWindowSize();

  void AppendBuffer(const std::string& string) {
    buffer.append(string);
  }
};

TerminalModel::TerminalModel() {
  enableRawMode();
  if(getWindowSize() == -1) {
    std::cout << "getWindowSize failed\r\n";
  }
  std::cout << "screenrows: " << screenrows << "\r\n";
  std::cout << "screencols: " << screencols << "\r\n";

  cx = cy = 0;

  std::cout << "TerminalModel created\r\n";

}

TerminalModel::~TerminalModel() {
  disableRawMode();

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
#pragma once
#include "../view/TerminalView.h"
#include "../model/TerminalModel.h"
#include <istream>
#include <unistd.h>

class TerminalController {
public:
  TerminalController(TerminalView *view, TerminalModel *model); 
  ~TerminalController();
  void runEditor();

private:
  TerminalView *view;
  TerminalModel *model;
  bool processKeypress();
  char readKey();
};

[[nodiscard]] constexpr int getCtrlKey(char k) { return k & 0x1f; }

TerminalController::TerminalController(TerminalView *_view, TerminalModel *_model) 
  : view(_view), model(_model) {  view->clearScreenAndCursor(); }

TerminalController::~TerminalController() { std::cout << "TerminalController destroyed\r\n"; }

char TerminalController::readKey() {
  ssize_t nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) exit(1);
  }
  return c;
}


bool TerminalController::processKeypress() {
  char c = readKey();

  switch (int(c)) {  
    case getCtrlKey('q'): 
      std::cout << "CTRLQ\r\n"; 
      return false;
    case 'w':
    case 's':
    case 'a':
    case 'd':
      model->editorMoveCursor(c);
      return true;
  }
  return -1;
}

void TerminalController::runEditor() {
  do {
    view->refreshScreen();
  } while (processKeypress());
}

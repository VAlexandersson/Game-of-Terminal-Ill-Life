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
  int processKeypress();
  char readKey();

};

[[nodiscard]] constexpr int getCtrlKey(char k) { return k & 0x1f; }

TerminalController::TerminalController(TerminalView *view, TerminalModel *model) {
  this->view = view;
  this->model = model;

  std::cout << "TerminalController created\r\n";
  view->clearScreenAndCursor();
  view->drawRow(model->getScreenRows());
}

TerminalController::~TerminalController() {
  std::cout << "TerminalController destroyed\r\n";
}

char TerminalController::readKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) exit(1);
  }
  return c;
}

int TerminalController::processKeypress() {
  int c = readKey();

  std::cout << "c: " << c << "\r\n";

  switch (c) {  
    case getCtrlKey('q'): 
      std::cout << "CTRLQ\r\n"; 
      return 1;
    case 'w':
    case 's':
    case 'a':
    case 'd':
      model->editorMoveCursor(c);
      break;
  }
  return -1;
}

void TerminalController::runEditor() {
  while (processKeypress() != 1)  {
  }
}

#pragma once
#include <termios.h>
#include <iostream>
#include "../model/TerminalModel.h"

class TerminalView {
public:
  TerminalModel *model;

  TerminalView(TerminalModel *model);
  ~TerminalView();

  void clearScreenAndCursor();
  void DrawRows();
  void refreshScreen();
private:
  std::string buffer;

};

TerminalView::TerminalView(TerminalModel *_model) : model(_model) { }
TerminalView::~TerminalView() { }

void TerminalView::clearScreenAndCursor() {
  std::cout << "\x1b[2J"; // clear screen
  std::cout << "\x1b[H"; // move cursor to top left
}


void TerminalView::refreshScreen() {
  this->buffer.append("\x1b[?25l");
  this->buffer.append("\x1b[H");

  this->DrawRows();

  this->buffer.append("\x1b[" + std::to_string(model->getCy()+1) + ";" + std::to_string(model->getCx()+1) + "H");
  this->buffer.append("\x1b[?25h");

  std::cout << buffer;
  std::cout.flush();
  this->buffer.clear();
}

void TerminalView::DrawRows() {
  for (size_t y = 0; y < model->getScreenRows(); y++) {
    if (y == model->getScreenRows() / 3) {
      std::string welcome = "GOL -- version 0.0.1";
      if (welcome.size() > model->getScreenCols()) {
        welcome = welcome.substr(0, model->getScreenCols());
      }
      size_t padding = (model->getScreenCols() - welcome.size()) / 2;
      if (padding) {
        buffer.append("~");
        padding--;
      }
      while (padding--) { buffer.append(" "); }
      buffer.append(welcome);
    }
    else { buffer.append("~"); }
    buffer.append("\x1b[K");
    if (y < model->getScreenRows() - 1) { buffer.append("\r\n"); }
  }
}

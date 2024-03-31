#pragma once
#include <termios.h>
#include <iostream>

class TerminalView {
public:
  TerminalView();
  ~TerminalView();

  void clearScreenAndCursor();

  void drawRow(const int& rows);
  void drawStatusBar();

  void drawMessageBar();

private:
};

TerminalView::TerminalView() {
  std::cout << "TerminalView created\r\n";
}

TerminalView::~TerminalView() {
  std::cout << "TerminalView destroyed\r\n";
}

void TerminalView::clearScreenAndCursor() {
  // write(STDOUT_FILENO, "\x1b[2J", 4); // clear screen
  // write(STDOUT_FILENO, "\x1b[H", 3); // move cursor to top left
  std::cout << "\x1b[2J"; // clear screen
  std::cout << "\x1b[H"; // move cursor to top left
}

void TerminalView::drawRow(const int& rows) {
  for (int i = 0; i < rows; i++)
    std::cout << "~\r\n";
}

void TerminalView::drawStatusBar() {
  std::cout << "status bar\r\n";
}

void TerminalView::drawMessageBar() {
  std::cout << "message bar\r\n";
}
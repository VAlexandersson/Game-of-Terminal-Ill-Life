#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>

namespace gol {

class Terminal {
private:
  termios origTermios;

public:
  Terminal();
  ~Terminal();
  void enableRawMode();
  void disableRawMode();
};

}
#endif // TERMINAL_H
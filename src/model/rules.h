
#ifndef RULES_H
#define RULES_H

#include "grid.h"

class Rules {
public:
  static int countAliveNeighbors(const Grid& grid, int x, int y);
  static bool determineNextState(const Grid& grid, int x, int y);
};

#endif
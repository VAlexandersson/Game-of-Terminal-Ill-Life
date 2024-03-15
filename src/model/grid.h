
#ifndef GRID_H
#define GRID_H

#include <vector>

class Grid {
public:
  Grid(int width, int height);
  bool getCellState(int x, int y) const;
  void setCellState(int x, int y, bool state);
  void updateGrid();
private:
  int width;
  int height;
  std::vector<std::vector<bool>> grid;
};

#endif // GRID_H
#include "TetMode.hpp"
#include <iostream>

void TetMode::additional_init() {

  gameboard[0][0] = 1;
  gameboard[0][1] = 1;

  srand(time(NULL));

  gen_rand_tile();

}

void TetMode::step_increment() {
  // show_board();
  std::cout << "!\n";
}

bool TetMode::adjacent_to_tile(int x, int y) {
  bool adj_to_1 = false;
  for(uint i=0; i<8; i+=2) {
    int difx = abs(x - active_tile[i]);
    int dify = abs(y - active_tile[i+1]);
    if (difx + dify == 0) return false; // duplicate
    if (difx + dify == 1) adj_to_1 = true;
  }
  return adj_to_1;
}

void TetMode::gen_rand_tile() {
  // first generate a tile within [0,4), [0,4)
  active_tile = vec1D(8, -2);

  active_tile[0] = rand() % 4;
  active_tile[1] = rand() % 4;

  for (uint i=2; i<8; i+=2) {

    while(true) {
      uint x = rand() % 4;
      uint y = rand() % 4;
      if (adjacent_to_tile(x,y)) {
        active_tile[i] = x;
        active_tile[i+1] = y;
        break;
      }
    }

  }

  // then move it into center of gameboard
  for (uint i=0; i<active_tile.size(); i++) {
    active_tile[i] += (board_size / 2) - 2;
  }

}

// ---- miscellaneous helpers ----
  bool adjacent_to_tile(int x, int y);

void TetMode::show_board() {
  for (uint i=0; i<gameboard.size(); i++) {
    for (uint j=0; j<gameboard.size(); j++) {
      std::cout << gameboard[j][i] << " ";
    }
    std::cout << std::endl;
  }
}

void TetMode::show_vector(vec1D& vec) {
  for(uint i=0; i<vec.size(); i++) {
    std::cout << vec[i] << ", ";
  }
  std::cout << std::endl;
}

#include "TetMode.hpp"
#include <iostream>

void TetMode::additional_init() {

  gameboard[10][4] = 1;

  for (int i=0; i<board_size; i++) {
    gameboard[i][0] = 1;
    gameboard[i][1] = 1;
    gameboard[i][2] = 1;
  }
  gameboard[4][2] = 0;
  gameboard[8][0] = 0;

  srand(time(NULL));

  gen_rand_tile();

}

void TetMode::step_increment() {
  if (!on_ground()) move_down();
  if (step_count == 2) clear_filled_rows();
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

void TetMode::move_down() {
  for(uint j=1; j<8; j+=2) {
    active_tile[j] -= 1;
  }
}

bool TetMode::on_ground() {
  for(uint i=0; i<8; i+=2) {
    int x = active_tile[i];
    int y = active_tile[i+1];
    if (y == 0) return true;
    if (gameboard[x][y-1]>0) return true;
  }
  return false;
}

bool TetMode::clear_filled_rows() {

  auto get_first_filled_row = [&]() {
    for (int y=0; y<board_size; y++) {
      uint x_cnt = 0;
      for (uint x=0; x<board_size; x++) {
        if (gameboard[x][y]>0) x_cnt++;
      }
      if (x_cnt == board_size) return y;
    }
    return -1;
  };

  bool has_filled_rows = false;
  int first_filled_row_y;
  while ( (first_filled_row_y=get_first_filled_row()) >= 0 ) {
    has_filled_rows = true;

    // remove this first_filled_row
    for (int x=0; x<board_size; x++) {

      // for each column, first remove the tile on filled row
      gameboard[x][first_filled_row_y] = 0;

      int y = first_filled_row_y + 1;
      assert(y>0);
      while (true) { // for all the ones stacking on top, move down by 1
        // end condition: 
        if (y >= board_size || gameboard[x][y] == 0) break;
        gameboard[x][y-1] = gameboard[x][y]; // move y down to y-1
        gameboard[x][y] = 0; // clear where y was
        y++; // now go look at the one above it.
      }
      
    }
    
  }
  return has_filled_rows;
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

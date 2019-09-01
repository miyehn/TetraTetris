#include "TetMode.hpp"
#include <iostream>

void TetMode::additional_init() {

  gen_rand_tile();

}

void TetMode::step_increment() {
  if (on_ground()) {
    inactivate_tile();
  } else if (!has_tile_active) {
    if (need_clear_check) {
      if (!clear_filled_rows()) gen_rand_tile();
    } else
      gen_rand_tile();
  } else {
    move_down();
  }
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

  has_tile_active = true;

}

void TetMode::move_down() {
  for(uint j=1; j<8; j+=2) {
    active_tile[j] -= 1;
  }
}

bool TetMode::on_ground() {
  if (!has_tile_active) return false;
  for(uint i=0; i<8; i+=2) {
    int x = active_tile[i];
    int y = active_tile[i+1];
    if (y == 0) return true;
    if (gameboard[x][y-1]>0) return true;
  }
  return false;
}

bool TetMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
  if (evt.type == SDL_KEYUP) {
    SDL_Keycode key = evt.key.keysym.sym;
    if (key == SDLK_LEFT) {
      rotate_board(1);
      return true;
    } else if (key == SDLK_RIGHT) {
      rotate_board(-1);
      return true;
    }
  }
  return false;
}

void TetMode::inactivate_tile() {
  for (int i=0; i<8; i+=2) {
    int x = active_tile[i];
    int y = active_tile[i+1];
    gameboard[x][y] = 1;
  }
  has_tile_active = false;
  need_clear_check = true;
}

void TetMode::rotate_board(int dir) {
  // create an alternate, rotated board first
  vec2D newboard = vec2D( board_size, vec1D(board_size, 0) );
  if (dir > 0) { // ccw 90 degrees
    for (int x=0; x<board_size; x++) {
      for (int y=0; y<board_size; y++) {
        int newx = board_size - 1 - y;
        int newy = x;
        newboard[newx][newy] = gameboard[x][y];
      }
    }
  } else if (dir < 0) {
    for (int x=0; x<board_size; x++) {
      for (int y=0; y<board_size; y++) {
        int newx = y;
        int newy = board_size - 1 - x;
        newboard[newx][newy] = gameboard[x][y];
      }
    }
  } else { return; }

  // check if rotated board is valid (does not overlap with active tile)
  bool valid_flag = true;
  for (int i=0; i<8; i+=2) {
    int x = active_tile[i];
    int y = active_tile[i+1];
    if (newboard[x][y] > 0) {
      valid_flag = false;
      break;
    }
  }

  if (valid_flag) gameboard = newboard;
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

      // find ceiling height
      int ceiling_height = board_size;
      while (gameboard[x][ceiling_height-1]>0) ceiling_height--;

      // for each column, remove the tile on filled row
      gameboard[x][first_filled_row_y] = 0;

      int y = first_filled_row_y + 1;
      assert(y>0);
      while (true) { // for all the ones stacking on top, move down by 1
        // end condition: 
        if (y >= ceiling_height) break;
        gameboard[x][y-1] = gameboard[x][y]; // move y down to y-1
        gameboard[x][y] = 0; // clear where y was
        y++; // now go look at the one above it.
      }
      
    }
  }
  need_clear_check = false;
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

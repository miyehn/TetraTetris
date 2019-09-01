#include "TetMode.hpp"
#include <iostream>

void TetMode::init_game() {

  // initialize game state
  board_size = 24;
  score = 0;
  step_count = 0;
  timestep = 0.5;
  gameboard = vec2D( board_size, vec1D(board_size, 0) );
  // init to all -2 for gen_rand_tile() correctness
  active_tile = vec1D(8, -2); // { (x,y), (x,y), (x,y), (x,y) },

  gameover = false;
  has_tile_active = false;
  rotatable = true;
  need_clear_check = false;

  // init other states
  court_radius = glm::vec2((float)board_size/2.0f, (float)board_size/2.0f);
  tile_radius = glm::vec2(0.45f, 0.45f);
  score_radius = glm::vec2(0.15f, 0.15f);
  
  gen_rand_tile();

}

void TetMode::step_increment() {
  if (gameover) return;

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

  // check if it overlaps with the board. If so, game ends
  for (uint i=0; i<8; i+=2) {
    int x = active_tile[i];
    int y = active_tile[i+1];
    if (gameboard[x][y]>0) {
      endgame();
      return;
    }
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
    if (gameover) {
      init_game();
      return true;
    }
    SDL_Keycode key = evt.key.keysym.sym;
    if (key == SDLK_LEFT) {
      rotate_board(1);
      return true;
    } else if (key == SDLK_RIGHT) {
      rotate_board(-1);
      return true;
    } else if (key == SDLK_DOWN) {
      while (!on_ground()) {
        move_down();
      }
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
  if (!rotatable) return;
  // create an alternate, rotated board first
  vec2D newboard = vec2D( board_size, vec1D(board_size, 0) );
  vec1D newtile = vec1D(8, 0);
  if (dir > 0) { // ccw 90 degrees
    for (int x=0; x<board_size; x++) {
      for (int y=0; y<board_size; y++) {
        int newx = board_size - 1 - y;
        int newy = x;
        newboard[newx][newy] = gameboard[x][y];
      }
    }
    for (int i=0; i<8; i+=2) {
      int x = active_tile[i];
      int y = active_tile[i+1];
      newtile[i] = board_size - 1 - y;
      newtile[i+1] = x;
    }
  } else if (dir < 0) {
    for (int x=0; x<board_size; x++) {
      for (int y=0; y<board_size; y++) {
        int newx = y;
        int newy = board_size - 1 - x;
        newboard[newx][newy] = gameboard[x][y];
      }
    }
    for (int i=0; i<8; i+=2) {
      int x = active_tile[i];
      int y = active_tile[i+1];
      newtile[i] = y;
      newtile[i+1] = board_size - 1 - x;
    }
  } else { return; }

  active_tile = newtile;
  gameboard = newboard;
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
    score++;
    timestep = std::max(0.2f, timestep-0.05f);
  }
  need_clear_check = false;
  return has_filled_rows;
}

void TetMode::endgame() {
  vec1D text = {
    0,4, 0,5, 1,4, 1,6, 2,4, 2,5, // G
    4,4, 4,5, 5,5, 5,6, 6,4, 6,5, 6,6, // A
    8,4, 8,5, 9,6, 10,4, 10,5, 10,6, 11,4, 11,5, 11,6, // M
    13,4, 13,5, 14,4, 14,5, 14,6, 15,4, 15,6, // E
    1,0, 1,1, 2,0, 2,2, 3,0, 3,1, 3,2, // O
    5,1, 6,0, 7,1, 7,2, // V
    9,0, 9,1, 10,0, 10,1, 10,2, 11,0, 11,2, // E
    13,0, 13,1, 14,2 // R
  };

  gameboard = vec2D( board_size, vec1D(board_size, 0) );
  for (int i=0; i<text.size(); i+=2) {
    int x = text[i];
    int y = text[i+1];
    gameboard[x+4][y+8] = 1;
  }

  has_tile_active = false;
  rotatable = false;
  gameover = true;
}

// ---- miscellaneous helpers ----

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

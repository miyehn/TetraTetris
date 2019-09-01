#include "ColorTextureProgram.hpp"

#include "Mode.hpp"
#include "GL.hpp"

#include <vector>
#include <deque>

/*
 * TetMode is a game mode that implements a modified version of tetris.
 */

struct TetMode : Mode {
  TetMode();
  virtual ~TetMode();

  // convenience typedefs
  typedef uint32_t uint; 
  typedef std::vector<int> vec1D;
  typedef std::vector<vec1D> vec2D;

  // functions called by main loop:
  virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
  virtual void update(float elapsed) override;
  virtual void draw(glm::uvec2 const &drawable_size) override;

  //----- tet-specific functions and game state -----
  void init_game();
  void step_increment();

  void gen_rand_tile(); bool adjacent_to_tile(int x, int y); // helper to gen_rand_tile
  void move_down();
  bool on_ground(); 
  bool clear_filled_rows();
  void rotate_board(int dir); // only rotate if feasible. dir=1: ccw, dir=-1: cw
  void inactivate_tile();

  void endgame();

  uint board_size;
  uint score;
  uint step_count;
  float timestep;
  vec2D gameboard;
  vec1D active_tile; // vec1D of 8 ints, { (x,y), (x,y), (x,y), (x,y) },

  // flags
  bool gameover;
  bool has_tile_active;
  bool rotatable;
  bool need_clear_check;

  // miscellaneous helpers
  void show_board();
  void show_vector(vec1D& vec);

  //----- other generic game state -----
  
  glm::vec2 court_radius;
  glm::vec2 tile_radius;
  glm::vec2 score_radius;
  float time_elapsed = 0.0f;

  //----- view-related constants -----
  const float wall_radius = 0.05f;
  const float padding = 0.14f; //padding between outside of walls and edge of window

  //----- opengl assets / helpers ------

  //draw functions will work on vectors of vertices, defined as follows:
  struct Vertex {
    Vertex(glm::vec3 const &Position_, glm::u8vec4 const &Color_, glm::vec2 const &TexCoord_) :
      Position(Position_), Color(Color_), TexCoord(TexCoord_) { }
    glm::vec3 Position;
    glm::u8vec4 Color;
    glm::vec2 TexCoord;
  };
  static_assert(sizeof(Vertex) == 4*3 + 1*4 + 4*2, "TetMode::Vertex should be packed");

  //Shader program that draws transformed, vertices tinted with vertex colors:
  ColorTextureProgram color_texture_program;

  //Buffer used to hold vertex data during drawing:
  GLuint vertex_buffer = 0;

  //Vertex Array Object that maps buffer locations to color_texture_program attribute locations:
  GLuint vertex_buffer_for_color_texture_program = 0;

  //Solid white texture:
  GLuint white_tex = 0;

  //matrix that maps from clip coordinates to court-space coordinates:
  glm::mat3x2 clip_to_court = glm::mat3x2(1.0f);
  // computed in draw() as the inverse of OBJECT_TO_CLIP
  // (stored here so that the mouse handling code can use it to position the paddle)

};

#include "ColorTextureProgram.hpp"

#include "Mode.hpp"
#include "GL.hpp"

#include <vector>
#include <deque>

/*
 * PongMode is a game mode that implements a single-player game of Pong.
 */

struct TetMode : Mode {
  // convenience typedefs
  typedef uint32_t uint; 
  typedef std::vector<int> vec1D;
  typedef std::vector<vec1D> vec2D;

  TetMode();
  virtual ~TetMode();

  // functions called by main loop:
  virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
  virtual void update(float elapsed) override;
  virtual void draw(glm::uvec2 const &drawable_size) override;

  //----- tet-specific functions -----
  void step_increment();
  void show_board();

  bool on_ground();
  bool clear_filled_rows();

  //----- game state -----
  
  uint board_size = 24;
  glm::vec2 court_radius = glm::vec2((float)board_size/2.0f, (float)board_size/2.0f);
  glm::vec2 tile_radius = glm::vec2(0.45f, 0.45f);

  uint32_t score = 24;

  const float timestep = 0.5f;
  float time_elapsed = 0.0f;

  vec2D gameboard = vec2D( board_size, vec1D(board_size, 0) );
  vec1D active_tile = vec1D(8, 0); // { (x,y), (x,y), (x,y), (x,y) }
  bool has_tile_active = false;
  bool rotatable = false;

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

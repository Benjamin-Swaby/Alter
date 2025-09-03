#pragma once

#include <cctype>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <functional>
#include <utility>

#include "gap_buffer.hpp"
#include "file.hpp"


namespace terminal {


  typedef struct terminal_meta {
    struct termios orig_termios;
    size_t columns = 80;
    size_t rows = 30;
    size_t cx, cy = 0;
  }terminal_meta_t;
  

  void cleanup_terminal();
  void setup_terminal();
  void clear_terminal();
  void send_cursor_home();
  void enter_alternative_screen();
  void exit_alternative_screen();
  void draw_rows();
  void poll_terminal_size();
  void set_cursor_position(int x, int y);
  void clear_append();
  void put_char(char c);
  void put_str(const char* c, int N);
  void put_line(const char* c, int N);
  void put_buffer(buffers::Gap_Buffer<GAP_BUFFER_SIZE>* gb);
  void put_line_obj(files::Editor_File::Line* l); // same as put_buffer but with wrapping.
  std::pair<size_t, size_t> get_terminal_size();
  std::pair<size_t, size_t> get_cursor_location();
  std::function<char()> get_input();


  }

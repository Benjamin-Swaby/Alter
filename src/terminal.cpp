#include "terminal.hpp"
#include "file.hpp"
#include "gap_buffer.hpp"
#include <cstdio>
#include <functional>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>


namespace terminal {


  terminal_meta_t tconf;

  const int append_buffer_size = 4096;

  struct {
    char *b;
    int len = 0;
    int free = append_buffer_size;
    int pushed_lines = 0;
  } append_buffer;


  void append_buffer_push(const char* data, int len) {
    if(append_buffer.free >= len) {
      memcpy(&append_buffer.b[append_buffer.len], data, len);
      append_buffer.len += len;
      append_buffer.free -= len;
    }
  };

  void append_buffer_clear() {
    append_buffer.len = 0;
    append_buffer.pushed_lines = 0;
    append_buffer.free = append_buffer_size;

    // manually wiping the append buffer content.
    for(int i = 0; i < append_buffer_size; i++) {
      append_buffer.b[i] = ' '; 
    }
    
  }
  
  
  void cleanup_terminal() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &tconf.orig_termios);
    terminal::exit_alternative_screen();
    
  }
  
  
  void setup_terminal() {

    terminal::enter_alternative_screen();
    
    tcgetattr(STDIN_FILENO, &tconf.orig_termios);
    atexit(cleanup_terminal);
    
    struct termios raw = tconf.orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST); // No Output Processing
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    // allocate screen buffer. Big buffer >> small allocations
    append_buffer.b = new char[append_buffer_size];


    poll_terminal_size();
    
  }
  
  void clear_terminal() {

    append_buffer_push("\x1b[2J", 4);
    //    write(STDOUT_FILENO, "\x1b[2J", 4);
  }
  
  
  void send_cursor_home() {
    write(STDOUT_FILENO, "\x1b[H", 3);
  }


  void clear_append() {
    append_buffer_clear();
  }
  

  void draw_rows() {
    
    send_cursor_home();
    
      
    // Draw Cursor at pasition in tconf
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", tconf.cy + 1, tconf.cx + 1);
    append_buffer_push(buf, strlen(buf));
   
    write(STDOUT_FILENO, append_buffer.b, append_buffer.len);
    
    append_buffer_clear();
    
  }



  void put_char(char c) {
    append_buffer_push(&c, 1);
  }

  void put_str(const char* c, int N) {
    append_buffer_push(c, N);
  }

  void put_line(const char* c, int N) {
    append_buffer_push(c, N);

    append_buffer_push("\r\n", 2);    
    
  }

  void put_buffer(buffers::Gap_Buffer<GAP_BUFFER_SIZE>* gb) {

    for(auto c : *gb) {      
      append_buffer_push(&c, 1);
    }

    append_buffer_push("\r\n", 2);
    
  }


  void put_line_obj(files::Editor_File::Line *l) {
    const auto col = tconf.columns - 5;
    const auto strlen = l->buf->get_strlen();    
    const std::string wrap_str ="\r\n\033[37;44m^^^^\033[0m ";    

    int chars_written = 0;
    l->wrapping = 0;
    
    for(auto c : *l->buf) {
      if(chars_written % col == 0 && chars_written != 0) {
        append_buffer_push(wrap_str.c_str(), wrap_str.length());
        l->wrapping++;
      }
      
      append_buffer_push(&c, 1);
      chars_written++;
    }

    append_buffer_push("\r\n", 2);

    
  }



  void set_cursor_position(int x, int y) {
    tconf.cx = (x <= tconf.columns && x >= 0) ? x : tconf.cx;
    tconf.cy = (y <= tconf.rows && y >= 0 ) ? y : tconf.cy;
  }


  void enter_alternative_screen() {    
    write(STDOUT_FILENO, "\x1b[?1049h", 8); // Switch to alternate buffer
  }

  void exit_alternative_screen() {
    write(STDOUT_FILENO, "\x1b[?1049l", 8); // Return to normal buffer
  }


  void poll_terminal_size() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1
        || ws.ws_col != 0) {
      
      tconf.columns = ws.ws_col;
      tconf.rows = ws.ws_row;
    }
  }


  std::pair<size_t, size_t> get_terminal_size() {
    return {tconf.columns, tconf.rows};
  }

  std::pair<size_t, size_t> get_cursor_location() {
    return {tconf.cx, tconf.cy};
  }
 

  std::function<char()> get_input() {
    return []() -> char {
      char c;
      read(STDIN_FILENO, &c, 1);
      return c;
    };
  }
  


  

  
}

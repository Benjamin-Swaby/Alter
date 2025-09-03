#include "editor.hpp"
#include "file.hpp"
#include "terminal.hpp"
#include <cstdio>
#include <format>

namespace editor {

  TUI_Editor::TUI_Editor() {

    terminal::setup_terminal();
    terminal::send_cursor_home();

  }


  // offset of other UI elements, i.e line numbers
  // status bars.
  const int x_offset = 5;
  const int y_offset = 1;
  const int y_offset_b = 1;
  


  int wrapping_offset = 0;

  // this function aims to sync the tui cursor and the editor cursor.
  // it queries the open_file cursor position and curent context.
  void TUI_Editor::sync_cursors() {

    auto x = this->openFile->context->buf->getCursorPosition();
    auto line = this->openFile->current_context_line - this->f->start_line_number;
    
    terminal::set_cursor_position(x + x_offset, line + y_offset + wrapping_offset);
    
    
  }
  

  coord_t TUI_Editor::get_display_character_dim() {

    terminal::poll_terminal_size();
    auto ts = terminal::get_terminal_size();
    
    return {ts.second, ts.first};
    
    
  }

  coord_t TUI_Editor::get_cursor_position() {
    auto p = terminal::get_cursor_location();
    return {p.second, p.first};
  }
  

  inline void TUI_Editor::set_cursor_position(coord_t pos) {
    terminal::set_cursor_position(pos.column, pos.row);
  }


  void TUI_Editor::home() {
    this->openFile->context->buf->put_cursor_home();
  }

  void TUI_Editor::end() {
    this->openFile->context->buf->put_cursor_end();
  }
  


  void TUI_Editor::put_status_line(std::string msg) {

    this->status_line = "";
    this->status_line += msg;
    
  }
  
  void TUI_Editor::next_line() {

    auto cpos = terminal::get_cursor_location();


    if(cpos.second == terminal::get_terminal_size().second - 2)
      return;

    if(!this->openFile->has_next()) {
      return;
    }


    wrapping_offset += this->openFile->context->wrapping;
    this->openFile->next_line();

    auto strlen = this->openFile->context->buf->strlen;

    
    if(strlen < cpos.first) {
      this->openFile->context->buf->put_cursor_end();
    }

    if(this->openFile->current_context_line == this->f->end_line_number - 1)
      f->scroll_down(1);

  }


  void TUI_Editor::prev_line() {

    auto cpos = terminal::get_cursor_location();
    wrapping_offset -= this->openFile->context->wrapping;
    
    this->openFile->prev_line();

    auto strlen = this->openFile->context->buf->strlen;
    
    if(strlen < cpos.first) {
      this->openFile->context->buf->put_cursor_end();
    } else {
      // put the cursor in the right column
      for(int i = 0; i < cpos.first; i++) {
        this->openFile->forward();
      }
      
    }

    if(this->openFile->current_context_line == this->f->start_line_number && this->openFile->current_context_line != 0)
      this->f->scroll_up(1);
    
  }
  
  void TUI_Editor::forward() {

    auto strlen = this->openFile->context->buf->strlen;
    auto cpos = terminal::get_cursor_location();

    if(cpos.first < strlen) {
      //terminal::set_cursor_position(cpos.first + 1, cpos.second);
      this->openFile->forward();
    }
   
  }
  
  void TUI_Editor::backward() {
    auto cpos = terminal::get_cursor_location();
    //terminal::set_cursor_position(cpos.first - 1, cpos.second);
    this->openFile->backward();
  }


  void TUI_Editor::delete_char() {
    auto cpos = terminal::get_cursor_location();

    if(this->openFile->context->buf->getCursorPosition() > 0) {
      //terminal::set_cursor_position(cpos.first - 1, cpos.second);
      this->openFile->context->buf->free();
    } else {
      this->f->remove_line_hook();
      this->openFile->remove_line();
      this->f->scroll_up(1);
      this->sync_cursors();
      

      auto strlen = this->openFile->context->buf->strlen;
      
      //terminal::set_cursor_position(strlen, cpos.second - 1);
      this->openFile->context->buf->put_cursor_end();

      
    }
   
  }


 
  void TUI_Editor::new_line() {
    auto cpos = terminal::get_cursor_location();
    
    this->openFile->new_line();
    this->next_line();    
    this->f->new_line_hook(false);

    if(this->openFile->current_context_line >= this->f->size)
      this->f->scroll_down(1);
    
  }

  


  void put_modline(std::string mod_line) {

    std::string start = "\033[37;44m";
    std::string reset = "\033[0m";
    
    int padding = terminal::get_terminal_size().first - mod_line.length();
    start += mod_line;
    for(int i = 0; i < padding; i++) {
      start += " "; 
    }
    start += reset;
    terminal::put_str(start.c_str(), start.length());
    
  }



  std::string TUI_Editor::get_user_input(std::string prompt) {


    char* buf = new char[64];
    int len = 0;
    
    auto get_char = terminal::get_input();

    char cmd;
    
    while(1) {

      draw();

      terminal::put_str(prompt.c_str(), prompt.length());
      terminal::put_str(buf, len);
      terminal::draw_rows();

      cmd = get_char();
      
      if(cmd == 13) {
        break;
      }
      
      if(cmd == 127 && len > 0) {
        len--;
        continue;
      }

      if(cmd != 0) {
        buf[len] = cmd;
        len++;
      }
      
    }

    
    
    return std::string(buf, buf+len);
    
  }
  


  void TUI_Editor::draw() {
    terminal::clear_terminal();
    put_modline(mod_line);
    f->display();      
    this->sync_cursors();
  }


  
  
  void TUI_Editor::run() {

    auto get_char = terminal::get_input();

    auto rows = terminal::get_terminal_size().second - 3;
       
    this->f = new Frame(this->openFile->head, rows, 0);

    
    while (1) {
      terminal::poll_terminal_size();
      draw();
      terminal::put_str(this->status_line.c_str(), this->status_line.length());

      if(!status_persist)
        this->status_line = "";
      
      terminal::draw_rows();
      
      char c = get_char();


      this->mod_line = "";
      int k = 1;
      for(const auto& [key, value] : this->Files) {
        
        //        if(key == this->openFile->filename)
        //  this->mod_line += "\033[0m\033[37;41m";
        
        this->mod_line += std::format(" [{:02}]{}", k, key);

        //if(key == this->openFile->filename)
        //  this->mod_line += "\033[0m\033[37;44m"; 
        
        k++;
      }
                 
      // consult keymap
      if(this->keymap.contains(c)) {
        this->keymap[c]();
        continue;
      }
           

      // tab
      if(c == 9) {
        this->openFile->write_char(' ');
        this->openFile->write_char(' ');
        this->openFile->write_char(' ');
        this->openFile->write_char(' ');
        continue;
      }
      
      if(c == 17) {
        break;
      }


      if(c != 0 && c >= 32 && c <= 126) {
        
        this->openFile->write_char(c); // write a character to the buffer
        auto cpos = terminal::get_cursor_location(); // advance the cursor
        
      }
      
    }
    
  }



  void TUI_Editor::open_file(std::string path) {
    Editor::open_file(path);
    auto rows = terminal::get_terminal_size().second - 3;
    delete this->f;
    this->f = new Frame(this->openFile->head, rows, 0);
  }


  void TUI_Editor::switch_buffer(int index) {
    Editor::switch_buffer(index);
    auto rows = terminal::get_terminal_size().second - 3;
    delete this->f;
    this->f = new Frame(this->openFile->head, rows, 0);
  }
  
  
}

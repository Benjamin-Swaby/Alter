#include "editor.hpp"
#include "terminal.hpp"


namespace editor {


  Frame::Frame(files::Editor_File::Line* ctx, size_t N, int ctx_line) {

    start_line_number = ctx_line;
    start = ctx;
    end = start;
    
    int i = 1;
    for(i; i <= N; i++) {
      if(end->Next == nullptr)
        break;
      end = end->Next;
    }
    
    size = i;

    end_line_number = ctx_line + size;
       
  }


  void Frame::scroll_down(int lines) {
    
    for(int i = 0; i < lines; i++) {
      if(end->Next == nullptr)
        break;

      if(start == nullptr)
        break;
      
      start = start->Next;
      start_line_number++;
      
    }
    
  }


  void Frame::scroll_up(int lines) {

    if(start == nullptr || end == nullptr)
      return;
    
    for(int i = 0; i < lines; i++) {

      if(start->Prev == nullptr)
        break;

      start = start->Prev;
      start_line_number--;
      
    }
  }

  void Frame::display() {
        
    int i = start_line_number;

    for(auto ptr = start; ptr != nullptr && i < (size + start_line_number); ptr = ptr->Next, i++) {
      auto line_num = std::format("\033[37;44m{:04}\033[0m ", i);      
      terminal::put_str(line_num.c_str(), line_num.length());
      terminal::put_line_obj(ptr);
      this->end = ptr;
      this->end_line_number = i;
    }
            
  }


  void Frame::new_line_hook(bool last_line) {
    // maintain frame size by bringing the end back one IF
    // the frame fills the screen

    auto rows = terminal::get_terminal_size().second;
      
    if(this->size < rows - 2) {
      size++;
    }
    
    
  }


  void Frame::remove_line_hook() {
    auto rows = terminal::get_terminal_size().second;


    if(this->start->Prev == nullptr)
      return;
    
    if(this->size < rows - 2) {
      this->size--;
    }
    
  }
  
  

  } // namespace editor


  

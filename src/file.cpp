#include "file.hpp"
#include "gap_buffer.hpp"

#include <fstream>

namespace files {



  Editor_File::Editor_File(std::string filename) {
    using std::fstream;

    fstream infile(filename);

    if(!infile.is_open()) {
 
      this->head = new Line("", 0);
      this->head->Next = nullptr;
      this->head->Prev = nullptr;
      this->context = this->head;
      this->tail = this->head;

      this->lines = 1;
      this->current_context_line = 0;
      this->filename = "*" + filename;
      
      return;
    }

    int lines = 0;

    Line* t_next;
    Line* t_prev = this->head;
    
    for (std::string s; std::getline(infile, s); ) {
      this->context = new Line(s.c_str(), s.length());

      if (lines == 0) [[unlikely]] {
        this->context->Prev = nullptr;
        this->head = this->context;
      } else {
        this->context->Prev = t_prev;
        t_prev->Next = this->context;
      }
     


      t_prev = this->context;
      this->context = this->context->Next;
      lines++;
    }

    this->tail = this->context;
    this->context = this->head;

    infile.close();

    this->filename = filename;
    this->lines = lines;
    this->current_context_line = 0;
    
  }


  Editor_File::~Editor_File() {
    auto togo = this->head->Next;
    
    for(this->context = this->head;
        togo != nullptr; ) {

      delete this->context;
      this->context = togo;
      togo = this->context->Next;
      
    }
    
  }



  void Editor_File::next_line() {
    auto togo = this->context->Next;
    if (togo != nullptr) {
      this->context = togo;
      this->current_context_line++;
    }
  }

  void Editor_File::prev_line() {
    auto togo = this->context->Prev;
    if (togo != nullptr) {
      this->context = togo;
      this->current_context_line--;
    }
    
    
  }

  void Editor_File::forward() {
    this->context->buf->increment_cursor();
  }

  void Editor_File::backward() {
    this->context->buf->decrement_cursor();
  }

  void Editor_File::write_char(char c) {

    // if the current buffer is full then
    // create a new one and link it.

    /**

       V------------p    
                    n ------>
       a            b
       n------------^
       p

       
       
     */
    
    if(this->context->buf->full()) [[unlikely]] {
      auto ctx_next = this->context->Next;

      // create a new node set the previous value and next according
      this->context->Next = new Line(&c, 1);
      this->context->Next->Prev = this->context;
      this->context->Next->Next = ctx_next;


      ctx_next->Prev = this->context->Next;

      // advance context into new buffer
      this->next_line();

      this->lines++;
      
    } else {
      this->context->buf->insert(c);
    }
    
    
  }


  void Editor_File::save_as(const char* path) {

    using std::ofstream;

    ofstream out;

    out.open(path);

    auto prev_context = this->context;
    
    this->context = this->head;
    while(this->context != nullptr) {

      for(auto c : *this->context->buf) {
        out << c;
      }
      
      this->context = this->context->Next;

      out << '\n';
      
    }


    this->context = prev_context;
    out.close();
    
  }


  void Editor_File::save() {
    this->save_as(this->filename.c_str());
  }


  void Editor_File::new_line() {

    auto ctx_next = this->context->Next;

    const char* empty = "";
    auto content = this->context->buf->get_post_gap();
    
    // create a new node set the previous value and next according
    this->context->Next =
      new Line(content.c_str(), content.length());

    this->context->buf->trim_post_gap();
    
    this->context->Next->Prev = this->context;
    this->context->Next->Next = ctx_next;
    

    if(ctx_next != nullptr) 
      ctx_next->Prev = this->context->Next;
    this->lines++;

    // You will need to manually advance onto the newline

    
    
  }


  void Editor_File::remove_line() {

    if(this->context == nullptr) {
      return;
    }
    
    // remove this->context.

    auto prev = this->context->Prev;

    if(prev == nullptr) {
      return;
    }
    
    auto next = this->context->Next;
    
    prev->buf->put_cursor_end();
    for(auto c : this->context->get_chars())
      prev->buf->insert(c);
    
    delete this->context;

    this->current_context_line--;

    prev->Next = next;


    if(next != nullptr) {
      next->Prev = prev;
      this->context = prev;
    } else {
      this->context = prev;
    }
       
  }
  
  
}

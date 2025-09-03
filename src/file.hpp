#pragma once

#include "gap_buffer.hpp"
#include <string>


#define GAP_BUFFER_SIZE 512

namespace files {
  
  /**

     Editor_File

     This is how a file is represented.

     Each file is a linked list of lines
     each line is either
     
   */
    class Editor_File {
    public:
      struct Line {
        
        buffers::Gap_Buffer<GAP_BUFFER_SIZE>* buf;
        Line* Next = nullptr;
        Line* Prev = nullptr;

        int wrapping = 0;

        
        Line(const char *b, unsigned int N) {
            buf = new buffers::Gap_Buffer<GAP_BUFFER_SIZE>();
            buf->load(b, N);
        }

        ~Line() {
          if(buf != nullptr) {
            delete buf;
          }          
        }


        std::string get_chars() {
          std::string r;
 
          r.reserve(GAP_BUFFER_SIZE);
          for(auto c : *buf) {
            r.push_back(c);
          }
         
          return r;
          
        }
        
        
      };


      Line* head; // First Line and Head of the files
      Line* tail; //  Tail of the linked list and last line
      Line* context; // Context = Line currently being looked at.
      
      std::string filename; // filename
      unsigned int size; // size of the file (realtime?)
      unsigned int lines; // number of lines in the file (realtime?)
      unsigned int current_context_line; // line number of the current context.
      
      
      Editor_File(std::string filename);
      ~Editor_File(); // make sure to follow the linked list and not leave floating objects


      void save();
      void save_as(const char* path);
      
      void write_char(char c);
      
      void next_line();
      void prev_line();
      void forward();
      void backward();


      void new_line();
      void remove_line();

      constexpr inline bool has_next() {
        return this->context->Next != nullptr;
      }

      constexpr inline bool has_prev() {
        return this->context->Prev != nullptr;
      }
      
    };
  
}

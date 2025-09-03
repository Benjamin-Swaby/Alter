
#pragma once

#include <string.h>

// TODO

#include <iostream>

/**

   Implement a gap buffer here.

   text must be able to be seamlessly read
   line by line using an iterator.

   inside details mustn't be revealed to the developer

 */


namespace buffers {


  template<const unsigned int SIZE = 512>
  class Gap_Buffer {
  private:
    char* buffer;   // ptr to the start of the buffer
    char* buffer_end; // end of the buffer
    char *gap_start; // gap start
    char *gap_end; // end of gap

    
    unsigned int gap = SIZE; // size of the gap
    
    /**

       
       [b, -, -  gs, -, -, -, ,ge,-, be]
       [a, b, c, d|,           e, f, g]
                  ^ cursor

      Size = 10
      Gap = 3
     */
    
    
  public:

    unsigned int strlen = 0;
    
    Gap_Buffer() {
      this->buffer = new char[SIZE];
      this->buffer_end = this->buffer + SIZE;
      this->gap_start = this->buffer;
      this->gap_end = this->buffer_end;
    }
    
    ~Gap_Buffer() {
      delete this->buffer;
    }

    

    bool load(char const* inbuffer, unsigned int size) {

      if (size > SIZE) [[unlikely]] {
        return false;
      }
     
     
      memcpy(this->buffer, inbuffer, size);
      
      this->gap -= size;
      this->strlen += size;
      this->gap_start = this->buffer + size + 1;

      /**
         1 2 3 4 5
         V V V V V
         a b c d e *
                   ^
       */


      this->put_cursor_home();
      
      return true;
      
    }


    void increment_cursor() {

      //          start       end
      // [         v           v      }  
      // [a, b, c, -, -, -. -. -, d, e]
      //
      // 1.1 : if start is the end of the buffer
      //       may read beyond the buffer. The buffer is therefore
      //       full or an error in setting the gap.
      // 1.2 : if the end is at the buffer end then the gap is
      //       as far as it can go left.

      if (this->gap_start == this->buffer_end)
        return;

      if (this->gap_end == this->buffer_end - 1)
        return;
      

      //         start        end
      // [         v           v      }
      // [a, b, c, -, -, -. -. -, d, e]
      //

      // 2 : swap the values of start and end.
      //   : The end doesn't /need/ to be updated it can be
      //     overwritten when needed.
      // 2.2: if the start == end it will perform a useless swap

      *this->gap_start = *(this->gap_end + 1);
      
      //         start        end
      // [         v           v      }
      // [a, b, c, d, -, -. -. -, d, e]
      //


      // 3 : increment pointers. it is not at the end
      this->gap_end++;
      this->gap_start++;
      
      //            start        end
      // [            v           V   }
      // [a, b, c, d, -, -. -. -, -, e]
      //
     
    }



    void decrement_cursor() {
      //         start        end
      // [         v           v      }
      // [a, b, c, -, -, -. -. -, d, e]
      //        ^----->>-->>---^

      
      //               start        end
      // [               v           v}
      // [a, b, c, d, e, -. -. -, -, -]
      //              ^----->>-->>---^

      
      if (this->gap_end  == this->buffer)
        return;

      if (this->gap_start == this->buffer)
        return;
      
      this->gap_start--; 

      // 2: swap end and start
      //    start is just considered gap and will
      //    be overwritten when needed.

      *this->gap_end = *this->gap_start;


      // 3: decrement end
      this->gap_end--;   
      
    }
   

    void print_with_gap() {

      for(auto *ptr = this->buffer; ptr != this->buffer_end; ptr++) {

       
        if(ptr >= this->gap_start && ptr <= this->gap_end) {
          std::cout << '*';
        } else {
          std::cout << *ptr;
        }
        
      }

      
    }


    void insert(char c) {

      if(this->gap_start == this->gap_end) {
        return;
      }

      if(this->gap_start == this->buffer_end) {
        return;
      }

      
      *this->gap_start = c;
      this->gap_start++;
      this->gap--;
      this->strlen++;

    }

    void free() {
      if(this->gap_start == this->buffer)
        return;
      this->gap_start--;

      this->gap++;
      this->strlen--;
    }


    void put_cursor_home() {


      /**
                     gap start      gap end
                        V             V
         a, b, c, d, e, * * * * * * ...
         a,b,c,d,e,*,*,*,*
         *,*,*,*,a,b,c,d,e
         
         
       */


      while(this->gap_start != this->buffer) {
        decrement_cursor();
      }
     
    }


    void put_cursor_end() {
      while(this->gap_end != this->buffer_end - 1) {
        increment_cursor();
      }
    }
    


    constexpr inline int getCursorPosition() {
      return this->gap_start- this->buffer;
    }
    
    constexpr inline char const* get_buffer_end() {
      return this->buffer + SIZE;
    }

    constexpr inline char const* get_buffer_start() {
      return this->buffer;
    }

    constexpr inline bool full() {
      return (this->gap_start == this->gap_end);
    }


    // returns everything after the cursor
    inline std::string get_post_gap() {
      return std::string(this->gap_end + 1, this->buffer_end);
    }

    // trims everything after the cursor.
    void trim_post_gap() {
      this->gap_end = this->buffer_end - 1;
    }


    // getstrlen
    inline int get_strlen() {
      return (this->gap_start - this->buffer)
        + (this->buffer_end - this->gap_end);
    }


    struct iterator {
      Gap_Buffer<SIZE> *owner;
      char *ptr;
      
      char& operator*() const {return *ptr;}

      iterator& operator++() {
        ++ptr;
        if (ptr == owner->gap_start) ptr = owner->gap_end + 1;
        return *this;
       }

      iterator(Gap_Buffer<SIZE> *o, char* p) : owner(o), ptr(p) {
        if(ptr == owner->gap_start)
          ptr = owner->gap_end + 1;
      }



      bool operator!=(const iterator& other) const {return ptr != other.ptr;}
      
      
      
    };


    auto begin() {return iterator{this, this->buffer};}
    auto end() {return iterator{this, this->buffer_end};}
     
    
  };
  
  
}



    

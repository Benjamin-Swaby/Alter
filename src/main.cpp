#include <iostream>
#include <unistd.h>
#include "editor.hpp"
#include "file.hpp"
#include "gap_buffer.hpp"
#include "terminal.hpp"


int main(int argc, char **argv) {

  using std::cout, std::endl;


  cout << "[EDITOR] Version 0.0.0d" << endl;
  
  editor::TUI_Editor *te = new editor::TUI_Editor();  

  if(argc < 2)
    return 1;
  
  te->open_file(argv[1]);

  
  // down
  te->keymap[14] = [te]() {
    te->next_line();
  };


  // up
  te->keymap[16] = [te]() {
    te->prev_line();
  };

  // forward
  te->keymap[6] = [te]() {
    te->forward();
  };

  // backwards
  te->keymap[2] = [te]() {
    te->backward();
  };

  // backspace
  te->keymap[127] = [te]() {
    te->delete_char();
  };


  // enter
  te->keymap[13] = [te]() {
    te->new_line();
  };


  // ALT codes
  te->keymap[27] = [te]() {

    char cmd = terminal::get_input()();
    if(cmd == 'f') {
      auto filename = te->get_user_input("Open: ");          
      te->open_file(filename);
      
    } else if (cmd == 's') {
      auto filename = te->get_user_input("Save as: ");
      if(filename.length() > 1)
        te->save(filename);
      
    } else if (cmd <= 57 && cmd >= 49) { // numbers 1 -> 9
      te->switch_buffer(cmd - 48);
    }
    
    
  };


  // ctrl-e == end
  te->keymap[5] = [te]() {
    te->end();
  };


  // ctrl-a == home
  te->keymap[1] = [te]() {
    te->home();
  };


  // ctrl-s == save
  te->keymap[19] = [te, argv]() {
    te->save(argv[1]);
    te->put_status_line("Saved");
  };



  te->run();


}



#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

#include "file.hpp"
#include "terminal.hpp"
#include <format>

namespace editor {


  typedef struct coord {
    int row;
    int column;
  } coord_t;


  typedef std::unordered_map<char, std::function<void()>> keymap_t;
  
  
  class Editor {
  protected:
    files::Editor_File* openFile;
    std::unordered_map<std::string, files::Editor_File*> Files;
    bool cmd_mode = false;
    std::string mod_line;
    std::string status_line;
    bool status_persist = false;
    
  public:
    Editor() = default;
    
    ~Editor() = default;

    keymap_t keymap;

    virtual coord_t get_cursor_position() = 0;

    /**
       get_display_character_dim:

       This should return the current number of
       rows and columns that text can occupy.
       Irrespective of frontend.
       
     */
    virtual coord_t get_display_character_dim() = 0; 
    
    virtual void set_cursor_position(coord_t pos) = 0;


    virtual void next_line() = 0;
    virtual void prev_line() = 0;
    virtual void forward() = 0;
    virtual void backward() = 0;
    virtual void delete_char() = 0;
    virtual void new_line() = 0;
    virtual void end() = 0;
    virtual void home() = 0;

    
    
    virtual std::string get_user_input(std::string prompt) = 0;
    virtual void put_status_line(std::string msg) = 0;


    void persist_status(bool f) {
      this->status_persist = f;
    }
    
    virtual void open_file(std::string path) {
      if(this->openFile != nullptr) {

        auto in = this->get_user_input("Save Current Buffer? [Y/n]");

        if(!(in == "n" || in == "no" || in == "N" || in == "No")) {
          this->openFile->save();
        }

        in = this->get_user_input("Close Current Buffer? [y/N]");
        if((in == "y" || in == "yes" || in == "Y" || in == "Yes")) {
          delete this->openFile;
        } 
        
      }

      this->openFile = new files::Editor_File(path);
      this->Files[this->openFile->filename] = this->openFile;
    }

    void save(std::string path) {
      this->openFile->save_as(path.c_str());
    }


    virtual void switch_buffer(int index) {
      if(index <= this->Files.size() && index >= 1) {

        int i = 1;
        for(const auto& [key, value] : this->Files) {
          if(i == index) {
            this->Files[this->openFile->filename] = this->openFile;
            this->openFile = value;
            return;
          }
          i++;
        }       
        
      }
    }


    
    /**
       run

       execute the main editor loop

       render, poll, execute...
       
       
     */
    virtual void run() = 0;
    
  };



  class Frame {
  public:
    files::Editor_File::Line* start;
    files::Editor_File::Line* end;
    int size;
    int start_line_number;
    int end_line_number;
    
    Frame(files::Editor_File::Line* ctx, size_t N, int ctx_line);
    void scroll_up(int lines);
    void scroll_down(int lines);
    void display();

    void new_line_hook(bool last_line);
    void remove_line_hook();
    
  };
  
   

 
  class TUI_Editor : public Editor {
  private:


    void draw();
    
  public:
    Frame* f;
    TUI_Editor();

    coord_t get_cursor_position() override;
    coord_t get_display_character_dim() override;
    void set_cursor_position(coord_t pos) override;
    
    void next_line() override;
    void prev_line() override;
    void forward() override;
    void backward() override;
    void end() override;
    void home() override;
    void new_line() override;
    void delete_char() override;
        
    void run() override;
    void put_status_line(std::string msg) override;


    void open_file(std::string path) override;
    void switch_buffer(int index) override;
    
    void sync_cursors();

    std::string get_user_input(std::string prompt) override;
    
  };


 
  
  class GUI_Editor : public Editor {

  public:
    coord_t get_cursor_position() override;
    coord_t get_display_character_dim() override;
    void set_cursor_position(coord_t pos) override;
    void run() override;
    void next_line() override;
    void prev_line() override;
    void forward() override;
    void backward() override;
    void delete_char() override;
    void end() override;
    void home() override;


    void put_status_line(std::string msg) override;
    std::string get_user_input(std::string prompt) override;
    void new_line() override;

  };
}

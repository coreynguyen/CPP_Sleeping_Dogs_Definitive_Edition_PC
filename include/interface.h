/*
 ________  ___  ___  ________  _________        ________  ________  _________        ___   ___
|\   ____\|\  \|\  \|\   __  \|\___   ___\     |\   ____\|\   __  \|\___   ___\     |\  \ |\  \
\ \  \___|\ \  \\\  \ \  \|\  \|___ \  \_|     \ \  \___|\ \  \|\  \|___ \  \_|     \ \  \\_\  \  _______
 \ \  \    \ \   __  \ \   __  \   \ \  \       \ \  \  __\ \   ____\   \ \  \       \ \______  \|\  ___ \
  \ \  \____\ \  \ \  \ \  \ \  \   \ \  \       \ \  \|\  \ \  \___|    \ \  \       \|_____|\  \ \ \__\ \
   \ \_______\ \__\ \__\ \__\ \__\   \ \__\       \ \_______\ \__\        \ \__\             \ \__\ \______\
    \|_______|\|__|\|__|\|__|\|__|    \|__|        \|_______|\|__|         \|__|              \|__|\|______|

*/
// generated by Fast Light User Interface Designer (fluid) version 1.0305

#ifndef interface_h
#define interface_h
#include <FL/Fl.H>
#include "version.h"
#include "resource.h"
#include "filesystem.h"
#include <sstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <numeric>
#include <chrono>
#include <ctime>
#include <cmath>
#include <FL/x.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Multi_Label.H>
#include <FL/Fl_Table_Row.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Image.H>
#include "interface_logo.h"
#include "Filenames.h"
#include "BigInventoryFile.h"
extern std::string app_version; 
extern std::string app_filename; 
extern bool USE_DARK_MODE; 
void updateLabels();

class MyTable : public Fl_Table_Row {
public:
  std::vector<std::vector<std::string>> data; 
  std::vector<std::vector<std::string>> original_data; 
  int secondary_selection[4]; 
  std::vector<bool> displayStatus; 
  bool useStatusColors; 
  std::vector<int> sort_direction; 
  int sorting_column; 
  std::vector<int> indices; 
  bool resizing = false; 
  int resizing_col; 
  int resizing_start_x; 
  int resizing_start_width; 
  int original_x; 
  int original_y; 
  int original_w; 
  int original_h; 
private:
  bool altKeyPressed; 
public:
  /**
   Mapping from displayed rows to globalBigInventory entries
  */
  std::vector<int> row_to_entry_map; 
  std::string headers[5] = {"Hash", "Filename", "Offset", "Size", "Status"}; 
  /**
   // Reference to the search input widget
  */
  Fl_Input* search_input; 
  MyTable(int X, int Y, int W, int H, const char* L = "") ;
protected:
  void draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) override;
  int handle(int event) override;
public:
  void nullifySelectedEntries();
  void deleteSelectedEntries();
  void addEntry();
  void reset_row_colors();
  void reset_table();
  void load_entries(const BigInventory_t &inventory);
  void DrawHeader(int C, int X, int Y, int W, int H);
  void sort_column(int col);
  void sort_by_column(int column);
  void filterData(const std::string& filter_text);
  void handle_click_on_header(int col);
  void resize_to_fit(int col);
  int get_col_under_mouse(int mouse_x);
  int get_col_under_mouse_for_sort(int mouse_x);
  void create_context_menu();
  int col_right_edge(int col);
  void setData(const std::vector<std::vector<std::string>>& newData);
private:
  void updateRowToEntryMap();
  bool find_and_select(const std::string& find_str, bool whole_word_only);
  void handleDelayedTask();
  void performDelayedTask();
  void ensureMinimumRows();
  void showEditPopup(int row);
};
#include <FL/Fl_Double_Window.H>
extern Fl_Double_Window *mainWindow;
#include <FL/Fl_Menu_Bar.H>
extern Fl_Menu_Bar *mnu_main;
#include <FL/Fl_Group.H>
extern Fl_Group *grp_main;
extern Fl_Group *grp_crc32;
#include <FL/Fl_Input.H>
extern Fl_Input *edt_crc32str;
#include <FL/Fl_Output.H>
extern Fl_Output *edt_crc32hex;
extern MyTable *lbox_files;
extern Fl_Group *grp_statusbar;
extern Fl_Group *stb_left;
#include <FL/Fl_Box.H>
extern Fl_Box *stb_llabel;
extern Fl_Group *stb_mid;
extern Fl_Group *stb_right;
extern Fl_Box *stb_rlabel;
Fl_Double_Window* dogpack();
extern Fl_Menu_Item menu_mnu_main[];
#define mnu_main_file (menu_mnu_main+0)
#define mnu_main_open (menu_mnu_main+1)
#define mnu_main_save (menu_mnu_main+2)
#define mnu_main_exit (menu_mnu_main+3)
#define mnu_main_tools (menu_mnu_main+5)
#define mnu_main_exp (menu_mnu_main+6)
#define mnu_main_exp_all (menu_mnu_main+7)
#define mnu_main_imp (menu_mnu_main+8)
#define mnu_main_imp_all (menu_mnu_main+9)
#define mnu_main_add (menu_mnu_main+10)
#define mnu_main_del (menu_mnu_main+11)
#define mnu_main_null (menu_mnu_main+12)
#define mnu_main_help (menu_mnu_main+14)
#define mnu_main_about (menu_mnu_main+15)
#endif

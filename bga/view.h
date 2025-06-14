#pragma once

typedef void (*fnevent)();
typedef void (*fncommand)(void* object);

struct item;

enum resn : unsigned short;

extern bool button_pressed, button_executed, button_hilited, input_disabled;
extern fnevent on_player_change;

const char* getkg(int weight);

void* choose_combat_action();

void button(resn res, unsigned short f1, unsigned short f2, unsigned key = 0);
void button(resn res, unsigned short f1, unsigned short f2, unsigned key, const char* id);
void button(resn res, unsigned short f1, unsigned short f2, unsigned key, const char* id, unsigned short fd, bool allowed);
void button_check(unsigned key);
void change_zoom_factor();
void checkbox(int& source, int value, resn res, unsigned short f1, unsigned short f2, unsigned short fc, unsigned key);
void choose_creature();
bool confirm_overvrite();
void edit(char* string, size_t maximum, unsigned text_flags, bool upper_case = false);
void fire(fnevent proc, long param = 0, long param2 = 0, const void* object = 0);
void hotkey(unsigned key, fnevent proc, int param = 0);
void initialize_ui();
void input_debug();
void next_scene();
void next_scene(fnevent proc);
void paint_action_panel();
void paint_action_panel_combat();
void paint_action_panel_player();
void paint_action_panel_na();
void paint_area();
void paint_description(int scr_x, int scr_y, int scr_height);
void paint_dialog(resn v, int frame = 0);
void paint_game_dialog(resn v, int frame = 0);
void paint_game_inventory();
void paint_game_panel(bool allow_input, bool combat_mode);
void paint_item(const item* pi);
void paint_list(void* data, size_t size, int maximum, int& origin, int per_page, fncommand proc, int row_height, point scr, int scr_height, fnevent action_proc, fnevent info_proc);
void paint_minimap();
void paint_worldmap_area();
void paperdoll();
void open_combat_mode();
long open_dialog(fnevent proc, bool faded);
void open_dialog();
void open_item_count();
void open_item_description();
void open_game();
void open_save_game();
void open_scene();
void open_store();
void open_worldmap();
void set_description(const char* id);
void setdialog(int x, int y);
void setdialog(int x, int y, int w, int h);
void texta(resn res, const char* string, unsigned flags);
void update_frames();

unsigned char open_color_pick(unsigned char current_color, unsigned char default_color);

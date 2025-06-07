#pragma once

typedef void (*fnevent)();

enum resn : unsigned short;

extern bool button_pressed, button_executed, button_hilited, input_disabled;

const char* getkg(int weight);

void button(resn res, unsigned short f1, unsigned short f2, unsigned key = 0);
void button(resn res, unsigned short f1, unsigned short f2, unsigned key, const char* id);
void button_check(unsigned key);
void change_zoom_factor();
void fire(fnevent proc, long param = 0, long param2 = 0, const void* object = 0);
void hotkey(unsigned key, fnevent proc, int param = 0);
void initialize_ui();
void input_debug();
void next_scene();
void next_scene(fnevent proc);
void paint_area();
void paint_dialog(resn v, int frame = 0);
void paint_game_dialog(resn v, int frame = 0);
void paint_game_inventory();
void paint_minimap();
void paint_worldmap_area();
void paperdoll();
long open_dialog(fnevent proc, bool faded);
void open_dialog();
void open_item_description();
void open_game();
void open_scene();
void open_worldmap();
void setdialog(int x, int y);
void setdialog(int x, int y, int w, int h);
void texta(resn res, const char* string, unsigned flags);

unsigned char open_color_pick(unsigned char current_color, unsigned char default_color);

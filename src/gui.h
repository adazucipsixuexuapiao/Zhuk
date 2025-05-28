#pragma once

#include "renderer.h"
#include "asciiloader.h"
#include "config.h"
#include "menugui.h"
#include "timer.h"
#include "localizer.h"
#include "gamelogic.h"

#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include "client.h"

class GUIButton;
class GUIPanel;
class GUIText;

enum e_current_state
	{
		QUIT, MENU, GAME, READ, SETTINGS, WAITING
	};

enum enum_structures
{
	NONE, FURNACE, DRILL, FACTORY, ARM, ROAD
};

class GuiManager
{
public:
	void init(); // gets called at the very start
	void init_menu_gui(); // initialize menu gui elements
	void init_game_gui(); // initialize game gui elements
	
	void update(std::vector<SDL_Event>& events); // updates buttons
	
	void draw_cursor();
	SDL_Rect cursor_rect;
	int mouse_x, mouse_y; // in ascii coordinates
	
	void draw_gui(); // draws gui elements
	void draw_game_gui();
	
	void draw_cursor_ascii(); // draws ascii arrow at cursor(with possible alt text)
	std::string current_alt_text;
	bool drawing_alt_text; // true or false
	
	void add_button(std::string text, int x, int y, int w, int h, void (*_onClick)(void*), void* _arg, std::string alt_text="");
	void dealloc_buttons();
	~GuiManager();
	
	GUIDirt dirt;
	AsciiManager ascii;
	
	std::vector<GUIButton*> buttons;
	std::vector<GUIText*> text;
	
	// panel hierarchy is - last element gets drawn in front, selected panel gets moved to front
	std::vector<GUIPanel*> panels;
	
	e_current_state current_state;
	bool can_play;
	bool is_inputting; // cancels game commands until input is finished
	bool clicked_panel; // gets updated each frame, to ensure panel clicks dont propogate to world
	bool just_loaded_game=true;
	
	enum_structures current_construction = NONE;
	
	GUIText* selected_struct;
};
extern GuiManager gui; // guimanager global object

// milliseconds
#define GUIBUTTON_ALT_TEXT_TIMER_MS 1000
class GUIButton
{
public:
	std::string text; // text on the button
	std::string alt_text; // when hovering
	unsigned char color_idle;
	unsigned char color_press;
	unsigned char color_hover;
	int x, y, w, h; // button position, dimensions(measured from top left)

	void* arg; // argument
	void (*onClick)(void*);
	
	bool hovering;
	bool is_active;
	float ms_hovered;
	
	bool mouseOn(int mx, int my); // check if the mouse is hovering on the button?
	void init(std::string _text, int _x, int _y, int _w, int _h, void (*_onClick)(void*), void* _arg, std::string _alt_text="");
	void draw();
	void click();
	
	GUIPanel* gui_panel = nullptr; // if gui_panel is specified, then use x, y as relative to panel
private:
};

void clickPlay(void* arg);
void clickRead(void* arg);
void clickSettings(void* arg);
void clickQuit(void* arg);

// game buttons
void clickConstruct(void* arg);
void clickMap(void* arg);
void clickInfo(void* arg);
void clickUnits(void* arg);

// integer displaying class
class GUIInteger
{
public:
	int x, y, w;
	unsigned char color;
	bool use_ptr;
	int* num_ptr;
	int num;
	std::string str;

	void init(int _x, int _y, int _w, unsigned char _color, int _num);
	void init_ptr(int _x, int _y, int _w, unsigned char _color, int* _num);
	void set(int _num);
	void set_ptr(int* _num_ptr);
	void update_str();
	void update();
	void draw();
	
	GUIPanel* gui_panel = nullptr; // if gui_panel is specified, then use x, y as relative to panel
};

class GUIFloat
{
public:
	int x, y, w, prec; // prec is precision
	unsigned char color;
	bool use_ptr;
	float* num_ptr;
	float num;
	std::string str;
	
	void init(int _x, int _y, int _w, unsigned char _color, float _num, int _prec);
	void init_ptr(int _x, int _y, int _w, unsigned char _color, float* _num_ptr, int _prec);
	void set(float _num);
	void set_ptr(float* _num_ptr);
	void update_str();
	void update();
	void draw();
	
	GUIPanel* gui_panel = nullptr; // if gui_panel is specified, then use x, y as relative to panel
};

class GUIText
{
public:
	int x, y, w;
	bool use_ptr;
	unsigned char color;
	std::string str;
	std::string* str_ptr;
	void init(int _x, int _y, int _w, unsigned char _color, std::string _str);
	void init_ptr(int _x, int _y, int _w, unsigned char _color, std::string* _str_ptr);
	void draw();
	GUIPanel* gui_panel = nullptr; // if gui_panel is specified, then use x, y as relative to panel
};

#define GUI_INPUT_MS_CURSOR_FLASH 100
// text input class
class GUIInput
{
public:
	bool inputting;

	int x, y, w;
	std::string buff; // text in the buffer
	std::string inp; // currently stored text
	
	unsigned int cursor_ind; // cursor index
	
	void init(int _x, int _y, int _w);
	void update(std::vector<SDL_Event>& evnt, bool clicking);
	void draw();
	void set_inp();
	void clear_buff();
	void begin_input();
	void end_input();
	
	float elapsed_time; // for blinking
	bool blink_state; // cursor blinking animation state
	
	GUIPanel* gui_panel = nullptr; // if gui_panel is specified, then use x, y as relative to panel
};

class GUIPanel
{
public:
	int x, y, w, h; // position and dimensions
	unsigned char box_color; // color of the box
	bool is_selected;
	bool is_visible;
	
	bool destroy; // true when ready to destroy
	bool dragging = false;
	int drag_x; // dragging x relative to the window
	
	void init(int _x, int _y, int _w, int _h, unsigned char _box_color);
	void draw();
	void update(std::vector<SDL_Event>& evnt);
	void deselect();
	~GUIPanel();
	
	bool use_draw=false; // use custom draw function
	bool use_update=false; // use custom update function
	void set_draw(void (*_draw_func)(void*)); // set custom draw function
	void set_update(void (*_update_func)(void*)); // set custom update function
	void (*draw_func)(void*);
	void (*update_func)(void*);
	
	bool mouseOn(int mx, int my);
	
	std::vector<GUIButton*> buttons;
	std::vector<GUIInput*> input;
	std::vector<GUIInteger*> integers;
	std::vector<GUIFloat*> floats;
	std::vector<GUIText*> text;
};

GUIPanel* make_default_panel();
GUIPanel* make_hover_panel();
GUIPanel* make_ctrl_click_panel();

void clickMakeDrill(void* arg);
void clickMakeRoad(void* arg);
void clickMakeArm(void* arg);
void clickMakeFurnace(void* arg);
void clickMakeFactory(void* arg);
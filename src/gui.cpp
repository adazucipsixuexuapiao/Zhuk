#include "gui.h"

#include "gamelogic.h" // to interface between gui and gamelogic

GuiManager gui;

void GuiManager::init()
{
	SDL_ShowCursor(SDL_DISABLE);
	cursor_rect.w = 32; cursor_rect.h = 32;

	dirt.init(rend.w,rend.h); // init the effect
	
	ascii.load("data/img/heart.img","heart");
	
	current_alt_text = "";
	drawing_alt_text = false;
	
	current_state = MENU;
	can_play = false;
}

void GuiManager::draw_gui()
{
	unsigned int _fps = 1000 / fps_timer.deltaTime;
	if(current_state == MENU)
	{
		dirt.draw(); // cool effect
		dirt.update(rend.w, rend.h);
		
		// title
		rend.draw_text(rend.w/2-2, rend.h/2-7, loc("Zhuk"), 0xf6);
		rend.draw_box_full(rend.w/2-5,rend.h/2-5,rend.w/2+5,rend.h/2+5,0xf0);
	}
	
	if(current_state == GAME)
	{
		draw_game_gui();
	}
	
	static int animation = 0;
	animation ++;
	if(current_state == WAITING)
	{ // draw waiting animation
		rend.draw_text(0,0, loc("Waiting"), 0xf0);
		for(int i=0;i<animation % 4;i++)
			rend.draw_text(loc("Waiting").length()+i, 0, ".", 0xf0);
	}
	
	// draw buttons
	for(int i=0;i<buttons.size();i++)
		buttons[i]->draw();
	
	// draw panels
	for(int i=0;i<panels.size();i++)
		panels[i]->draw();
	
	// draw text
	for(int i=0;i<text.size();i++)
		text[i]->draw();
	
	// draw FPS counter
	if(config.get_bool("fps_counter"))
	{
		GUIInteger gui_fps;
		gui_fps.init(0,0,0,0,_fps);
		rend.draw_text(rend.w-6,0,"FPS:", 0x10); rend.draw_text(rend.w-2,0,gui_fps.str,0x10);
	}
	
	// cursor in ascii
	draw_cursor_ascii();
}

void GuiManager::draw_game_gui()
{
	rend.draw_rect(0, 0, 16, rend.h-1, 0x28,0xB0);
	// vertical bar
	for(int i=0;i<rend.h;i++)
		rend.set(16,i,0xba, 0x80);
	// update selected struct text
	selected_struct->str = "struktura:";
	if(current_construction == NONE) selected_struct->str += "nav";
	if(current_construction == FURNACE) selected_struct->str += "pliits";
	if(current_construction == DRILL) selected_struct->str += "urbis";
	if(current_construction == FACTORY) selected_struct->str += "rupnica";
	if(current_construction == ARM) selected_struct->str += "roka";
	if(current_construction == ROAD) selected_struct->str += "celsh";
}

void GuiManager::draw_cursor_ascii()
{
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	
	cursor_rect.x = mx; cursor_rect.y = my;
	
	mouse_x = (int)(mx / rend.tile_w);
	mouse_y = (int)(my / rend.tile_h);
	
	rend.set(mouse_x, mouse_y, 0x18, 0x10);
	
	if(drawing_alt_text)
	{
		rend.draw_text(mouse_x+1,mouse_y-1,current_alt_text,0xf3);
	}
}

void GuiManager::draw_cursor()
{	
	// cursor icon
	SDL_RenderCopy(rend.rend, rend.cursor, NULL, &cursor_rect);
}

void GuiManager::init_menu_gui()
{
	add_button(loc("Infect"), rend.w/2-3, rend.h/2-3, 6, 1, clickPlay, (void*)this, loc("Become the pest..."));
	add_button(loc("Read"), rend.w/2-3, rend.h/2-2, 4, 1, clickRead, (void*)this, loc("Inquire about the pests..."));
	add_button(loc("Settings"), rend.w/2-3, rend.h/2-1, 8, 1, clickSettings, (void*)this);
	add_button(loc("Rot"), rend.w/2-3, rend.h/2, 3, 1, clickQuit, (void*)this);
}

void GuiManager::init_game_gui()
{
	add_button("Konstruet(b)", 1, 1, 12, 1, clickConstruct, (void*)this, "Izdot paveli konstruet");
	add_button("Karte(m)", 1, 3, 8, 1, clickMap, (void*)this, "Skatit pasaules karti");
	add_button("Informacija(v)",1,5,14,1, clickInfo, (void*)this, "Skatit informaciju");
	add_button("Vienibas(u)",1,7,11,1,clickUnits, (void*)this, "Skatit vienibas");
	
	selected_struct = new GUIText;
	selected_struct->init(1,9,-1,0x07, "struktura:nav");
	text.push_back(selected_struct);
}

void GuiManager::update(std::vector<SDL_Event>& events)
{
	// initialize the game GUI when just loaded
	if(just_loaded_game && current_state == GAME)
	{
		just_loaded_game = false;
		init_game_gui();
	}
	
	clicked_panel = false;
	bool click = false;
	for(int i=0;i<events.size();i++)
		if(events[i].type == SDL_MOUSEBUTTONDOWN && events[i].button.button == SDL_BUTTON_LEFT)
			click = true;
	
	// update all buttons
	for(int i=0;i<buttons.size();i++)
	{
		bool mouseon = buttons[i]->mouseOn(mouse_x, mouse_y);
		if(mouseon && click)
		{
			drawing_alt_text = false;
			buttons[i]->click();
		}
	}
	
	// check for clicks
	if(click)
	{
		int index=panels.size();
		for(int i=panels.size()-1;i>=0;i--)
		{
			if(panels[i]->mouseOn(gui.mouse_x, gui.mouse_y))
			{
				clicked_panel = true;
				GUIPanel* panel = panels[i];
				panels.erase(panels.begin()+i);
				panels.push_back(panel); // move panel to the front
				panel->is_selected = true;
				index = i;
				break;
			} else {
				panels[i]->deselect();
			}
		}
		for(int i=0;i<index;i++)
			panels[i]->deselect(); // deselect all
		// panels other than the one selected
	}
		
	// update panels
	for(int i=0;i<panels.size();i++)
	{
		panels[i]->update(events);
		// delete destroyed panels
		if(panels[i]->destroy)
		{
			delete panels[i];
			panels.erase(panels.begin()+i);
		}
	}
}

void GuiManager::add_button(std::string text, int x, int y, int w, int h, void (*_onClick)(void*), void* _arg, std::string alt_text)
{
	GUIButton* button = new GUIButton;
	button->init(text, x, y, w, h, _onClick, _arg, alt_text);
	buttons.push_back(button);
}

void GuiManager::dealloc_buttons()
{
	// free allocated buttons
	for(int i=0;i<buttons.size();i++)
		if(buttons[i] != nullptr) delete buttons[i];
	buttons.clear();
}

GuiManager::~GuiManager()
{
	dealloc_buttons();
	// dealloc text
	for(int i=0;i<text.size();i++)
		if(text[i] != nullptr) delete text[i];
	text.clear();
}

/* ===== GUIBUTTON ====== */
void GUIButton::init(std::string _text, int _x, int _y, int _w, int _h, void (*_onClick)(void*), void* _arg, std::string _alt_text)
{
	text = _text; alt_text = _alt_text; x = _x; y = _y; w = _w; h = _h;
	color_press = 0xf7; color_hover = color_press;
	color_idle = 0xf8;
	ms_hovered = 0.0f;
	onClick = _onClick;
	arg = _arg;
	
	hovering = false;
	is_active = true;
}

bool GUIButton::mouseOn(int mx, int my)
{
	if(!is_active) return false;
	
	// relative to panel if there is any
	int gx, gy;
	if(gui_panel == nullptr)
	{
		gx = x; gy = y;
	} else {
		gx = gui_panel->x + x; gy = gui_panel->y + y;
	}
	
	// bounds checking
	if(mx >= gx && mx < gx+w && my >= gy && my < gy+h)
		hovering = true;
	else
	{
		if(hovering) // was hovering earlier
		{
			gui.current_alt_text = "";
			gui.drawing_alt_text = false;
		}
		hovering = false;
	}
	
	// timer changing
	if(hovering)
	{
		ms_hovered += fps_timer.deltaTime;
	} else {
		ms_hovered = 0.0f;
	}
	
	// setting alt text
	if(ms_hovered >= GUIBUTTON_ALT_TEXT_TIMER_MS)
	{
		gui.drawing_alt_text=true;
		gui.current_alt_text = alt_text;
	}
	
	return hovering;
}

void GUIButton::draw()
{
	int draw_x, draw_y;
	if(gui_panel == nullptr)
	{
		draw_x = x; draw_y = y;
	} else {
		draw_x = gui_panel->x + x; draw_y = gui_panel->y + y;
	}
	if(!is_active) return;
	if(hovering)
	{
		rend.draw_rect(draw_x,draw_y,w+x-1,h+y-1,color_hover, 0x00);
		rend.draw_text(draw_x,draw_y,text,color_hover);
	}
	else
	{
		rend.draw_rect(draw_x,draw_y,w+x-1,h+y-1,color_idle, 0x00);
		rend.draw_text(draw_x,draw_y,text,color_idle);
	}
}

void GUIButton::click()
{
	audio.play("push.wav");
	onClick(arg);
}

/* BUTTON STUFF */
void clickPlay(void* arg)
{
	if(gui.can_play)
	{
		gui.current_state = WAITING;
		gui.dealloc_buttons();
		client_thread.join();
		client_thread = std::thread(client_wait_for_tick);
	}
}

void clickRead(void* arg)
{
	gui.current_state = READ;
	gui.dealloc_buttons();
}

void clickSettings(void* arg)
{
	gui.current_state = SETTINGS;
	gui.dealloc_buttons();
}

void clickQuit(void* arg)
{
	gui.current_state = QUIT;
	gui.dealloc_buttons();
}

void clickConstruct(void* arg)
{
	GUIPanel* construct_panel = new GUIPanel;
	construct_panel->init(0,0,30,13,0x77);
	
	// teksts
	GUIText* txt = new GUIText;
	txt->init(0,1,-1,0x07,"wasd - novietot ar orientaciju");
	txt->gui_panel = construct_panel;
	construct_panel->text.push_back(txt);
	
	GUIButton* btn_road = new GUIButton;
	btn_road->init("Celsh", 1, 3, 5, 1, clickMakeRoad, arg);
	btn_road->gui_panel = construct_panel;
	construct_panel->buttons.push_back(btn_road);
	
	GUIButton* btn_drill = new GUIButton;
	btn_drill->init("Urbis", 1, 5, 5, 1, clickMakeDrill, arg);
	btn_drill->gui_panel = construct_panel;
	construct_panel->buttons.push_back(btn_drill);
	
	GUIButton* btn_arm = new GUIButton;
	btn_arm->init("Roka", 1, 7, 4, 1, clickMakeArm, arg);
	btn_arm->gui_panel = construct_panel;
	construct_panel->buttons.push_back(btn_arm);
	
	GUIButton* btn_furnace = new GUIButton;
	btn_furnace->init("Pliits", 1, 9, 6, 1, clickMakeFurnace, arg);
	btn_furnace->gui_panel = construct_panel;
	construct_panel->buttons.push_back(btn_furnace);
	
	GUIButton* btn_factory = new GUIButton;
	btn_factory->init("Rupnica", 1, 11, 7, 1, clickMakeFactory, arg);
	btn_factory->gui_panel = construct_panel;
	construct_panel->buttons.push_back(btn_factory);
	
	gui.panels.push_back(construct_panel);
}
void clickMap(void* arg)
{
	
}

void clickInfo(void* arg)
{
	
}

void clickUnits(void* arg)
{
	
}

// construction panel
void clickMakeFurnace(void* arg) {gui.current_construction = FURNACE;}
void clickMakeDrill(void* arg) {gui.current_construction = DRILL;}
void clickMakeFactory(void* arg) {gui.current_construction = FACTORY;}
void clickMakeArm(void* arg) {gui.current_construction = ARM;}
void clickMakeRoad(void* arg) {gui.current_construction = ROAD;}

/* GUI INTEGERS */
void GUIInteger::init(int _x, int _y, int _w, unsigned char _color, int _num)
{
	x = _x; y = _y; color = _color; num = _num; w = _w;
	use_ptr = false;
	update_str();
}

void GUIInteger::init_ptr(int _x, int _y, int _w, unsigned char _color, int* _num)
{
	x = _x; y = _y; color = _color; num_ptr = _num; w = _w;
	use_ptr = true;
	update_str();
}

void GUIInteger::update_str()
{
	bool negative = false;
	int n;
	if(use_ptr == true) n = *num_ptr;
	else n = num;
	if(n < 0) {negative = true; n = -n;}
	str = "";
	while(true)
	{
		str += '0' + (n % 10);
		n /= 10;
		if(n == 0) break;
	}
	if(negative) str += '-';
	std::reverse(str.begin(), str.end());
}
void GUIInteger::set(int _num)
{
	num = _num;
	update_str();
}
void GUIInteger::set_ptr(int* _num_ptr)
{
	num_ptr = _num_ptr;
	update_str();
}
void GUIInteger::update(){update_str();}

void GUIInteger::draw()
{
	int draw_x, draw_y;
	if(gui_panel == nullptr)
	{
		draw_x = x; draw_y = y;
	} else {
		draw_x = gui_panel->x + x; draw_y = gui_panel->y + y;
	}
	if(w >= 0)
		rend.draw_text_w(draw_x, draw_y, w, str, color);
	else rend.draw_text(draw_x,draw_y,str,color);
}

/* GUI FLOATS */
void GUIFloat::init(int _x, int _y, int _w, unsigned char _color, float _num, int _prec)
{
	x = _x; y = _y; color = _color; num = _num; w = _w;
	use_ptr = false; prec = _prec;
	update_str();
}

void GUIFloat::init_ptr(int _x, int _y, int _w, unsigned char _color, float* _num, int _prec)
{
	x = _x; y = _y; color = _color; num_ptr = _num; w = _w;
	use_ptr = true; prec = _prec;
	update_str();
}

void GUIFloat::update_str()
{
	bool negative = false;
	float n;
	if(use_ptr == true) n = *num_ptr;
	else n = num;
	if(n < 0) {negative = true; n = -n;}
	str = "";
	std::string whole_str="";
	int whole_n = std::floor(n);
	float frac_n = n - whole_n;

	// loop through the whole part of n
	while(true)
	{
		whole_str += '0' + (whole_n % 10);
		whole_n /= 10;
		if(whole_n == 0) break;
	}
	
	std::string frac_str = "";
	// loop through fractional part of ns
	for(int i=0;i<prec;i++)
	{
		frac_n *= 10;
		frac_str += '0' + std::floor(frac_n);
		frac_n -= std::floor(frac_n);
	}
	
	// remove unneccessary 0's from fractional part
	int last_zero_index = -1;
	for(int i=frac_str.length()-1;i>=0;i--)
	{
		if(frac_str[i] = '0') last_zero_index = i;
		else break;
	}
	if(last_zero_index != -1) // if there is something to delete
		frac_str.erase(frac_str.begin()+last_zero_index,frac_str.end());
	
	if(negative) whole_str += '-';
	std::reverse(whole_str.begin(), whole_str.end());
	if(frac_str.length() > 0)
	{
		whole_str += '.';
		whole_str += frac_str;
	}
	str = whole_str;
}
void GUIFloat::set(float _num)
{
	num = _num;
	update_str();
}
void GUIFloat::set_ptr(float* _num_ptr)
{
	num_ptr = _num_ptr;
	update_str();
}
void GUIFloat::update(){update_str();}

void GUIFloat::draw()
{
	int draw_x, draw_y;
	if(gui_panel == nullptr)
	{
		draw_x = x; draw_y = y;
	} else {
		draw_x = gui_panel->x + x; draw_y = gui_panel->y + y;
	}
	if(w >= 0)
		rend.draw_text_w(draw_x, draw_y, w, str, color);
	else rend.draw_text(draw_x,draw_y,str,color);
}

void GUIText::init(int _x, int _y, int _w, unsigned char _color, std::string _str)
{
	x = _x; y = _y; str = _str; use_ptr = false; color = _color; w = _w;
}
void GUIText::init_ptr(int _x, int _y, int _w, unsigned char _color, std::string* _str_ptr)
{
	x = _x; y = _y; str_ptr = _str_ptr; use_ptr = true;
	color = _color; w = _w;
}

void GUIText::draw()
{
	int draw_x, draw_y;
	if(gui_panel == nullptr)
	{
		draw_x = x; draw_y = y;
	} else {
		draw_x = gui_panel->x + x; draw_y = gui_panel->y + y;
	}
	if(!use_ptr)
		rend.draw_text_w(draw_x, draw_y, w, str, color);
	else rend.draw_text_w(draw_x,draw_y,w,*str_ptr, color);
}

/* INPUT */
void GUIInput::init(int _x, int _y, int _w)
{
	cursor_ind = 0;
	x = _x; y = _y; w = _w;
	buff = ""; inp = "";
	for(int i=0;i<w;i++)
		buff += ' ';
	
	// for cursor animation
	blink_state = false;
	elapsed_time = 0.0f;
	
	// state
	inputting = false;
}

void GUIInput::update(std::vector<SDL_Event>& evnt,bool clicking)
{
	elapsed_time += fps_timer.deltaTime;
	// if animation cycle passed
	if(elapsed_time >= GUI_INPUT_MS_CURSOR_FLASH)
	{
		// change animation and reset clock
		elapsed_time = 0.0f;
		blink_state = !blink_state;
	}
	
	// check if clicked
	if(gui_panel->is_selected && clicking)
	{
		if(gui.mouse_x >= x+gui_panel->x && gui.mouse_x < x+w+gui_panel->x &&
		gui.mouse_y == y+gui_panel->y)
			begin_input();
		else end_input();
	}
	
	if(inputting == true)
	{
		for(int i=0;i<evnt.size();i++)
		{
			if(evnt[i].type != SDL_KEYDOWN) continue;
			SDL_Keycode key = evnt[i].key.keysym.sym;
			// pressing written key
			if(key >= SDLK_SPACE && key <= SDLK_z)
			{
				unsigned char symbol = ' ' + (unsigned char)(key - SDLK_SPACE);
				buff[cursor_ind] = symbol;
				if(cursor_ind < buff.length()-1)
					cursor_ind ++;
			}
			// moving cursor
			if(key == SDLK_LEFT)
				if(cursor_ind > 0) cursor_ind --;
			if(key == SDLK_RIGHT)
				if(cursor_ind < buff.length()-1)
					cursor_ind ++;
			if(key == SDLK_BACKSPACE)
			{
				buff[cursor_ind] = ' ';
				if(cursor_ind > 0) cursor_ind --;
			}
			if(key == SDLK_RETURN) // accept input
			{
				set_inp();
				clear_buff();
				end_input();
			}
		}
	}
}

void GUIInput::begin_input()
{
	inputting = true;
	gui.is_inputting = true;
}

void GUIInput::end_input()
{
	if(inputting)
		gui.is_inputting = false;
	inputting = false;
}

void GUIInput::set_inp()
{
	int last_space_ind = -1;
	for(int i=buff.length()-1;i>=0;i--)
	{
		if(buff[i] == ' ') last_space_ind = i;
		else break;
	}
	inp = buff;
	if(last_space_ind != -1)
		inp.erase(inp.begin()+last_space_ind, inp.end());
}

void GUIInput::clear_buff()
{
	buff = "";
	for(int i=0;i<w;i++)
		buff += ' ';
}

void GUIInput::draw()
{
	int draw_x, draw_y;
	if(gui_panel == nullptr)
	{
		draw_x = x; draw_y = y;
	} else {
		draw_x = gui_panel->x + x; draw_y = gui_panel->y + y;
	}
	// set black background
	for(int i=0;i<w;i++)
		rend.set(draw_x+i, draw_y, 0x00, 0x00);
	rend.draw_text(draw_x, draw_y, buff, 0xf0);
	// draw cursor
	if(inputting == false) return;
	unsigned char cur_color = 0xf0;
	if(blink_state) cur_color = 0x0f;
	rend.draw_text(draw_x+cursor_ind, draw_y, std::string(1,buff[cursor_ind]), cur_color);
}

void clickPanelX(void* _gui_panel)
{
	GUIPanel* gui_panel = (GUIPanel*)_gui_panel;
	gui_panel->destroy = true;
}

/* PANEL */
void GUIPanel::init(int _x, int _y, int _w, int _h, unsigned char _box_color)
{
	is_selected = true;
	is_visible = true;
	x = _x; y = _y; w = _w; h = _h; box_color = _box_color;
	destroy = false;
	
	GUIButton* quitbtn = new GUIButton;
	quitbtn->init(std::string("X"), x+w-1, y, 1, 1, clickPanelX, this);
	quitbtn->gui_panel = this;
	buttons.push_back(quitbtn);
}

void GUIPanel::draw()
{
	if(!is_visible) return;
	// draw box
	rend.draw_rect(x,y,x+w-1,y+h-1,box_color,0x00);
	// draw bar
	rend.draw_rect(x,y,x+w-1,y,0x87,0xb1);
	
	// draw elements
	for(int i=0;i<integers.size();i++)
		integers[i]->draw();
	for(int i=0;i<floats.size();i++)
		floats[i]->draw();
	for(int i=0;i<input.size();i++)
		input[i]->draw();
	for(int i=0;i<text.size();i++)
		text[i]->draw();
	for(int i=0;i<buttons.size();i++)
		buttons[i]->draw();
	
	if(use_draw)
		draw_func(this);
}

void GUIPanel::update(std::vector<SDL_Event>& evnt)
{
	bool clicking = false;
	if(!is_visible) return;
	if(is_selected) // update based on user input like clicks
		for(int i=0;i<evnt.size();i++)
		{
			SDL_Event e = evnt[i];
			if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
			{
				clicking = true;
				// drag if top panel is being clicked
				if(gui.mouse_x >= x && gui.mouse_x < x+w-1 && gui.mouse_y == y)
				{
					dragging = true;
					drag_x = gui.mouse_x - x;
				}
				
				// button handling
				for(int i=0;i<buttons.size();i++)
				{
					GUIButton* btn = buttons[i];
					if(gui.mouse_x >= btn->x + x && gui.mouse_x < btn->x + x + btn->w
					&& gui.mouse_y >= btn->y + y && gui.mouse_y < btn->y + y + btn->h)
					{
						// click the button
						btn->click();
					}
				}
			}
			if(e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
			{
				dragging = false;
			}
		}
	
	// drag panel if dragging
	if(dragging)
	{
		x = gui.mouse_x - drag_x;
		y = gui.mouse_y;
	}
	
	// update panel elements
	for(int i=0;i<input.size();i++)
		input[i]->update(evnt,clicking);

	for(int i=0;i<integers.size();i++)
		integers[i]->update();
	
	for(int i=0;i<floats.size();i++)
		floats[i]->update();
	
	for(int i=0;i<buttons.size();i++)
		buttons[i]->mouseOn(gui.mouse_x, gui.mouse_y);
	
	if(use_update)
		update_func(this);
}

void GUIPanel::deselect()
{
	is_selected = false;
	// deactivate input
	for(int i=0;i<input.size();i++)
		input[i]->end_input();
}

bool GUIPanel::mouseOn(int mx, int my)
{
	if(!is_visible) return false;
	if(mx >= x && mx < x+w && my >= y && my < y+h)
		return true;
	return false;
}

void GUIPanel::set_draw(void (*_draw_func)(void*))
{
	use_draw = true;
	draw_func = _draw_func;
}

void GUIPanel::set_update(void (*_update_func)(void*))
{
	use_update = true;
	update_func = _update_func;
}

GUIPanel::~GUIPanel()
{
	deselect();
	// deallocate buttons from heap
	for(int i=0;i<buttons.size();i++)
		if(buttons[i] != nullptr) delete buttons[i];

	// deallocate inputs from heap
	for(int i=0;i<input.size();i++)
		if(input[i] != nullptr) delete input[i];
	
	// deallocate integers from heap
	for(int i=0;i<integers.size();i++)
		if(integers[i] != nullptr) delete integers[i];
	
	// deallocate floats from heap
	for(int i=0;i<floats.size();i++)
		if(floats[i] != nullptr) delete floats[i];

	// deallocate text from heap
	for(int i=0;i<text.size();i++)
		if(text[i] != nullptr) delete text[i];
}

GUIPanel* make_default_panel()
{
	GUIPanel* new_panel = new GUIPanel;
	new_panel->init(0,0,20,20,0x77);
	
	GUIText* txt = new GUIText;
	txt->init(2,2,-1,0x07,"Hello, World");
	txt->gui_panel = new_panel;
	new_panel->text.push_back(txt);
	
	GUIInput* inp = new GUIInput;
	inp->init(2,3,10);
	inp->gui_panel = new_panel;
	new_panel->input.push_back(inp);
	
	gui.panels.push_back(new_panel);
	return new_panel;
}

// number of hover panel entries
#define HOVER_PANEL_NUM 19
#define HOVER_PANEL_W 15
void hover_panel_update(void* arg)
{
	GUIPanel* panel = (GUIPanel*)arg;
	// clear text entries
	for(int i=0;i<panel->text.size();i++)
		panel->text[i]->str = "";
	// update text
	unsigned int index = 2; // text index
	
	// hovering tile
	GUIText* t = panel->text[0];
	DefTile* hover_tile_def = tile_man.get(camera.hover_tile->tile_id);
	t->str = hover_tile_def->name;
	if(hover_tile_def->can_get_item)
	{
		t->str += '(';
		t->str += std::to_string(camera.hover_tile->resource);
		t->str += ')';
	}
	t->color = 0xa7;
	
	t = panel->text[1];
	int temp = camera.hover_tile->temperature - 273;
	t->str = std::to_string(temp);
	t->str += "*C";
	if(temp <= 0) t->color = 0xe7;
	if(0 < temp && temp < 30) t->color = 0xa7;
	if(temp >= 30) t->color = 0xb7;
	
	for(int i=0;i<camera.hover_structs.size();i++) // loop through hovered structures
	{
		if(index >= HOVER_PANEL_NUM) break;
		
		GUIText* txt = panel->text[index];
		DefStructure* defstr = struct_man.get(camera.hover_structs[i]->id); // retrieve structure name
		txt->str = defstr->name;
		txt->color = 0xb7;
		
		index++;
	}
	for(int i=0;i<camera.hover_units.size();i++) // loop through hovered units
	{
		if(index >= HOVER_PANEL_NUM) break;
		
		GUIText* txt = panel->text[index];
		txt->str = "";
		txt->color = 0x97;
		
		index++;
	}
	for(int i=0;i<camera.hover_items.size();i++) // loop through hovered items
	{
		if(index >= HOVER_PANEL_NUM) break;
		
		GUIText* txt = panel->text[index];
		DefItem* defitem = item_man.get_item(camera.hover_items[i]->item_id);
		DefMat* defmat = item_man.get_mat(camera.hover_items[i]->mat_id);
		
		txt->str = defmat->desc_name;
		txt->str += ' ';
		txt->str += defitem->name;
		txt->color = 0x07;
		
		index++;
	}
}

// for hovering
GUIPanel* make_hover_panel()
{
	GUIPanel* pan = new GUIPanel;
	pan->init(0,0,HOVER_PANEL_W,HOVER_PANEL_NUM+1,0x77);
	
	// make all entries
	for(int i=0;i<HOVER_PANEL_NUM;i++)
	{
		GUIText* txt = new GUIText;
		txt->init(0,i+1,HOVER_PANEL_W,0x07,"");
		txt->gui_panel = pan;
		pan->text.push_back(txt);
	}
	
	pan->set_update(hover_panel_update);
	gui.panels.push_back(pan);
	return pan;
}

// for ctrl-clicking
GUIPanel* make_ctrl_click_panel()
{
	
}
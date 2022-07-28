#include "AdviLib.h"

class SDL_SolidCircle
{
private:
	SDL_Circle cir{};
	SDL_Rect Col{};
	bool ShowCol{};
	bool Filled{};
	SDL_Renderer* ren{};
	int min{};
	int max{};
public:
	SDL_SolidCircle(SDL_Renderer* ren, SDL_Point cen_start, int rad, bool ShowCol = false, bool filled = true)
	{
		cir.center = cen_start;
		cir.radius = rad;
		this->ShowCol = ShowCol;
		Col.x = cir.center.x - rad;
		Col.y = cir.center.y - rad;
		Col.w = rad * 2;
		Col.h = rad * 2;
		this->ren = ren;
		Filled = filled;
		min = 1;
		max = 500;
	}
	const bool Collides(SDL_Rect r)
	{
		return SDL_HasIntersection(&this->Col, &r);
	}
	SDL_Circle get()
	{
		return cir;
	}
	SDL_Rect rect()
	{
		return Col;
	}
	void render()
	{
		if (Filled) SDL_FillCircle(ren, cir);
		else if (!Filled) SDL_DrawCircle(ren, cir);
		if (ShowCol)
		{
			GetRandomDrawColor(ren);
			SDL_RenderDrawRect(ren, &Col);
		}
	}
	void repos(int x, int y)
	{
		cir.center.x = x;
		cir.center.y = y;
		Col.x = x - (Col.w / 2);
		Col.y = y - (Col.h / 2);
	}
	void shrink(int count = 1)
	{
		if (cir.radius > min)
		{
			cir.radius -= count;
			Col.x += count; Col.y += count;
			Col.w -= count * 2; Col.h -= count * 2;
			while (cir.radius < min) expand();
		}
	}
	void expand(int count = 1)
	{
		if (cir.radius < max)
		{
			cir.radius += count;
			Col.x -= count; Col.y -= count;
			Col.w += count * 2; Col.h += count * 2;
			while (cir.radius > max) shrink();
		}
	}
	void ToggleShowCollider()
	{
		Toggle(ShowCol);
	}
	void ToggleFill()
	{
		Toggle(Filled);
	}
	void Bounds(int sx, int sy, int ex, int ey)
	{
		if (cir.center.x < sx) repos(sx, cir.center.y); if (cir.center.x > ex) repos(ex, cir.center.y);
		if (cir.center.y < sy) repos(cir.center.x, sy); if (cir.center.y > ey) repos(cir.center.x, ey);
	}
	/// <summary>
	/// set the min/max radius of the circle, leave parameter field blank to revert to default limits
	/// </summary>
	void RadLimits(int min = 1, int max = 500)
	{
		this->min = min;
		this->max = max;
	}

	void operator()()
	{
		render();
	}
};

/// <summary>
/// contains resolution of entire screen inside of members x(width) and y(height)
/// </summary>
const SDL_Point Screen()
{
	RECT screen;
	GetWindowRect(GetDesktopWindow(), &screen);
	SDL_Point _Screen{ screen.right, screen.bottom };
	return _Screen;
}

static bool isFullscreen = false;
/// <summary>
/// manually preform fullscreen toggle
/// </summary>
/// <param name="win">the window in use</param>
/// <param name="nFS">the position and size of the window when not fullscreened</param>
void ToggleFullscreen(SDL_Window* win, SDL_Rect nFS)
{
	Toggle(isFullscreen);
	SDL_SetWindowBordered(win, (SDL_bool)!isFullscreen);
	SDL_Rect R;
	if (isFullscreen) R = SDL_Rect{ 0, 0, Screen().x, Screen().y };
	else if (!isFullscreen) R = nFS;
	SDL_SetWindowPosition(win, R.x, R.y);
	SDL_SetWindowSize(win, R.w, R.h);
}

class SDL_Triangle
{
private:
	int size{};
	SDL_Color color{};
	SDL_Point center{};
	bool fill{};
public:
	SDL_Triangle(SDL_Point cen, int s, SDL_Color c = SDL_GetRandomColor(), bool filled = false)
	{
		center = cen;
		size = s;
		color = c;
		fill = filled;
	}

	void resize(int s)
	{
		size = s;
	}
	void repos(int x, int y)
	{
		center.x = x;
		center.y = y;
	}

	const SDL_Point get()
	{
		return center;
	}
	const int Size()
	{
		return size;
	}

	void ToggleFill()
	{
		Toggle(fill);
	}

	void draw(SDL_Renderer* ren)
	{
		SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a); int temp_x{ center.x - size };
		if (fill)
		{
			while (temp_x < (center.x + size))
			{
				SDL_RenderDrawLine(ren, center.x, center.y - size, temp_x, center.y + size);
				temp_x++;
			}
		}
		else if (!fill)
		{
			SDL_RenderDrawLine(ren, center.x - size, center.y + size, center.x, center.y - size);
			SDL_RenderDrawLine(ren, center.x + size, center.y + size, center.x, center.y - size);
			SDL_RenderDrawLine(ren, center.x - size, center.y + size, center.x + size, center.y + size);
		}
	}

	void bounds(int sx, int sy, int ex, int ey)
	{
		if ((center.x - size) < sx) repos(size, center.y); if ((center.x + size) > ex) repos((ex - size), center.y);
		if ((center.y - size) < sy) repos(center.x, size); if ((center.y + size) > ey) repos(center.x, (ey - size));
	}
};

bool SDL_LineInRect(SDL_Rect r, SDL_Line l)
{
	if (SDL_IntersectRectAndLine(&r, &l.x1, &l.y1, &l.x2, &l.y2)) return true;
	else return false;
}

class LogicGate
{
	const bool t = true;
	const bool f = false;
public:
	bool a, b;

	LogicGate(bool a = random(1), bool b = random(1))
	{
		this->a = a;
		this->b = b;
	}

	bool AND() { return (a && b); }
	bool OR_() { return (a || b); }
	bool XOR() { return (a != b); }

	void a_cout() { std::cout << std::boolalpha << a << std::noboolalpha; }
	void b_cout() { std::cout << std::boolalpha << b << std::noboolalpha; }
};

string boolalpha(bool b)
{
	if (b) return "true";
	else if (!b) return "false";
	else return "error!";
}

string hex(unsigned int i)
{
	string res{};
	buf b;
	sprintf_s(b, "%X", i);
	res = b;
	return res;
}

string bin4(unsigned int i) { std::bitset<4>num = i; return num.to_string(); }
string bin8(unsigned int i) { std::bitset<8>num = i; return num.to_string(); }
string bin16(unsigned int i) { std::bitset<16>num = i; return num.to_string(); }
string bin32(unsigned int i) { std::bitset<32>num = i; return num.to_string(); }
string bin64(unsigned int i) { std::bitset<64>num = i; return num.to_string(); }

class timed_signal
{
private:
	bool sig{}, set{};
	Timer curtime{};
	float sigtime{}, on_time{};

public:
	timed_signal()
	{
	}

	timed_signal(float off_time, float on_time)
	{
		sigtime = off_time;
		this->on_time = on_time;
		sig = false;
		set = false;
	}

	void set_times(float on, float off)
	{
		on_time = on;
		sigtime = off;
	}

	void begin()
	{
		curtime.reset();
	}

	bool check()
	{
		if (curtime.elapsed() >= sigtime)
		{
			sig = true;
			set = true;
			curtime.reset();
		}
		else if (set && curtime.elapsed() >= on_time)
		{
			sig = false;
			set = false;
			curtime.reset();
		}
		return sig;
	}
};

class SDL_Wire
{
private:
	SDL_Line wire{};
	timed_signal sig{};
	SDL_Color onc{}, offc{}, inuse{};
public:
	SDL_Wire(SDL_Point sp, SDL_Point ep, float ontime, float offtime, SDL_Color on_color = QuickColor_GRN, SDL_Color off_color = QuickColor_RED)
	{
		wire.x1 = sp.x; wire.y1 = sp.y;
		wire.x2 = ep.x; wire.y2 = ep.y;
		onc = on_color; offc = off_color;
		inuse = QuickColor_RED;
		sig.set_times(ontime, offtime);
	}

	bool check_signal()
	{
		if (sig.check()) inuse = onc;
		else if (!sig.check()) inuse = offc;
		return sig.check();
	}

	void draw(SDL_Renderer* ren)
	{
		SDL_SetRenderDrawColor(ren, inuse.r, inuse.g, inuse.b, 255);
		SDL_RenderDrawLine(ren, wire.x1, wire.y1, wire.x2, wire.y2);
	}
};

class CmdTools
{
private:
	bool show_cmd{};
	TCHAR title[MAX_PATH]{};
	buf b{};

public:
	CmdTools()
	{
		GetConsoleTitle(title, MAX_PATH);
		sprintf_s(b, "%ws", title);
		show_cmd = true;
	}

	CmdTools(bool showcmd)
	{
		GetConsoleTitle(title, MAX_PATH);
		sprintf_s(b, "%ws", title);
		show_cmd = showcmd;
		if (show_cmd) ShowWindow(GetConsoleWindow(), SW_SHOW);
		else if (!show_cmd) ShowWindow(GetConsoleWindow(), SW_HIDE);
	}

	const string get_name()
	{
		return b;
	}

	void focus()
	{
		ShowWindow(GetConsoleWindow(), SW_SHOW);
	}

	void set_name(string title)
	{
		SetConsoleTitleA(title.c_str());
		GetConsoleTitle(this->title, MAX_PATH);
		sprintf_s(b, "%ws", this->title);
	}

	void toggle()
	{
		Toggle(show_cmd);
		if (show_cmd) ShowWindow(GetConsoleWindow(), SW_SHOW);
		else if (!show_cmd) ShowWindow(GetConsoleWindow(), SW_HIDE);
	}

	void cls()
	{
		system("cls");
	}

	bool showing()
	{
		return IsWindowVisible(GetConsoleWindow());
	}
};

SDL_Renderer* SDL_VsyncVariableGpuRenderer(SDL_Window* win, bool vsync)
{
	if (vsync) return SDL_GpuRenderer_Vsync(win);
	else return SDL_GpuRenderer(win);
}

class SDL_Gamepad // figure out some form of use for SDL_CONTROLLERDEVICEADDED and SDL_CONTROLLERDEVICEREMOVED
{
private:
	SDL_GameController* gamepad{};
	int DeadZone{}; // stick deadzone
	int TrigZone{}; // trigger deadzone
	int ConnectionIndex{}; // the number of the gamepad connection
	bool initFlag{}; // only true during constructor process to tell connection() not to perform an initial check
	int initPass{}; // only 0 during the constructor process to tell connection() not to perform an initial check

public:
	SDL_Gamepad(int index = 0, int deadzone = 20000, int trigzone = 8000)
	{
		initPass = 0;
		initFlag = true;
		init(index);
		DeadZone = deadzone;
		TrigZone = trigzone;
	}

	void clear_init_flags()
	{
		if (initPass != 1) { initFlag = false; initPass = 1; }
		//if (initPass == 0 && initFlag) initFlag = false;
	}

	/// <summary>
	/// checks if a controller has been added or removed, requires clear_init_flags() to be called at end of loop
	/// </summary>
	/// <returns>1 = controller added, 0 = controller removed, -1 = nothing happened</returns>
	int connection(SDL_Event& e)
	{
		int ret{};

		if (!initFlag && e.type == SDL_CONTROLLERDEVICEREMOVED) { ret = 0; }
		else if (!initFlag && e.type == SDL_CONTROLLERDEVICEADDED) { ret = 1; }
		else ret = -1;

		return ret;
	}

	const int Index()
	{
		return ConnectionIndex;
	}

	void init(int index = 0)
	{
		if (SDL_IsGameController(index))
		{
			gamepad = SDL_GameControllerOpen(index);
			ConnectionIndex = SDL_GameControllerGetPlayerIndex(gamepad);
			std::cout << "controller " << index << " as set gamepad " << Index() + 1 << "\n";
		}
		else
		{
			std::cout << index << " is not a valid gamepad!\n";
		}
	}

	void set_stick_dead_zone(int deadzone) { DeadZone = deadzone; }
	void set_trigger_dead_zone(int trigzone) { TrigZone = trigzone; }

	bool InputLS_UP(SDL_Event e) { static bool b{}; if (e.type == SDL_CONTROLLERAXISMOTION) { if (e.caxis.axis == 1) { if (e.caxis.value < -DeadZone) { b = true; } else b = false; } else b = false; } return b; }
	bool InputLS_LEFT(SDL_Event e) { static bool b{}; if (e.type == SDL_CONTROLLERAXISMOTION) { if (e.caxis.axis == 0) { if (e.caxis.value < -DeadZone) { b = true; } else b = false; } else b = false; } return b; }
	bool InputLS_DOWN(SDL_Event e) { static bool b{}; if (e.type == SDL_CONTROLLERAXISMOTION) { if (e.caxis.axis == 1) { if (e.caxis.value > DeadZone) { b = true; } else b = false; } else b = false; } return b; }
	bool InputLS_RIGHT(SDL_Event e) { static bool b{}; if (e.type == SDL_CONTROLLERAXISMOTION) { if (e.caxis.axis == 0) { if (e.caxis.value > DeadZone) { b = true; } else b = false; } else b = false; } return b; }

	bool InputLS_NONE(SDL_Event e) { static bool b{}; if (e.type == SDL_CONTROLLERAXISMOTION) { if (e.caxis.value > -DeadZone && e.caxis.value < DeadZone) { b = true; } else b = false; } return b; }

	bool InputRS_UP(SDL_Event e) { static bool b{}; if (e.type == SDL_CONTROLLERAXISMOTION) { if (e.caxis.axis == 3) { if (e.caxis.value < -DeadZone) { b = true; } else b = false; } else b = false; } return b; }
	bool InputRS_LEFT(SDL_Event e) { static bool b{}; if (e.type == SDL_CONTROLLERAXISMOTION) { if (e.caxis.axis == 2) { if (e.caxis.value < -DeadZone) { b = true; } else b = false; } else b = false; } return b; }
	bool InputRS_DOWN(SDL_Event e) { static bool b{}; if (e.type == SDL_CONTROLLERAXISMOTION) { if (e.caxis.axis == 3) { if (e.caxis.value > DeadZone) { b = true; } else b = false; } else b = false; } return b; }
	bool InputRS_RIGHT(SDL_Event e) { static bool b{}; if (e.type == SDL_CONTROLLERAXISMOTION) { if (e.caxis.axis == 2) { if (e.caxis.value > DeadZone) { b = true; } else b = false; } else b = false; } return b; }

	bool InputLT(SDL_Event e) { static bool b{}; if (e.type == SDL_CONTROLLERAXISMOTION) { if (e.caxis.axis == 4) { if (e.caxis.value > TrigZone) { b = true; } else b = false; } else b = false; } return b; }
	bool InputRT(SDL_Event e) { static bool b{}; if (e.type == SDL_CONTROLLERAXISMOTION) { if (e.caxis.axis == 5) { if (e.caxis.value > TrigZone) { b = true; } else b = false; } else b = false; } return b; }

	bool InputA(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_A) { b = true; } else b = false; } return b; }
	bool InputB(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_B) { b = true; } else b = false; } return b; }
	bool InputX(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_X) { b = true; } else b = false; } return b; }
	bool InputY(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_Y) { b = true; } else b = false; } return b; }

	bool InputUP(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP) { b = true; } else b = false; } return b; }
	bool InputDOWN(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) { b = true; } else b = false; } return b; }
	bool InputLEFT(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) { b = true; } else b = false; } return b; }
	bool InputRIGHT(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) { b = true; } else b = false; } return b; }

	bool RELEASE_DPAD_UP(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONUP) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP) { b = true; } else b = false; } return b; }

	bool InputLB(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER) { b = true; } else b = false; } return b; }
	bool InputRB(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) { b = true; } else b = false; } return b; }

	bool InputGUIDE(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_GUIDE) { b = true; } else b = false; } return b; }
	bool InputSTART(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_START) { b = true; } else b = false; } return b; }
	bool InputSELECT(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_BACK) { b = true; } else b = false; } return b; }

	bool InputLS(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSTICK) { b = true; } else b = false; } return b; }
	bool InputRS(SDL_Event e) { bool b{}; if (e.type == SDL_CONTROLLERBUTTONDOWN) { if (e.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSTICK) { b = true; } else b = false; } return b; }

	void close()
	{
		SDL_GameControllerClose(gamepad);
	}
};

void SDL_GetCurrentMousePosition(SDL_Event e, SDL_Point& p)
{
	if (e.type == SDL_MOUSEMOTION)
	{
		p.x = e.button.x;
		p.y = e.button.y;
	}
}

class List
{
private:
	std::fstream file{};
	string name{};
	std::vector<string>_list{};
	bool UsingFile{};

	void dump()
	{
		if (UsingFile)
		{
			file.open(name.c_str());
			if (_list.size() > 0) { _list.pop_back(); _list.shrink_to_fit(); }
			while (file)
			{
				string temp{};
				std::getline(file, temp);
				_list.push_back(temp);
			}
			_list.pop_back(); _list.shrink_to_fit(); // remove empty space at the end
			file.close();
		}
		else std::cout << "dump() error: not using file!\n";
	}

	void pop()
	{
		_list.pop_back();
		_list.shrink_to_fit();
	}

	void Erase(string item)
	{
		_list.erase(std::remove(_list.begin(), _list.end(), item), _list.end());
	}

public:
	List()
	{
		UsingFile = false;
	}

	List(string strname)
	{
		UsingFile = true;
		name = strname;
		if (FileSize(name.c_str()) > 0) dump();
	}

	const bool Empty()
	{
		return _list.empty();
	}

	const size_t size()
	{
		return _list.size();
	}

	void attach_new_list(string strname)
	{
		name = strname;
		if (!UsingFile) UsingFile = true;
		if (FileSize(name.c_str()) > 0) dump();
	}

	void cout()
	{
		for (unsigned int i = 0; i < _list.size(); i++)
		{
			std::cout << _list[i] << "\n";
		}
	}

	std::vector<string>list()
	{
		return _list;
	}

	void clear()
	{
		while (_list.size() > 0)
		{
			_list.pop_back();
			_list.shrink_to_fit();
		}
		file.open(name.c_str(), std::fstream::out | std::fstream::trunc); std::cout << ""; file.close();
	}

	int compare(string item)
	{
		int num = -1;
		for (unsigned int i = 0; i < _list.size(); i++)
		{
			if (strfind(_list[i], item))
			{
				num = i;
				break;
			}
		}
		return num;
	}

	void overwrite()
	{
		if (UsingFile)
		{
			file.open(name.c_str(), std::fstream::out | std::fstream::trunc); file << ""; file.close();
			for (unsigned int i = 0; i < _list.size(); i++)
			{
				file.open(name.c_str(), std::fstream::app);
				file << _list[i] << std::endl;
				file.close();
			}
			std::cout << name << " overwritten!\n";
		}
		else std::cout << "overwrite() error: not using file!\n";
	}

	void operator+=(string s) { _list.push_back(s); }
	void operator-=(string s) { Erase(s); }
	void operator--() { pop(); }
};

bool SDL_IsGamepad(int index)
{
	return SDL_IsGameController(index);
}

class SDL_Image
{
private:
	SDL_Surface* sur{};
	SDL_Texture* tex{};
	SDL_Renderer* ren{};
	SDL_Rect rect{};
	string FileName{};
public:
	SDL_Image(SDL_Renderer* ren, string name, SDL_Rect pos)
	{
		this->ren = ren;
		FileName = name;
		if (strfind(FileName, ".png") || strfind(FileName, ".PNG") || strfind(FileName, ".jpg") || strfind(FileName, ".JPG"))
		{
			sur = IMG_Load(FileName.c_str());
			tex = SDL_CreateTextureFromSurface(ren, sur);
		}
		else if (strfind(FileName, ".bmp") || strfind(FileName, ".BMP"))
		{
			sur = SDL_LoadBMP(FileName.c_str());
			tex = SDL_CreateTextureFromSurface(ren, sur);
		}
		else std::cout << "SDL_Image::SDL_Image(): FILE FORMAT OF \"" << FileName << "\" IS NOT A VALID IMAGE FORMAT!\n";
		rect = pos;
		SDL_QueryTexture(tex, NULL, NULL, 0, 0);
	}

	SDL_Rect Rect() { return rect; }

	void render()
	{
		SDL_RenderCopy(ren, tex, NULL, &rect);
	}

	void destroy()
	{
		SDL_FreeSurface(sur);
		SDL_DestroyTexture(tex);
		sur = nullptr;
		tex = nullptr;
	}
};

class SDL_WindowManager
{
private:
	SDL_Window* win{};
	SDL_Renderer* ren{};
	SDL_Rect WinRect{};
	SDL_Surface* icon{};
	Uint32 flags{};
	string name{};
	bool Vsync{};
	bool GpuRender{}; // true mean gpu render, false means cpu render
public:
	SDL_WindowManager(const char* wName, int x, int y, int w, int h, Uint32 flags, bool vsync = false, bool gpu_render = false, string ico = "N/A")
	{
		name = wName;
		WinRect = {x,y,w,h};
		this->flags = flags;
		Vsync = vsync;
		GpuRender = gpu_render;
		win = SDL_CreateWindow(wName, x, y, w, h, flags);
		if (gpu_render && vsync) ren = SDL_GpuRenderer_Vsync(win);
		else if (gpu_render && !vsync) ren = SDL_GpuRenderer(win);
		else if (!gpu_render && !vsync) ren = SDL_CpuRenderer(win);
		else if (!gpu_render && vsync) ren = SDL_CpuRenderer_Vsync(win);
		if (!strfind(ico, "N/A"))
		{
			icon = IMG_Load(ico.c_str());
			SDL_SetWindowIcon(win, icon);
		}
	}

	void Icon(string ico)
	{
		icon = IMG_Load(ico.c_str());
		SDL_SetWindowIcon(win, icon);
	}

	SDL_Point Size() { return SDL_Point{ WinRect.w, WinRect.h }; }

	SDL_Renderer* Ren() { return ren; }

	void SetTitle(const char* name)
	{
		this->name = name;
		SDL_SetWindowTitle(win, name);
	}

	void destroy()
	{
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
	}

	SDL_Window* operator()()
	{
		return win;
	}
};
#include "SDL_Test.h"

#define PI 3.1415926535
#define P2 PI / 2
#define P3 3 * PI / 2
#define DR 0.0174533

float px, py, pdx, pdy, pa;

#define DumpTextboxes(src, des) for (unsigned int i = 0; i < src.size(); i++) { des.push_back(src[i]); src.pop_back(); src.shrink_to_fit(); }

void CheckController(SDL_Event e, SDL_Gamepad& gamepad, unsigned int& I)
{
	if (gamepad.connection(e) >= 0)
	{
		std::cout << "connection switched: ";
		if (gamepad.connection(e) == 1) { std::cout << "controller plugged!\n"; gamepad.init(I); }
		if (gamepad.connection(e) == 0) { I = gamepad.Index(); std::cout << "controller removed!\n"; gamepad.close(); }
	}
}

int mapX = 8, mapY = 8, mapS = 64;

int map[]
{
	//  1, 2, 3, 4, 5, 6, 7, 8
		1, 1, 1, 1, 1, 1, 1, 1, // 1
		1, 0, 1, 0, 0, 8, 0, 1, // 2
		1, 0, 4, 6, 0, 0, 0, 1, // 3
		1, 0, 0, 0, 0, 2, 0, 1, // 4
		1, 0, 0, 0, 0, 0, 0, 1, // 5
		1, 0, 0, 3, 0, 1, 0, 1, // 6
		5, 0, 0, 0, 0, 0, 0, 7, // 7
		1, 5, 1, 1, 1, 1, 7, 1  // 8
};

void drawPlayer(SDL_Renderer* ren)
{
	SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
	SDL_Circle pcir{ px, py, 5 };
	SDL_FillCircle(ren, pcir);
	SDL_RenderDrawLine(ren, px, py, px + pdx * 5, py + pdy * 5);
}

void drawMap(SDL_Renderer* ren)
{
	int x, y, xo, yo;
	for (y = 0; y < mapY; y++)
	{
		for (x = 0; x < mapX; x++)
		{
			if (map[y * mapY + x] != 0) SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
			else SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
			xo = x * mapS; yo = y * mapS;
			SDL_Rect map_rect{ xo + 1, yo + 1, mapS - 1, mapS - 1 };
			SDL_RenderFillRect(ren, &map_rect);
		}
	}
}

float dist(float ax, float ay, float bx, float by, float ang)
{
	return (sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay)));
}

int fov = 90;
int ys = 1;

void drawRays(SDL_Renderer* ren)
{
	Uint8 R, G, B;
	int r, mx, my, mp, dof, mh, mv; float rx, ry, ra, xo, yo, disT;
	ra = pa - DR * (fov / 2); if (ra < 0) { ra += 2 * PI; } if (ra > 2 * PI) { ra -= 2 * PI; }
	for (r = 0; r < fov; r++)
	{
		dof = 0;
		float disH = 10000000, hx = px, hy = py;
		float aTan = -1 / tan(ra);
		if (ra > PI) { ry = (((int)py >> 6) << 6) - 0.0001; rx = (py - ry) * aTan + px; yo = -64; xo = -yo * aTan; }
		if (ra < PI) { ry = (((int)py >> 6) << 6) + 64; rx = (py - ry) * aTan + px; yo = 64; xo = -yo * aTan; }
		if (ra == 0 || ra == PI) { rx = px; ry = py; dof = 8; }
		while (dof < 8)
		{
			mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx;
			if (mp > 0 && mp < mapX * mapY && map[mp] > 0) { mh = map[mp]; hx = rx; hy = ry; disH = dist(px, py, hx, hy, ra); dof = 8; }
			else { rx += xo; ry += yo; dof += 1; }
		}

		dof = 0; // was here
		float disV = 10000000, vx = px, vy = py;
		float nTan = -tan(ra);
		if (ra > P2 && ra < P3) { rx = (((int)px >> 6) << 6) - 0.0001; ry = (px - rx) * nTan + py; xo = -64; yo = -xo * nTan; }
		if (ra < P2 || ra > P3) { rx = (((int)px >> 6) << 6) + 64; ry = (px - rx) * nTan + py; xo = 64; yo = -xo * nTan; }
		if (ra == 0 || ra == PI) { rx = px; ry = py; dof = 8; }
		while (dof < 8)
		{
			mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx;
			if (mp > 0 && mp < mapX * mapY && map[mp] > 0) { mv = map[mp]; vx = rx; vy = ry; disV = dist(px, py, vx, vy, ra); dof = 8; }
			else { rx += xo; ry += yo; dof += 1; }
		}
		if (disV < disH) { rx = vx; ry = vy; disT = disV; R = 240; G = 0; B = 0; if (mv == 2) { R = 0; G = 0; B = 240; } if (mv == 3) { R = 0; G = 240; B = 0; } if (mv == 4) { R = 240; G = 0; B = 240; } if (mv == 5) { R = 240; G = 240; B = 0; } if (mv == 6) { R = 255; G = 127; B = 220; } if (mv == 7) { R = 200; G = 120; B = 100; } if (mv == 8) { R = 255; G = 255; B = 255; } }
		if (disH < disV) { rx = hx; ry = hy; disT = disH; R = 200; G = 0; B = 0; if (mh == 2) { R = 0; G = 0; B = 200; } if (mh == 3) { R = 0; G = 200; B = 0; } if (mh == 4) { R = 200; G = 0; B = 200; } if (mh == 5) { R = 200; G = 200; B = 0; } if (mh == 6) { R = 255; G = 100; B = 220; } if (mh == 7) { R = 180; G = 100; B = 80; } if (mh == 8) { R = 220; G = 220; B = 220; } }
		SDL_SetRenderDrawColor(ren, R, G, B, 255);
		SDL_RenderDrawLine(ren, px, py, rx, ry);
		// 3D rendering starts here
		float ca = pa - ra; if (ca < 0) { ca += 2 * PI; } if (ca > 2 * PI) { ca -= 2 * PI; } disT = disT * cos(ca);
		int v3D{ 360 };
		float lineH = (mapS * v3D) / disT; if (lineH > v3D) lineH = v3D;
		float lineO = (v3D / 2) - lineH / 2;
		int xinc{ 520 }, xscale{ 8 }, yscale{ ys };
		float lowpoint{ 0 };
		lowpoint = lineH;
		SDL_RenderSetScale(ren, xscale, yscale);
		SDL_RenderDrawLine(ren, (r * xscale + xinc) / xscale, (lineO) / yscale, (r * xscale + xinc) / xscale, (lineH + lineO) / yscale); // walls
		SDL_SetRenderDrawColor(ren, 0, 255, 255, 255); SDL_RenderDrawLine(ren, (r * xscale + xinc) / xscale, (lineO) / yscale, (r * xscale + xinc) / xscale, 0); // sky
		SDL_SetRenderDrawColor(ren, R / 2, G / 2, B / 2, 255); SDL_RenderDrawLine(ren, (r * xscale + xinc) / xscale, (lineH + lineO) / yscale, (r * xscale + xinc) / xscale, (lineH + lineO) + lowpoint ); // simple reflective floor
		SDL_RenderSetScale(ren, 1, 1);
		// 3D rendering stops here
		ra += DR; if (ra < 0) { ra += 2 * PI; } if (ra > 2 * PI) { ra -= 2 * PI; }
	}
}

int main(int argc, char* args[])
{
	// initialization and window creation
	CmdTools cmd{ true };
	InitSDL(); const string wTitle{ "SDL Raycaster proto 0" };
	SDL_WindowManager WM{ wTitle.c_str(), 0, 0, 1920, 1080, SDL_WINDOW_SHOWN };
	px = 300; py = 300; pdx = cos(pa) * 5; pdy = sin(pa) * 5;

	//cmd.toggle();

	// controller variables
	SDL_Gamepad gamepad{};

	// fonts
	const int TextSize = 25;
	TTF_Font* Font{ TTF_OpenFont("monofonto.ttf", TextSize) };

	SDL_Textbox playerPos{ WM.Ren(), Font, SDL_Rect{0, TextSize * 22, 0, 0}, QuickColor_CYN };
	SDL_Textbox playerAng{ WM.Ren(), Font, SDL_Rect{0, TextSize * 23, 0, 0}, QuickColor_CYN };
	SDL_Textbox engineFps{ WM.Ren(), Font, SDL_Rect{0, TextSize * 24, 0, 0}, QuickColor_CYN };
	SDL_Textbox playerFov{ WM.Ren(), Font, SDL_Rect{0, TextSize * 25, 0, 0}, QuickColor_CYN };
	SDL_Textbox engineRam{ WM.Ren(), Font, SDL_Rect{0, TextSize * 26, 0, 0}, QuickColor_CYN };
	SDL_Textbox engine_ys{ WM.Ren(), Font, SDL_Rect{0, TextSize * 27, 0, 0}, QuickColor_CYN };

	// main loop
	bool q{ false }; SDL_Event e{}; SDL_Point M{}; unsigned int I{}; FpsCounter fps;
	while (!q)
	{
		fps.equate(); WM.SetTitle(string(wTitle).c_str());

		// textbox buffers
		playerPos.fBuffer_Double("px: %f, py: %f", px, py);
		playerAng.fBuffer_Single("rot: %f", pa);
		engineFps.iBuffer_Single("fps: %i", fps.get());
		playerFov.iBuffer_Single("fov: %i", fov);
		engineRam.iBuffer_Double("mem: %imb/%imb", AppRamUsage(), TotalRamCount());
		engine_ys.iBuffer_Single("yscale: %i", ys);

		// start render phase
		SDL_SetRenderDrawColor(WM.Ren(), 60, 60, 60, 255); SDL_RenderClear(WM.Ren());

		// draw game
		drawMap(WM.Ren());
		drawRays(WM.Ren());
		drawPlayer(WM.Ren());

		// draw textboxes
		playerPos(); playerAng(); engineFps(); playerFov(); engineRam(); engine_ys();

		// end render phase
		fps.RenderFrame(WM.Ren());

		// handle events
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT) q = true;

			SDL_GetCurrentMousePosition(e, M);

			if (e.type == SDL_KEYDOWN)
			{
				int xo = 0; if (pdx < 0) { xo = -20; }
				else xo = 20;
				int yo = 0; if (pdy < 0) { yo = -20; }
				else yo = 20;
				int ipx = px / 64, ipx_add_xo = (px + xo) / 64, ipx_sub_xo = (px - xo) / 64;
				int ipy = py / 64, ipy_add_yo = (py + yo) / 64, ipy_sub_yo = (py - yo) / 64;

				switch (e.key.keysym.sym)
				{
				case SDLK_i: if (ys > 0) ys--; break;
				case SDLK_o: ys++; break;
				case SDLK_q: if (fov > 30) fov -= 2; break;
				case SDLK_e: if (fov < 360) fov += 2; break;
				case SDLK_a: pa -= 0.1; if (pa < 0) { pa += 2 * PI; } pdx = cos(pa) * 5; pdy = sin(pa) * 5; break;
				case SDLK_d: pa += 0.1; if (pa > 2 * PI) { pa -= 2 * PI; } pdx = cos(pa) * 5; pdy = sin(pa) * 5; break;
				case SDLK_w: if (map[ipy * mapX + ipx_add_xo] == 0) px += pdx; if (map[ipy_add_yo * mapX + ipx] == 0) py += pdy; break;
				case SDLK_s: if (map[ipy * mapX + ipx_sub_xo] == 0) px -= pdx; if (map[ipy_sub_yo * mapX + ipx] == 0) py -= pdy; break;
				case SDLKES: q = true; break;
				}
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				switch (e.button.button)
				{
				case SDLMBL: break;
				case SDLMBR: break;
				case SDLMBM: break;
				}
			}
			else if (e.type == SDL_MOUSEWHEEL)
			{
				if (SDL_WHEEL_UP(e)) {}
				else if (SDL_WHEEL_DOWN(e)) {}
			}
			CheckController(e, gamepad, I);

			if (gamepad.InputB(e)) {}
			if (gamepad.InputA(e)) {}
			if (gamepad.InputX(e)) {}
			if (gamepad.InputY(e)) {}

			if (gamepad.InputLS_UP(e)) {}
			if (gamepad.InputLS_DOWN(e)) {}

			if (gamepad.InputUP(e)) {}
			if (gamepad.InputDOWN(e)) {}

			if (gamepad.InputSTART(e)) {}
			if (gamepad.InputSELECT(e)) {}

			if (gamepad.InputGUIDE(e)) { q = true; }
		}

		// end of frame functions
		gamepad.clear_init_flags();
	}

	// exit code
	gamepad.close();
	WM.destroy();
	SDL_Textboxes Tbs{ playerPos, playerAng, engineFps, playerFov, engineRam, engine_ys };
	FontList Fonts{ Font };
	SurList Surs{};
	TexList Texs{};
	QuitSDL(Tbs, Fonts, Surs, Texs);
	if (!cmd.showing()) cmd.toggle();
	return 0;
}
/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje podstawy korzystania z biblioteki SDL:
 * rysowanie pikseli, obsługa zdarzeń - klawiatura i mysz, zapis obrazu do bitmapy
 *
 * program pozwala rysować kolorem zielonym na czarnym ekranie (ruszanie myszką z wciśniętym lewym klawiszem)
 * możliwe jest zapisanie obrazka (pod przypadkową nazwą) przez wciśnięcie klawisza "S"
 * opuszczenie programu po naciśnięciu "Esc"
 *
 * kompilacja: gcc `sdl-config --cflags --libs` rysownik_sdl.c
 *
 * opis API SDL: http://www.libsdl.org/cgi/docwiki.cgi/SDL_20API
 * więcej informacji o SDL po polsku: http://kodowanie.ath.cx/piero/
 *
 */

#include <SDL.h>

void namaluj_pixel(SDL_Surface *ekran, int x, int y, Uint8 R, Uint8 G, Uint8 B){

	// tworzymy wskaźnik do zadanego piksela
	Uint8 *p = (Uint8 *)ekran->pixels + y * ekran->pitch + x * ekran->format->BytesPerPixel;

	// obsługujemy wpisywanie barw w standardach RGB i BGR
	if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
		p[0] = R; p[1] = G; p[2] = B; 
	} else {
		p[0] = B; p[1] = G; p[2] = R; 
	}
	
	// aktualizujemy punkt na ekranie
	SDL_UpdateRect(ekran, x, y, 1, 1);
}

int main() {
	// inicjalizacja biblioteki SDL
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTTHREAD)!=0) {
		printf("Błąd inicjalizacji SDL: %s/n",SDL_GetError());
		return -1;
	}


	// tworzymy ekran
	SDL_Surface *screen;
	screen = SDL_SetVideoMode(1024,768,24,SDL_HWSURFACE|SDL_FULLSCREEN);
	// vesafb wymaga SDL_SWSURFACE zamiast SDL_HWSURFACE, dziala np. dla
	//  screen = SDL_SetVideoMode(1024,768,24,SDL_SWSURFACE|SDL_FULLSCREEN);
	// zobacz tez SdlVideoStart() w sdl.c w openttd

	// tworzymy unie przechowująca informacje o zdarzeniach
	SDL_Event zdarzenie;
	
	// pozbywamy się wszystkich istniejących zdarzeń
	while(SDL_PollEvent(&zdarzenie));


	// w nieskończonej pętli czekamy na zdarzenia
	while (1) {
		SDL_WaitEvent(&zdarzenie);
		switch (zdarzenie.type) {
			case SDL_MOUSEMOTION: // ruch myszka
				if ( SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1) )
					// gdy wciśnięty lewy klawisz
					namaluj_pixel(screen, zdarzenie.motion.x, zdarzenie.motion.y, 0, 255, 0);
					// malujemy na zielono
				break;
			case SDL_KEYDOWN:
				// naciśnięcie Esc = wyjście z programu
				if (zdarzenie.key.keysym.sym == SDLK_ESCAPE)
					goto koniec; // stosowanie instrukcji goto jest niezalecane,
					             // jednak w tym wypadku raczej uzasadnione (można by jednak użyć np. atexit i exit)
				// naciśnięcie S = zapisanie ekranu jako BMP z unikalną nazwą ...
				else if (zdarzenie.key.keysym.sym == SDLK_s)
					SDL_SaveBMP(screen, tempnam(".", ""));
				break;
		}
	} 


koniec:
	// zakończenie pracy biblioteki SDL
	SDL_Quit();
	return 0;
}

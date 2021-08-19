/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje podstawy korzystania z biblioteki SDL:
 * wyświetlanie bitmap, obsługa zdarzeń - klawiatura i tworzenie własnych, timery, czyszczenie ekranu
 *
 * program wyświetla kolejne pliki BMP
 * obrazki zmieniają się co 4 sekundy, możliwe przyspieszenie przez naciśnięcie dowolnego klawisza
 * opuszczenie programu po naciśnięciu "Esc" lub wyświetleniu wszystkich obrazków
 *
 * kompilacja: gcc `sdl-config --cflags --libs` pokaz_slajdow_sdl.c
 * (wymagany pakiet: libsdl1.2-dev)
 *
 */
 
#include <SDL.h>

void wyswietl_bitmape(char *file, SDL_Surface *ekran) {
	SDL_Surface *image;
	SDL_Rect dest;

	// wczytanie obrazka
	image = SDL_LoadBMP(file);
	if ( image == NULL ) {
		fprintf(stderr, "Nie można wczytać %s: %s\n", file, SDL_GetError());
		return;
	}

	// ustalenie obszaru na ekranie na którym ma być umieszczony obrazek
	// x,y - górny lewy róg obrazka, ustawiamy tak aby obrazek bł na środku ekranu
	dest.x = (ekran->w - image->w)/2;
	dest.y = (ekran->h - image->h)/2;

	// w,h - wysokość i szerokość obrazka
	dest.w = image->w;
	dest.h = image->h;

	// skopiowanie obrazka
	SDL_BlitSurface(image, NULL, ekran, &dest);

	// aktualizacja wybranego fragmentu ekranu
	SDL_UpdateRects(ekran, 1, &dest);

	// usuniecie z pamieci tymczasowego ekranu (obrazka)
	SDL_FreeSurface(image);
}

void wyczysc(SDL_Surface *ekran, SDL_Surface *pusty, SDL_Rect *dest) {
	// skopiowanie pustego ekranu
	SDL_BlitSurface(pusty, NULL, ekran, dest);

	// aktualizacja wybranego fragmentu ekranu
	SDL_UpdateRects(ekran, 1, dest);
}

Uint32 wyslij_zdarzenie(Uint32 interval, void *param) {
	SDL_PushEvent(param);
	return interval;
}

int main() {
	// inicjalizacja bliblioteki SDL
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTTHREAD)!=0) {
		printf("Błąd inicjalizacji SDL: %s/n",SDL_GetError());
		return -1;
	}


	// tworzymy ekran
	SDL_Surface *screen;
	screen = SDL_SetVideoMode(1024,768,24,SDL_HWSURFACE|SDL_FULLSCREEN);
	// vesafb wymaga SDL_SWSURFACE zamiast SDL_HWSURFACE, działa np. dla
	//  screen = SDL_SetVideoMode(1024,768,24,SDL_SWSURFACE|SDL_FULLSCREEN);
	// zobacz tez SdlVideoStart() w sdl.c w openttd

	// tworzymy pusty ekran - przydatny do czyszczenia
	SDL_Surface *black = SDL_CreateRGBSurface(SDL_SWSURFACE, screen->w, screen->h, 24,
		0xff000000, 0x00ff0000, 0x0000ff00, 0x00000000);
	SDL_Rect black_dest; black_dest.x=0; black_dest.y=0; black_dest.w=screen->w; black_dest.h=screen->h;


	// tworzymy unie przechowująca informacje o zdarzeniach
	SDL_Event zdarzenie_in, zdarzenie_out;
	
	// tworzymy zdarzenie typu naciśniecie klawisza
	zdarzenie_out.type = SDL_KEYDOWN;
	
	// pozbywamy się wszystkich istniejących zdarzeń
	while(SDL_PollEvent(&zdarzenie_in));
	
	// ustawiamy timer generujący zdarzenie_out co 4s
	SDL_TimerID timer = SDL_AddTimer(4000, wyslij_zdarzenie, &zdarzenie_out);


	// tablica wskaźników do ścieżek określających położenia obrazków
	char *nazwy[4];
	nazwy[0] = "./obraz0.bmp";
	nazwy[1] = "./obraz1.bmp";
	nazwy[2] = "./obraz2.bmp";
	nazwy[3] = "./obraz3.bmp";
	
	int i;
	for (i=0; i<4; i++) {
		// wyświetlamy obrazek
		wyswietl_bitmape(nazwy[i], screen);
		
		// czekamy na zdarzenie - naciśniecie klawisza
		do {
			SDL_WaitEvent(&zdarzenie_in);
		} while (zdarzenie_in.type != SDL_KEYDOWN);
		// moglibyśmy też czekać określoną ilość czasu (np. 4s): SDL_Delay(4000);
		
		// gdy nacisniety zostal Esc przechodzimy do końca programu ...
		if (zdarzenie_in.key.keysym.sym == SDLK_ESCAPE)
			break;
		
		// czyszczenie ekranu
		wyczysc(screen, black, &black_dest);
	}
	
	// zakończenie pracy biblioteki SDL
	SDL_Quit();
	return 0;
}

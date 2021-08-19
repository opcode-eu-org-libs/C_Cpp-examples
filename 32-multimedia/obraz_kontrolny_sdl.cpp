/** Program generuje obraz kontrolny z informacją o czasie w oparciu o dostarczona "tapete"
 * (np. http://commons.wikimedia.org/wiki/Image:Philips_PM5544.svg przekonwertowany do bmp)
 *
 * kompilacja: g++ -lSDL -lSDL_ttf -lSDL_gfx obraz_kontrolny.cpp
 * (wymagane pakiety:  libsdl1.2-dev  libsdl-ttf2.0-dev  libsdl-gfx1.2-dev)
 */

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_mutex.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#define DEBUG(a) printf("%s:%d debug: %s\n", __FILE__, __LINE__, (a))

// stale odpowiadające położenim napisu
#define WN_MODE_LEFTUP 1
#define WN_MODE_CENTER 2

class komunikat {
	private:
		SDL_Surface *napis_caly;
		SDL_Rect dest;
		int x;
		char x_is_not_set;
	
	public:
		int speed;
		int odstep;
	
		// konstruktory
		komunikat(char* str, char* font_path, int font_size, SDL_Color &font_color, int y) {
			// dla zgodności z wywołaniem funkcji SDLowych kolor odbierany przez referencję
			//  czyli pobieramy adres zmiennej podanej jako argument
			//  i posługujemy się zmienną o takim adresie,
			//   odpowiada to równoczesnemu przekazywaniu do funkcji wskaźnika na zmienną (&zm),
			//   odbieraniu wskaźnika (*zm) i operowaniu na wyłuskaniu wartości (*zm = ...)
			ustaw_komunikat(str, font_path, font_size, font_color, y);
			speed = 5;
			odstep = 150;
		}
		
		// ustawianie punktu startu napisu
		void ustaw_start (int x_init) {
			x = x_init;
			x_is_not_set = 0;
		}
		
		// ustawianie treści napisu
		void ustaw_komunikat (char *str, char *font_path, int font_size, SDL_Color &font_color, int y) {
			TTF_Font *ttf = TTF_OpenFont(font_path, font_size);
			if (ttf == NULL) {
				printf("Błąd otwarcia czcionki: %s\n",TTF_GetError());
				SDL_Quit();
				return;
			}
			
			if(!(napis_caly = TTF_RenderUTF8_Solid(ttf, str, font_color))) {
				printf("Błąd renderowania napisu: %s\n",TTF_GetError());
				SDL_Quit();
				return;
			}
			TTF_CloseFont(ttf);
			
			x_is_not_set=1;
			dest.y=y;
			dest.w=napis_caly->w;
			dest.h=napis_caly->h;
		}
		
		// wyświetlanie napisu
		void move_and_put(SDL_Surface *ekran) {
			/// gdy x nie był ustawiony to ustawiamy na prawy róg ekranu
			if (x_is_not_set)
				ustaw_start(ekran->w);
			
			/// przesówamy napis w lewo zgodnie z zadaną prędkoscią
			x -= speed;
			
			/// jeżeli cały napis nam uciekł za ekran przechodzimy do następnego napisu
			if (x + napis_caly->w < 0)
				x = x + napis_caly->w + odstep;
			
			/// wyświetlamy półprzezroczysty pasek
			// SDL_SetAlpha(ekran, SDL_SRCALPHA, 125) - ustawia polprzezroczystosc calego surface
			// SDL_SetColorKey - ustawia kolor RGB który ma być uważany za przeźroczysty
			// ale SDL_FillRect z ustawionym kanelem alfa nie powoduje blendingu
			// zatem zkorzystam z funkcji z SDL_gfx:
			//boxRGBA(ekran, , 0, dest->y + napis->w, , 0, 0, 0, 125);
			boxRGBA(ekran, 0, dest.y - 5, ekran->w, dest.y + napis_caly->h + 5, 0, 0, 0, 125);
			
			/// wypisywanie komunikatu w pętli
			dest.x = x;
			int i = 1;
			do {
				SDL_BlitSurface(napis_caly, NULL, ekran, &dest);
				SDL_UpdateRects(ekran, 1, &dest);
				dest.x = x + (napis_caly->w + odstep) * i++;
			} while (dest.x < ekran->w);
		}
};

void print_image(SDL_Surface *image, SDL_Surface *ekran) {
	SDL_Rect dest;

	// ustalenie obszaru na ekranie na ktorym ma byc umieszczony obrazek
	// x,y - gorny lewy rog obrazka, ustawiamy tak aby obrazek byl na srodku ekranu
	dest.x = (ekran->w - image->w)/2;
	dest.y = (ekran->h - image->h)/2;

	// w,h - wysokość i szerokość obrazka
	dest.w = image->w;
	dest.h = image->h;

	// skopiowanie obrazka
	SDL_BlitSurface(image, NULL, ekran, &dest);

	// aktualizacja wybranego fragmentu ekranu
	SDL_UpdateRects(ekran, 1, &dest);
}

void wyswietl_napis(char *str, SDL_Surface *ekran, int x, int y, char mode,
  TTF_Font *ttf, SDL_Color &kolor_napisu, SDL_Color &kolor_tla) {
	SDL_Rect dest;
	SDL_Surface *napis;
	
	if(!(napis = TTF_RenderUTF8_Shaded(ttf, str, kolor_napisu, kolor_tla))) {
		printf("Błąd renderowania napisu: %s\n",TTF_GetError());
		SDL_Quit();
		return;
	}
	
	// rozmiary napis
	dest.w=napis->w;
	dest.h=napis->h;
	
	// półożenie napisu (uwaga napis zawsze umieszczany od lewego górnego rogu dest)
	if (mode == WN_MODE_LEFTUP) {
		dest.x=x;
		dest.y=y;
	} else if (mode == WN_MODE_CENTER) {
		dest.x=x - napis->w /2;
		dest.y=y - napis->h /2;
	}
	
	SDL_BlitSurface(napis, NULL, ekran, &dest);
	SDL_UpdateRects(ekran, 1, &dest);
	
	SDL_FreeSurface(napis);
}

double oblicz_zoom(SDL_Surface *ekran, SDL_Surface *obrazek) {
	double zoom_x = (double)ekran->w / (double)obrazek->w;
	double zoom_y = (double)ekran->h / (double)obrazek->h;
	if (zoom_y < zoom_x)
		return zoom_y;
	else
		return zoom_x;
}

int main(int argc, char **argv) {
	// inicjalizacja bliblioteki SDL
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTTHREAD)!=0) {
		printf("Błąd inicjalizacji SDL: %s\n",SDL_GetError());
		return -1;
	}

	// tworzymy unie przechowujaca informacje o zdarzeniach
	SDL_Event zdarzenie;
	
	// inicjalizacja systemu czionek
	if (TTF_Init()) {
		printf("Błąd inicjalizacji SDL_ttf: %s\n",TTF_GetError());
		SDL_Quit();
		return -1;
	}
	
	// wczytany ekran obrazka
	SDL_Surface *obrazek = SDL_LoadBMP("./Philips_PM5544.bmp");
	SDL_Surface *output_image = SDL_LoadBMP("./Philips_PM5544.bmp");
		// zamiast SDL_CreateRGBSurface z którą były problemy
	if ( obrazek == NULL || output_image == NULL ) {
		fprintf(stderr, "Nie można wczytać obrazka: %s\n", SDL_GetError());
		return -1;
	}
	
	// ładowanie czionek
	TTF_Font *ttf_name = TTF_OpenFont(
		"/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf",
		30
	);
	TTF_Font *ttf_time = TTF_OpenFont(
		"/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf",
		30
	);
	if (ttf_name == NULL || ttf_time == NULL) {
		printf("Błąd otwarcia czcionki: %s\n",TTF_GetError());
		SDL_Quit();
		return -1;
	}
	
	double zoom = 0.35;
	int argc2 = argc; // zmienna pomocnicza przechowująca liczbę nieobsłużonych argumentów
	// tworzymy ekran
	SDL_Surface *screen;
	if ( argc >= 2 && ! strcmp(argv[1], "-w") ) {
		// okienko
		screen = SDL_SetVideoMode(320,240,24,SDL_HWSURFACE|SDL_RESIZABLE);
		argc2--;
	} else {
		// pełny ekran
		screen = SDL_SetVideoMode(1024,768,24,SDL_HWSURFACE|SDL_FULLSCREEN);
	}
	
	// ustalenie skalowania
	zoom = oblicz_zoom(screen, obrazek);
	
	// kolory napisów
	SDL_Color kolor_tla={0,0,0}, kolor_napisu={0xff,0xff,0xff};
	
	// umieszczenie nazwy na obrazku
	wyswietl_napis((char*)"www.opcode.eu.org", obrazek,
		512 * obrazek->w / 1025, 623 * obrazek->h / 770,
		WN_MODE_CENTER, ttf_name, kolor_napisu, kolor_tla);
	
	// tworzymy komunikat specjalny
	komunikat * napis_specjalny = NULL;
	if (argc2 == 2)
		napis_specjalny = new komunikat(
			argv[argc - 1],
			(char*)"/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf",
			32,
			kolor_napisu,
			715 * obrazek->h / 770
		); // komunikat tworzymy w oparciu o ostatni z argumentów
	
	// położenia zegarów
	const int time1_x = 311 * obrazek->w / 1025;
	const int time2_x = 713 * obrazek->w / 1025;
	const int time_y = 393 * obrazek->h / 770;
	
	// zmienne pomocnicze
	time_t czas;
	struct tm czas_rozlozony;
	char czas_napis[100];
	int i=0;
	SDL_Surface *scaled_image;
	
	while (1) {
		// kopiujemy tło na ekran roboczy
		print_image(obrazek, output_image);
		
		// odczytujemy czas
		time(&czas);
		gmtime_r(&czas, &czas_rozlozony); // localtime_r(&czas, &czas_rozlozony);
		
		// tworzymy napis i dodajemy go do ekranu roboczego
		strftime(czas_napis, sizeof(czas_napis), "%Y-%m-%d", &czas_rozlozony);
		wyswietl_napis(czas_napis, output_image, time1_x, time_y,
			WN_MODE_CENTER, ttf_time, kolor_napisu, kolor_tla);
		strftime(czas_napis, sizeof(czas_napis), "%H:%M:%S UTC", &czas_rozlozony);
		wyswietl_napis(czas_napis, output_image, time2_x, time_y,
			WN_MODE_CENTER, ttf_time, kolor_napisu, kolor_tla);
		
		if (napis_specjalny != NULL)
			napis_specjalny->move_and_put(output_image);
		
		// skalujemy
		if (zoom != 1) {
			scaled_image = zoomSurface(output_image, zoom, zoom, 1);
		} else {
			scaled_image = output_image;
		}
		
		// przenosimy ekran roboczy na ekran docelowy
		print_image(scaled_image, screen);
		
		if (zoom != 1) {
			SDL_FreeSurface(scaled_image);
		}
		
		// obsługujemy ewentualne zdarzenia
		// warto zauważyć że korzystamy z nieblokującej funkcji SDL_PollEvent zamiast SDL_WaitEvent
		while (SDL_PollEvent(&zdarzenie)) {
			if (zdarzenie.type == SDL_QUIT || zdarzenie.type == SDL_KEYDOWN && zdarzenie.key.keysym.sym == SDLK_ESCAPE) {
				SDL_FreeSurface(output_image);
				SDL_Quit();
				exit (0);
			}
		}
		// czekamy 0.1s
		SDL_Delay(100);
	}
}

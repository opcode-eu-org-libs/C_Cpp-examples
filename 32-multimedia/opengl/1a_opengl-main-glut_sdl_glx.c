/**

plik odpowiedzialny za obsługę "bibliotek wysokopoziomowych" OpenGL - glut, sdl, glx

pokazuje różnice w inicjalizacji poszczególnych bibliotek, a także możliwość ich mieszania
(np. SDL wraz z swoimi udogodnieniami jak timery itp + GLUT z swoimi funkcji do obsługi napisów itp)

OpenGL jest biblioteką (API) do obsługi grafiki 3D, potrafi on wykorzystywać wsparcie sprzętowe
GPU zgodnych z tą specyfikacją (oprócz tego jest implementacja programowa - MESA).
Biblioteka stworzona jest w architekturze klient-serwer. Klientem jest program zlecający operacje 3D,
a serwerem implementacja OpenGL (programowa lub korzystająca z wsparcia sprzętowego - w sterowniku
karty graficznej). W szczególności biblioteka pozwala na użycie zdalnego serwera.

Pierwszą rzeczą, która rzuca się w oczy przy korzystaniu z OpenGL jest to iż jest on maszyną stanu.
Korzystanie z tej biblioteki polega w dużej mierze na wywoływaniu funkcji odpowiedzialnych
za zmiany stanu w jakim pracuje serwer OpenGL (np. funkcje glEnable, funkcje ustawiające
parametry renderingu itp).

Część funkcjonalności OpenGL jest implementowana w postaci opcjonalnych rozszerzeń
(wiele z nich wchodzi potem do kolejnego wydania podstawowej specyfikacji),
wsparcie w rozpoznawaniu rozszerzeń obsługiwanych przez aktualną implementację
zapewnia biblioteka GLEW.

Oprócz samego OpenGL funkcjonują biblioteki ułatwiające korzystanie z niego (tworzenie okna itp)
i dodające m.in. funkcjonalności obsługi wejścia takie jak porównywane w tym pliku GLUT, SDL, GLX.

Ogólnie zauważalna jest tendencja do coraz swobodniejszego programowania GPU - zauważalne
jest to chociażby w zwiększaniu roli programowalnego potoku (shadery itp), a minimalizacji
znaczenia potoku stałego (patrz np. OpenGL ES 2.0). Rozwój GPU i zwiększanie ilości pamięci
na kartach zaczyna pociągać za sobą zagadnienie zarządzania tą pamięcią przez sterownik
karty / system operacyjny (pewne echa tego będą wspomniane w jednym z przykładowych programów)
Przy omawianiu tego typu zagadnień warto wspomnieć o OpenCL - języku stworzonym z myślą o
możliwości swobodnego przerzucenia obliczeń pomiędzy CPU a GPU (obecnie moce obliczeniowe
GPU w niektórych zagadnieniach znacznie przekraczają moce obliczeniowe CPU - patrz GPGPU
czyli General Purpose Graphics Processing Unit).

UWAGA:
Są to dość stare przykłady korzystające z Fixed Function Pipeline nie obsługiwanym w współczesnych wersjach OpenGL i wspierającym je sprzęcie.
Współcześnie także używanie surowego OpenGL jest rzadkością – dużo częściej stosowane są wyżej poziomowe biblioteki związane z obsługą 3D (np. Ogre 3D).
W związku z tym przykłady z tego katalogu należy traktować czysto dydaktycznie (celem zrozumienia jak to może działać, jak to może być robione) niż jako punkt wyjścia do tworzenia czegoś.

**/

/// numeryczne identyfikatory trybów kompilacji
#define GLUT 1
// gcc -lGL -lGLU -lglut          -lm -D MODE=1 1a_opengl-main-glut_sdl_glx.c
#define SDL 2
// gcc -lGL -lGLU -lSDL           -lm -D MODE=2 1a_opengl-main-glut_sdl_glx.c
#define GLX 3
// gcc -lGL -lGLU -lXxf86vm -LX11 -lm -D MODE=3 1a_opengl-main-glut_sdl_glx.c

/// określamy plik z funkcją używaną do rysowania
#ifndef DRAW_FILE
# define DRAW_FILE "3a_opengl-draw-simple.c"
#endif

/// włączamy plik odpowiedzialny za ustawienia "czystego" OpenGL
#include "2_opengl-init.c"

/// standardowe pliki nagłówkowe
#include <stdio.h>
#include <stdlib.h> // exit
#include <unistd.h> // sleep, usleep


/// funkcja główna (w trzech wariantach) - przygotowanie do funkcji obsługi OpenGL
/// oraz funkcje pomocnicze dla niej
/// i nagłówki odpowiednich bibliotek "wysokopoziomowych" OpenGL 
#if MODE == GLUT
	/// GLUT (-lglut)
	#include <GL/glut.h>
	
	// określenie funkcji rysującej na ekran (zależne od MODE)
	#define DRAW_TO_SCREEN glutSwapBuffers()
	// makro określające że wykonana była inicjalizacja glut'a
	// i wolno korzystać z jego funkcji
	#define GLUT_INIT 1
	// włączenie pliku obsługującego rysowanie "czystym" OpenGL
	#include DRAW_FILE
	
	// globalna zmienna z identyfikatorem okna GLUT'owego
	int okno;
	
	// funkcja obsługująca reakcję na zdarzenia klawiatury
	void nacisnieto_klawisz(unsigned char klawisz, int x, int y) {
		// naciśnięcie q lub ESCAPE (kod ASCII)
		// klawisze specjalne zdefiniowane w GL/glut.h (np. GLUT_KEY_DOWN)
		switch (klawisz) {
			case 'q':
			case 0x1b:
				// niszczymy okno GLUT'owe
				glutDestroyWindow(okno);
				// kończymy program
				exit(0);
			case 'F':
				// przejście w tryb pełnoekranowy
				glutFullScreen();
				// w zasadzie przedtem powinniśmy zapamiętać:
				// glutGet(GLUT_WINDOW_WIDTH) glutGet(GLUT_WINDOW_HEIGHT)
				// oraz położenie okna aby muc wrócić do trybu okienkowego przy pomocy:
				// glutReshapeWindow() i glutPositionWindow()
				break;
		}
	}
	
	// funkcja główna
	int main (int argc, char **argv) {
		// inicjalizacja GLUTa i przekazanie do niego argumentów wywołania
		glutInit(&argc, argv);
		// ustawienie trybu wyświetlania
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
		// ustawienie początkowego rozmiaru okna GLUTowego
		glutInitWindowSize(500, 300);
		// ustawienie położenia okna GLUTowego (lewy górny róg)
		glutInitWindowPosition(0, 0);
		// utworzenie okna
		okno = glutCreateWindow("tytul okna GLUT");
		
		// przejście w tryb pełnoekranowy
		//glutFullScreen();
		
		// określenie funkcji odpowiedzialnej za rysowanie sceny
		glutDisplayFunc(&narysuj_scene);
		// określenie funkcji wykonywanej gdy nie ma zdarzeń do obsłużenia
		glutIdleFunc(&narysuj_scene);
		// okręslenie funkcji wykonywane przy zmianie rozmiaru okna
		glutReshapeFunc(&ustaw_rozmiar_sceny);
		
		// określenie funkcji wywoływanej po naciśnięciu przycisku na klawiaturze
		glutKeyboardFunc(&nacisnieto_klawisz);
		
		// inicjalizacja OpenGL
		inicjalizuj_GL();
		
		// przygotowanie obiektów dla funkcji rysującej
		przygotuj_scene();
		
		// uruchomienie pętli przetwarzającej zdarzenia GLUT 
		glutMainLoop();
	}
#elif MODE == SDL
	/// SDL (-lSDL)
	#include <SDL/SDL.h>
	#ifdef GLUT_INIT
		#include <GL/glut.h> // GLUT (-lglut)
	#endif
	// określenie funkcji rysującej na ekran (zależne od MODE)
	#define DRAW_TO_SCREEN SDL_GL_SwapBuffers()
	// włączenie pliku obsługującego "czysty" OpenGL
	#include DRAW_FILE
	
	// zmienna globalna będąca wskaźnikiem na powierzchnię rysunkową
	SDL_Surface *powierzchnia;
	
	int main (int argc, char **argv) {
		// pomimo używania innej biblioteki możemy korzystać z
		// pewnych funkcji GLUTa (np. umieszczających napisy czy też obiekty geometryczne)
		// jeżeli dokonamy jego inicjalizacji
		#ifdef GLUT_INIT
			glutInit(&argc, argv);
		#endif
		
		// inicjalizacja bliblioteki SDL
		if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTTHREAD)!=0) {
			printf("Błąd inicjalizacji SDL: %s/n",SDL_GetError());
			SDL_Quit(); // likwidacja okna SDL
			return -1;
		}
		
		// pobranie informacji o urządzeniu wideo 
		const SDL_VideoInfo *video_info;
		if ( (video_info = SDL_GetVideoInfo()) == NULL ) {
			printf("Błąd odczytu informacji o video: %s/n",SDL_GetError());
			SDL_Quit(); // likwidacja okna SDL
			return -1;
		}
		
		// ustawienie flag dla trybu wideo w zależności od posiadanego sprzętu
		int flagi_trybu_wideo = 0;
		if ( video_info->hw_available ) {
			flagi_trybu_wideo = SDL_HWSURFACE;
			puts("SDL_HWSURFACE - \"powierzchnia\" w pamięci karty graficznej");
		} else {
			flagi_trybu_wideo = SDL_SWSURFACE;
			puts("SDL_SWSURFACE - \"powierzchnia\" w pamięci programowej");
		}
		if ( video_info->blit_hw ) {
			flagi_trybu_wideo |= SDL_HWACCEL;
			puts("SDL_HWACCEL - akceleracja sprzętowa");
		} else {
			puts("NO SDL_HWACCEL - brak akceleracji sprzętowej");
		}
		// rozszerzenie wcześniej ustawionych właściwości trybu widoo o:
		// OpenGL + podwójne buferowanie + sprżetowa paleta + okno z możliwością zamiany rozmiaru
		flagi_trybu_wideo |= SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;
		
		// włączamy podwójne buferowanie OpenGL
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		
		// uzyskanie "powierzchni" o zadanym rozmiarze (500x300), głębi kolorów (8 bitów)
		// i właściwościach (ustawione wcześniej flagi)
		if ( (powierzchnia = SDL_SetVideoMode( 500, 300, 8, flagi_trybu_wideo)) == NULL ) {
			printf("Błąd pobrania \"powierzchni\" SDL: %s/n",SDL_GetError());
			SDL_Quit(); // likwidacja okna SDL
			return -1;
		}
		
		// inicjalizacja OpenGL
		inicjalizuj_GL();
		// ustawienie rozmiaru sceny
		ustaw_rozmiar_sceny( 500, 300 );
		
		// przygotowanie obiektów dla funkcji rysującej
		przygotuj_scene();
		
		// pętla zdarzeń
		SDL_Event zdarzenie;
		char okno_jest_aktywne;
		while (1) {
			while ( SDL_PollEvent( &zdarzenie ) ) {
				switch (zdarzenie.type) {
					case SDL_KEYDOWN:
						// naciśnięty przycisk
						switch ( zdarzenie.key.keysym.sym ) {
							case SDLK_F2:
								// przełączenie w tryb pełnioekranowy
								SDL_WM_ToggleFullScreen( powierzchnia );
								break;
							case SDLK_ESCAPE:
								// likwidacja okna SDL
								SDL_Quit();
								// zakończenie programu
								exit(0);
						}
						break;
					case SDL_VIDEORESIZE:
						// zmienił się rozmiar okna
						if ( (powierzchnia = SDL_SetVideoMode(
							zdarzenie.resize.w, zdarzenie.resize.h, 8, flagi_trybu_wideo)) == NULL ) {
							printf("Błąd pobrania \"powierzchni\" SDL po zmianie rozmiaru okna: %s/n",
								SDL_GetError());
							SDL_Quit(); // likwidacja okna SDL
							return -1;
						}
						ustaw_rozmiar_sceny( zdarzenie.resize.w, zdarzenie.resize.h );
						break;
					case SDL_ACTIVEEVENT:
						// okno zostało aktywowane lub deaktywowane
						okno_jest_aktywne = zdarzenie.active.gain;
						// zapamiętujemy czy okno aktywne
						break;
					case SDL_QUIT:
						SDL_Quit();
						exit (0);
				}
			}
			// rysowanie tylko gdy okno aktywne
			if ( okno_jest_aktywne )
				narysuj_scene();
		}
	}
#elif MODE == GLX
	/// GLX (--)
	#include <GL/glx.h>
	#ifdef GLUT_INIT
		#include <GL/glut.h> // GLUT (-lglut)
	#endif
	// X11 (-lXxf86vm)
	#include <X11/extensions/xf86vmode.h>
	#include <X11/keysym.h>
	
	// UWAGA: do działania wymagany pakiet libgl1-mesa-glx a nie libgl1-mesa-swx11
	
	// struktura przechowująca informacje o oknie
	struct {
		Display *wyswietlacz;
		int ekran;
		Window okno;
		
		GLXContext kontext_gl;
		XSetWindowAttributes attrybuty;
		XF86VidModeModeInfo tryb_x_serwera;
		
		char pelny_ekran;
		unsigned int okno_szer, scena_szer;
		unsigned int okno_wys, scena_wys;
	} oknoGLX;

	// określenie funkcji rysującej na ekran (zależne od MODE)
	#define DRAW_TO_SCREEN glXSwapBuffers(oknoGLX.wyswietlacz, oknoGLX.okno)
	// włączenie pliku obsługującego "czysty" OpenGL
	#include DRAW_FILE

	void stworz_okno_glx() {
		// połączenie z Xserwerem
		oknoGLX.wyswietlacz = XOpenDisplay(0);
		oknoGLX.ekran = DefaultScreen(oknoGLX.wyswietlacz);
		
		// pobieramy informację o trybach ekranu
		XF86VidModeModeInfo **tryby;
		int ile_trybow;
		XF86VidModeGetAllModeLines(oknoGLX.wyswietlacz, oknoGLX.ekran, &ile_trybow, &tryby);
		// zachowujemy informację o obecnie używanym trybie ekranu
		oknoGLX.tryb_x_serwera = *tryby[0];
		
		// uzyskiwania visuala o zadanych atrybutach
		XVisualInfo *visual;
		int attrList[] = {GLX_RGBA, GLX_RED_SIZE, 4, GLX_GREEN_SIZE, 4, GLX_BLUE_SIZE, 4,
		GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};
		visual = glXChooseVisual(oknoGLX.wyswietlacz, oknoGLX.ekran, attrList);
		if (visual == NULL) {
			// gdy się nie udało próbujemy bez podwójnego buforowania
			attrList[9] = None;
			visual = glXChooseVisual(oknoGLX.wyswietlacz, oknoGLX.ekran, attrList);
		}
		
		// tworzenie kontekstu GLX
		oknoGLX.kontext_gl = glXCreateContext(oknoGLX.wyswietlacz, visual, 0, GL_TRUE);
		
		// tworzenie mapy kolorów
		oknoGLX.attrybuty.colormap = XCreateColormap(oknoGLX.wyswietlacz,
			RootWindow(oknoGLX.wyswietlacz, visual->screen), visual->visual, AllocNone);
		oknoGLX.attrybuty.border_pixel = 1;
		oknoGLX.attrybuty.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
			StructureNotifyMask;
		
		// tworzenie okna w trybie pełnoekranowym
		if (oknoGLX.pelny_ekran) {
			// wybieramy tryb ekranu zgodny z naszymi preferencjami
			int i;
			int preferowany_tryb = 0;
			for (i = 0; i < ile_trybow; i++) {
				if ((tryby[i]->hdisplay == oknoGLX.okno_szer)
				&& (tryby[i]->vdisplay == oknoGLX.okno_wys)) {
					preferowany_tryb = i;
					break;
				}
			}
			XF86VidModeSwitchToMode(oknoGLX.wyswietlacz, oknoGLX.ekran, tryby[preferowany_tryb]);
			XF86VidModeSetViewPort(oknoGLX.wyswietlacz, oknoGLX.ekran, 0, 0);
			oknoGLX.attrybuty.override_redirect = True;

			oknoGLX.okno = XCreateWindow(oknoGLX.wyswietlacz,
				RootWindow(oknoGLX.wyswietlacz, visual->screen),
				0, 0, tryby[preferowany_tryb]->hdisplay, tryby[preferowany_tryb]->vdisplay, 0,
				visual->depth, InputOutput, visual->visual,
				CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
				&oknoGLX.attrybuty);
			
			XWarpPointer(oknoGLX.wyswietlacz, None, oknoGLX.okno, 0, 0, 0, 0, 0, 0);
			XMapRaised(oknoGLX.wyswietlacz, oknoGLX.okno);
			XGrabKeyboard(oknoGLX.wyswietlacz, oknoGLX.okno, True, GrabModeAsync,
				GrabModeAsync, CurrentTime);
			XGrabPointer(oknoGLX.wyswietlacz, oknoGLX.okno, True, ButtonPressMask,
				GrabModeAsync, GrabModeAsync, oknoGLX.okno, None, CurrentTime);
			
			// dla poprawnego skalowania sceny
			oknoGLX.scena_szer = tryby[preferowany_tryb]->hdisplay;
			oknoGLX.scena_wys = tryby[preferowany_tryb]->vdisplay;
		// tworzenie normalnego okna
		} else {
			oknoGLX.okno = XCreateWindow(oknoGLX.wyswietlacz,
				RootWindow(oknoGLX.wyswietlacz, visual->screen),
				0, 0, oknoGLX.okno_szer, oknoGLX.okno_wys, 0, visual->depth,
				InputOutput, visual->visual,CWBorderPixel | CWColormap | CWEventMask,
				&oknoGLX.attrybuty);
				
			Atom wm_delete = XInternAtom(oknoGLX.wyswietlacz, "WM_DELETE_WINDOW", True);
			XSetWMProtocols(oknoGLX.wyswietlacz, oknoGLX.okno, &wm_delete, 1);
			XSetStandardProperties(oknoGLX.wyswietlacz, oknoGLX.okno, "tytul okna GLX",
				"tytul okna GLX", None, NULL, 0, NULL);
			XMapRaised(oknoGLX.wyswietlacz, oknoGLX.okno);
			
			// dla poprawnego skalowania sceny
			oknoGLX.scena_szer = oknoGLX.okno_szer;
			oknoGLX.scena_wys = oknoGLX.okno_wys;
		}
		XFree(visual);
		XFree(tryby);
		
		// łączenie kontekstu z oknem
		glXMakeCurrent(oknoGLX.wyswietlacz, oknoGLX.okno, oknoGLX.kontext_gl);
		// XGetGeometry możemy pobrać aktualną geometrię okna
		// poprzez glXIsDirect możemy sprawdzić czy używamy DRI
		
		// inicjalizacja OpenGL
		inicjalizuj_GL();
		// ustawienie rozmiaru sceny
		ustaw_rozmiar_sceny(oknoGLX.scena_szer, oknoGLX.scena_wys);
	}

	void zniszcz_okno_glx() {
		if (oknoGLX.kontext_gl) {
			// jeżeli był to zwalniamy kontekst
			glXMakeCurrent(oknoGLX.wyswietlacz, None, NULL);
			glXDestroyContext(oknoGLX.wyswietlacz, oknoGLX.kontext_gl);
			oknoGLX.kontext_gl = NULL;
		}
		if (oknoGLX.pelny_ekran) {
			// jeżeli był tryb pełnoekranowy to przywracamy orginalne ustawienia Xserwera
			XF86VidModeSwitchToMode(oknoGLX.wyswietlacz, oknoGLX.ekran, &oknoGLX.tryb_x_serwera);
			XF86VidModeSetViewPort(oknoGLX.wyswietlacz, oknoGLX.ekran, 0, 0);
		}
		// zamykamy połączenie z wyświetlaczem
		XCloseDisplay(oknoGLX.wyswietlacz);
	}
	
	int main (int argc, char **argv) {
		// pomimo używania innej biblioteki możemy korzystać z
		// pewnych funkcji GLUTa (np. umieszczających napisy czy też obiekty geometryczne)
		// jeżeli dokonamy jego inicjalizacji
		#ifdef GLUT_INIT
			glutInit(&argc, argv);
		#endif
		
		// wstępne ustawienia okna
		oknoGLX.pelny_ekran=0;
		oknoGLX.okno_szer=500;
		oknoGLX.okno_wys=300;
		
		// tworzymy okno
		stworz_okno_glx();
		
		// przygotowanie obiektów dla funkcji rysującej
		przygotuj_scene();
		
		// pętla zdarzeń
		XEvent zdarzenie;
		char okno_jest_aktywne;
		KeySym klawisz;
		while (1) {
			while (XPending(oknoGLX.wyswietlacz) > 0) {
				// jeżeli są oczekujące zdarzenia odbieramy je
				XNextEvent(oknoGLX.wyswietlacz, &zdarzenie);
				switch (zdarzenie.type) {
					// okno zostało pokazane
					case Expose:
						if (zdarzenie.xexpose.count == 0)
							narysuj_scene();
						break;
					// zmiana w konfiguracji okna
					case ConfigureNotify:
						// jeżeli rozmiar uległ rzeczywistej zmianie
						if ( (zdarzenie.xconfigure.width != oknoGLX.okno_szer) ||
						  (zdarzenie.xconfigure.height != oknoGLX.okno_wys) ) {
							oknoGLX.scena_szer = oknoGLX.okno_szer = zdarzenie.xconfigure.width;
							oknoGLX.scena_wys = oknoGLX.okno_wys = zdarzenie.xconfigure.height;
							ustaw_rozmiar_sceny(oknoGLX.okno_szer, oknoGLX.okno_wys);
						}
						break;
					// naciśnięcie przycisku na klawiaturze
					case KeyPress: // ButtonPress -> zdarzenie myszy
						klawisz = XLookupKeysym(&zdarzenie.xkey, 0);
						switch (klawisz) {
							case XK_Escape:
								puts("close via XK_Escape");
								zniszcz_okno_glx();
								exit(0);
								break;
							case XK_F2:
								puts("switch fullscreen");
								zniszcz_okno_glx();
								oknoGLX.pelny_ekran = !oknoGLX.pelny_ekran;
								stworz_okno_glx();
								break;
						}
						break;
					// wiadomość do klienta
					case ClientMessage:
						// obsługa zamknięcia okna poprzez komunikat X'ów
						// (np. naciśnięcie krzyżyka zamykającego na belce tytułowej)
						if (*XGetAtomName(oknoGLX.wyswietlacz, zdarzenie.xclient.message_type) == *"WM_PROTOCOLS") {
							puts("close via WM_PROTOCOLS");
							zniszcz_okno_glx();
							exit(0);
						}
						break;
				}
			}
			narysuj_scene();
		}
	}
#endif

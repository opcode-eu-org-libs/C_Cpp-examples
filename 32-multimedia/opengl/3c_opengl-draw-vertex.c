/**

plik odpowiedzialny za obsługę "czystego" OpenGL - rysowanie
prezentuje techniki poświęcone buforom vertexów i texturowaniu

WARTO JESZCZE ZAPOZNAĆ SIĘ Z:
 * obiekty buforowe vertexów i pikseli czyli korzystamy z RAMu karty graficznej

UWAGA:
Są to dość stare przykłady korzystające z Fixed Function Pipeline nie obsługiwanym w współczesnych wersjach OpenGL i wspierającym je sprzęcie.
Współcześnie także używanie surowego OpenGL jest rzadkością – dużo częściej stosowane są wyżej poziomowe biblioteki związane z obsługą 3D (np. Ogre 3D).
W związku z tym przykłady z tego katalogu należy traktować czysto dydaktycznie (celem zrozumienia jak to może działać, jak to może być robione) niż jako punkt wyjścia do tworzenia czegoś.

**/

/// nagłówek biblioteki libglpng odpowiedzialnej za
/// obsługę plików png dla OpenGL
#include <GL/glpng.h>

/// zmienna globalna przechowująca licznik klatek
int licznik_klatek = 0;

/// zmienne globalne trzymające informacje o teksturach i jej plikach
GLuint tekstura1, tekstura2;
pngInfo tekstura_info;


// typ struktury opisującej typ "podzbioru" informacji o vertexie
// ma wyłącznie charakter pomocniczy do zapisu deklaracji vdesc
typedef struct {
	GLint size;      // liczba składowych "pozdbioru"
	GLenum type;     // typ danych składowych "podzbioru"
	GLsizei offset;  // przesunięcie w bajtach od początku werteksu
} vertex_subset_desc;

/// struktura opisująca typ naszego vertexa
// swoją drogą warto też zauważyć sposób deklaracji struktury "const"
const struct {
	const vertex_subset_desc punkt;
	const vertex_subset_desc tekstura1;
	const vertex_subset_desc tekstura2;
} vdesc = {
	{3, GL_FLOAT, 0},
	{2, GL_FLOAT, 3*sizeof(GLfloat)},
	{2, GL_FLOAT, (3+2)*sizeof(GLfloat)}
};

/// typ struktury reprezentującej punkt naszego vertexu
typedef struct {
	GLfloat punkt[3];
	GLfloat tekstura1[2];
	GLfloat tekstura2[2];
} vertex_point;

vertex_point kwadrat [4] = {
	{{ 0.0f,  4.0f, 0.0f},   {0.0f, 0.0f},   {0.0f, 0.0f}},
	{{ 4.0f,  4.0f, 0.0f},   {1.0f, 0.0f},   {2.0f, 0.0f}},
	{{ 4.0f,  0.0f, 0.0f},   {1.0f, 1.0f},   {2.0f, 2.0f}},
	{{ 0.0f,  0.0f, 0.0f},   {0.0f, 1.0f},   {0.0f, 2.0f}},
};

// trywialna tablica indeksów - tylko po to aby pokazać jak działa
GLbyte kwadrat_indeksy [4] = {0, 1, 2, 3};

// w zasadzie możnaby (a chyba nawet warto) reprezentować taki obiekt jako klasę która oprócz
// tablicy elementów vertex_point, struktury vdesc i tablicy indeksów
// zawierałaby także informację o typie tekstury (np. GL_TEXTURE_2D), o sposobie rysowania
// vertexu (np. GL_QUADS), opcje teksturowania (powielanie, ramka itp)


/// funkcja przygotowująca scenę
void przygotuj_scene(void) {
	/// wczytanie pliku tekstury
	tekstura1 = pngBind("tekstura1.png", PNG_NOMIPMAP, PNG_ALPHA, &tekstura_info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	tekstura2 = pngBind("tekstura2.png", PNG_NOMIPMAP, PNG_ALPHA, &tekstura_info, GL_CLAMP, GL_NEAREST, GL_NEAREST);
	
	/// załaduj i aktywuj tablice położeń wierzchołków
	glVertexPointer(
		vdesc.punkt.size,   // ilość (kolejnych) elementu tablicy położeń
		vdesc.punkt.type,   // typ elementu tablicy położeń
		sizeof(vertex_point), // okresowość występowania początku tab. poł.
		                    // w tablicy opisującej cały vertex
		(void*)kwadrat +  vdesc.punkt.offset // adres pierwszego elementu tab. poł.
	);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_TEXTURE_2D);
	
	/// załaduj i aktywuj tablice współrzędnych tekstury
	// dla pierwszej jednostki teksturującej
	glActiveTexture(GL_TEXTURE0); // przełączamy aktywną jednostkę teksturującą
	glClientActiveTexture(GL_TEXTURE0); // i po stronie klienta
	glTexCoordPointer(
		vdesc.tekstura1.size,   // ilość (kolejnych) elementu tablicy wsp. teks.
		vdesc.tekstura1.type,   // typ elementu tablicy wsp. teks.
		sizeof(vertex_point),     // okresowość występowania początku tab. wsp. teks.
		                        // w tablicy opisującej cały vertex
		(void*)kwadrat +  vdesc.tekstura1.offset
		                       // adres pierwszego elementu tab. wsp. teks.
	);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY); // aktywujemy tablicę współrzędnych
	glEnable(GL_TEXTURE_2D); // właczamy teksturowanie 2d
	glBindTexture(GL_TEXTURE_2D, tekstura1); // ustawiamy teksturę
	
	// dla drugiej jednostki teksturującej
	glActiveTexture(GL_TEXTURE1); // przełączamy aktywną jednostkę teksturującą
	glClientActiveTexture(GL_TEXTURE1); // i po stronie klienta
	glTexCoordPointer(
		vdesc.tekstura2.size,   // ilość (kolejnych) elementu tablicy wsp. teks.
		vdesc.tekstura2.type,   // typ elementu tablicy wsp. teks.
		sizeof(vertex_point),     // okresowość występowania początku tab. wsp. teks.
		                        // w tablicy opisującej cały vertex
		(void*)kwadrat +  vdesc.tekstura2.offset
		                       // adres pierwszego elementu tab. wsp. teks.
	);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY); // aktywujemy tablicę współrzędnych
	glEnable(GL_TEXTURE_2D); // właczamy teksturowanie 2d
	glBindTexture(GL_TEXTURE_2D, tekstura2); // ustawiamy teksturę
	
	// wykorzystanie kilku jednostek teksturujących umożliwia korzystanie z prawdziwego wieloteksturowania
	// (zamiast renderowania kilkakrotnie tej samej sceny z przeźroczystością i zmienianą teksturą)
	// gdybyśmy nie korzystali z vertexów definiowanych tablicami wpółrzędne dla innych niż pierwsza
	// jednostek teksturujących wpółrzędne tekstury ustawiane byłyby funkcjami z grupy glMultiTexCoord()
	
	// tak jak pokazano w opengl-main-glut.c możemy wyłączyć używanie danej jednostki teksturującej
	// (np. dla danego obiektu) np. poprzez deaktywację przypisanego do niej typu tekstury:
	// glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	
	// można także ładować całą grupę przy pomocy glInterleavedArrays
	// jednak jest to rozwiązanie znacznie mniej elastyczne gdyż wymaga wyboru
	// jednego z narzuconych formatów wertexa
};

/// funkcja rysująca (przerysowująca) scenę
void narysuj_scene() {
	// jeżeli nastąpiła zmiana ustawień kamery to ją wdrażamy
	if (kamera.is_new) ustaw_kamere();
	
	// czyścimy ekran (ustawionym wcześniej) kolorem oraz czyścimy bufor głębokości
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// ustawiamy bierzącą macierz (GL_MODELVIEW) na identyczność
	glLoadIdentity();
	
	// mnożymy bierzącą macierz przez macierz przesunięcia (x, y, z)
	glTranslatef(-1.5f,0.0f,-6.0f);
	
	// wyświetlamy 4 kolejne elementy tablicy vertexów począwszy od elementu 0
	glDrawArrays(GL_QUADS, 0, 4);
	// jest również funkcja umożliwiająca rysowanie kilku obiektów zapisanych
	// w jednej tablicy - glMultiDrawArrays przyjmuje ona tablice określające
	// położenia pierwszego vertexu kolejnych obiektów i jego długości
	
	glTranslatef(-1.0f,0.0f,1.0f);
	glScalef(0.3f, 0.3f, 0.3f);
	
	// możemy też oczywiście korzystać z indeksowanych tablic werteksów
	// wprowadzenie tablicy indeksów służy redukcji rozmiaru tablic
	// werteksów poprzez wyeliminowanie powtarzających się wierzchołków
	
	// rysujemy trójkąt w oparciu o 3 ostatnie punkty kwadratu
	glDrawElements (GL_TRIANGLE_FAN, 3, GL_UNSIGNED_BYTE, kwadrat_indeksy + 1);
	// warto też zwrócić uwagę na glMultiDrawElements() i glDrawRangeElements()
	
	// przy korzystaniu z tablic verteksów i związanych z nimi ustawień warto
	// wspomnieć o funkcjach glPushClientAttrib()/glPopClientAttrib()
	// umożliwiających odkładanie na stos (i przywracanie) tychże ustawień
	// podobnie jak glPushMatrix() czyni z macierzami świata / widoku
	
	// licznik FPS
	licznik_klatek++;
	if (licznik_klatek == 1000) {
		licznik_klatek = 0;
#if MODE == SDL
		static czas_ostatniego_wypisania = 0;
		GLint czas1 = SDL_GetTicks();
		GLint czas2 = czas1 - czas_ostatniego_wypisania;
		czas_ostatniego_wypisania = czas1;
		printf("FPS %g\n", 1000000.0 / czas2); // 1000 ms/s * 1000 frame / x s
#endif
	}
	
	// wywołujemy (zależną od wyżej poziomowego mechanizmu) funkcję rysującą scenę do okna
#ifdef DRAW_TO_SCREEN
	DRAW_TO_SCREEN;
#elif MODE == GLUT
	glutSwapBuffers();
#elif MODE == SDL
	SDL_GL_SwapBuffers();
#elif MODE == GLX
	glXSwapBuffers(oknoGLX.wyswietlacz, oknoGLX.okno);
#else
#	error "Couldn't determine DRAW_TO_SCREEN function"
#endif
}

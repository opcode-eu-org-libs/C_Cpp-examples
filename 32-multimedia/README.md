# Multimedia

1. dźwięk:
	1. [alsa_copy.c](alsa_copy.c) – program kopiujący dźwięk z wejścia na wyjście karty dźwiękowej, korzystający wyłącznie z biblioteki asound (ALSA) a zatem umożliwiający korzystanie z dmix
	2. [openal.c](openal.c) – program prezentuje podstawy korzystania z biblioteki OpenAL służącej do implementacji dźwięku (przestrzennego) w multimediach
2. biblioteka SDL – biblioteka multimedialna, obsługująca grafikę 2D i 3D, dźwięk, urządzenia wejścia/wyjścia.
	W zastosowaniach graficznych wykorzystuje OpenGL (niskopoziomowy interfejs graficzny).
	Można powiedzieć że SDL stanowi swego rodzaju wielo-platformowy odpowiednik dla DirectX (OpenGL jest rozwiązaniem konkurencyjnym dla Direct3D).
	1. [pokaz_slajdow_sdl.c](pokaz_slajdow_sdl.c) – program demonstrujący korzystanie z SDL (wyświetlanie bitmap, obsługa zdarzeń - klawiatura i tworzenie własnych, timery, czyszczenie ekranu)
	2. [rysownik_sdl.c](rysownik_sdl.c) – program demonstrujący korzystanie z SDL (rysowanie pikseli, obsługa zdarzeń - klawiatura i mysz, zapis obrazu do bitmapy)
	3. [obraz_kontrolny_sdl.cpp](obraz_kontrolny_sdl.cpp) – program generujący z wykorzystaniem SDL obraz kontrolny (umieszcza stosowne napisy na podanym pliku graficznym)
3. [read_png.c](read_png.c) – program demonstrujący korzystanie z libPNG do wczytywania obrazków
4. [OpenGL](opengl/README.md) – trochę przestarzałe, ale nadal mające jakąś wartość wprowadzenie w postaci kilku przykładów do OpenGL.
   Prezentuje użycie zarówno z SDL, jak i własnymi wyżej poziomowe biblioteki OpenGL (GLU, GLUT, GLUI, ...).

# OpenGL

Katalog zawiera zestaw przykładowych programów obrazujących korzystanie z OpenGL:

* kod odpowiedzialny za inicjalizację okana OpenGL:
	* pokazujący różnice w korzystaniu z glut, sdl i glx [1a_opengl-main-glut_sdl_glx.c](1a_opengl-main-glut_sdl_glx.c) lub
	* dający więcej funkcjonalności ale tylko z wykorzystaniem glut [1b_opengl-main-glut.c](1b_opengl-main-glut.c)
* kod odpowiedzialny za inicjalizację OpenGL: [2_opengl-init.c](2_opengl-init.c)
* (opcjonalny) kod odpowiedzialny za ładowanie shaderów z wykorzystaniem mmap [2_opengl-load_shader.c](2_opengl-load_shader.c)
* kod odpowiedzialny za rysowanie sceny OpenGL:
	* [3a_opengl-draw-simple.c](3a_opengl-draw-simple.c) (podstawy) lub
	* [3b_opengl-draw-light.c](3b_opengl-draw-light.c) (obsługa świateł) lub
	* [3c_opengl-draw-vertex.c](3c_opengl-draw-vertex.c) (obsługa bardziej zaawansowanych metod rysowanie)

Wymagane pakiety: mesa-common-dev libglu1-mesa-dev libarb-dev freeglut3-dev libsdl1.2-dev oraz [libglpng](http://www.fifi.org/doc/libglpng-dev/glpng.html)

**Uwaga:**
Są to dość stare przykłady korzystające z [Fixed Function Pipeline](https://en.wikipedia.org/wiki/Fixed-function) nie obsługiwanym w współczesnych wersjach OpenGL i wspierającym je sprzęcie.
Współcześnie także używanie surowego OpenGL jest rzadkością – dużo częściej stosowane są wyżej poziomowe biblioteki związane z obsługą 3D (np. [Ogre 3D](https://www.ogre3d.org/)).
W związku z tym przykłady z tego katalogu należy traktować czysto dydaktycznie (celem zrozumienia jak to może działać, jak to może być robione) niż jako punkt wyjścia do tworzenia czegoś.

## kompilacja

Tylko dwa z powyżej wymienionych plików dostarczają funkcję main. Są to:

* [1a_opengl-main-glut_sdl_glx.c](1a_opengl-main-glut_sdl_glx.c), który może być budowany w kilku trybach związanych z używaną biblioteką wspierającą:
	* GLUT: `gcc -lGL -lGLU -lglut -lm -D MODE=1 1a_opengl-main-glut_sdl_glx.c`
	* SDL:  `gcc -lGL -lGLU -lSDL -lm -D MODE=2 1a_opengl-main-glut_sdl_glx.c`
	* GLX:  `gcc -lGL -lGLU -lXxf86vm -LX11 -lm -D MODE=3 1a_opengl-main-glut_sdl_glx.c`
* [1b_opengl-main-glut.c](1b_opengl-main-glut.c), który zawsze budowany jest w trybie GLUT:
	`gcc -lGL -lGLU -lglut -lm 1b_opengl-main-glut.c`

Każdy z tych programów trybów może on używać dowolnego z 3 plików odpowiedzialnych za rysowanie.
Domyślnie jest to *3a_opengl-draw-simple.c* dla *1a_opengl-main-glut_sdl_glx.c* i *3b_opengl-draw-light.c* dla *1b_opengl-main-glut.c*.

Inne mogą być określone poprzez dodanie w linii poleceń kompilacji `-DDRAW_FILE=\"plik\"`, czyli np.:
	* `gcc -lGL -lGLU -lglut -lm -D MODE=1 -DDRAW_FILE=\"3b_opengl-draw-light.c\" 1a_opengl-main-glut_sdl_glx.c` dla *1a_opengl-main-glut_sdl_glx.c* w trybie GLUT z plikiem *3b_opengl-draw-light.c*
	* `gcc -lGL -lGLU -lglut -lm -D DRAW_FILE=\"3a_opengl-draw-simple.c\" 1b_opengl-main-glut.c` dla *1b_opengl-main-glut.c* z plikiem *3a_opengl-draw-simple.c*

Plik *1b_opengl-main-glut.c* może być kompilowany dodatkowo z opcją `-DUSE_SHADER` celem użycia obsługi shaderów z [2_opengl-load_shader.c](2_opengl-load_shader.c).

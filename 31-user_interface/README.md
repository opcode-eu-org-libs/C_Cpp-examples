# (Graficzny) interfejs użytkownika

1. (pseudo graficzny) interfejs użytkownika w terminalu, czyli sekwencje sterujące, ncurses, itp:
	1. [terminal/1-sekwencje_sterujace.c](terminal/1-sekwencje_sterujace.c)
		* bezpośrednie operowanie [sekwencjami sterującymi ANSI](http://blog.opcode.eu.org/2021/07/25/sekwencje_sterujace_terminala.html) (i ustawieniami terminala via `tcsetattr`)
	2. [terminal/2-termcap.c](terminal/2-termcap.c)
		* użycie biblioteki termcap
	3. [terminal/3-terminfo.c](terminal/3-terminfo.c)
		* użycie biblioteki terminfo (i ustawień via `tcsetattr`)
	4. [terminal/4-ncurses.c](terminal/4-ncurses.c)
		* użycie( wysokopoziomowej) biblioteki ncurses
2. [text_on_framebuffer.c](text_on_framebuffer.c)
	* program prezentujący korzystanie z surowego framebuffer'a i biblioteki FreeType do renderingu tekstu
3. [gtk.c](gtk.c)
	* proste demonstracyjne GUI dla GTK+ (wyświetlenie okienka, obsługa przycisku, wyświetlenie i obsługa okienka dialogowego), działające zarówno w wersji 2 jak i 3
4. [qt.cpp](qt.cpp)
	* proste demonstracyjne GUI dla Qt (wyświetlenie okienka, obsługa przycisku, wyświetlenie i obsługa okienka dialogowego), działające zarówno w wersji 4 jak i 5

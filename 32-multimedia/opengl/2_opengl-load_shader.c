/** plik odpowiedzialny za obsługę ładowania i kompilacji shaderów
 *
 * shadery są programami działającymi na karcie graficznej i zmieniającymi
 * standardowy sposób przetwarzania grafiki
 *
 * ogólnie wyróżniamy dwa typy shaderów:
 *  - vertex shader, który operuje na wierzchołkach
 *  - fragment shader, który operuje na już zrasteryzowanych punktach
 *   (pixelach mających 3 wymiarowe współrzędne i kolor ...)
 *
 * w przypadku OpenGL język w którym pisane są te programy nazywa się GLSL
 * i od strony składniowej przypomina C, jednak z zupełnie inną biblioteką
 * (nie mamy funkcji biblioteki standardowej, za to są specyficzne funkcje tego języka)
 *
 * pełna specyfikacja tego języka dostępna jest na http://www.opengl.org/documentation/glsl/
**/

#include <GL/glew.h>
#include <GL/glut.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>

// for fstat()
#include <sys/types.h>
#include <sys/stat.h>
// for close()
#include <unistd.h>
// for open()
#include <fcntl.h>
// for mmap() munmap()
#include <sys/mman.h>

#ifndef _POSIX_MAPPED_FILES 
#error Wymagany system z obsługą POSIXowego mapowania plików do pamięci - funkcja mmap
#endif

/// funkcja mapująca plik tekstowy do pamięci, 
/// zwraca wskażnik na początek pliku,
/// jeżeli length != NULL ustawia w nim długość pliku
char* MapTextFile(const char *file_path, size_t *length, char null_end) {
	// otwieranie pliku - dostęp przez deskryptor
	int desc = open(file_path, O_RDONLY);
	if (desc == -1) {
		return NULL;
	}
	
	// uzyskiwanie informacji o pliku
	// robimy to w mniej przenośny ale sprytniejszy sposób niż przy uzyciu fopen, fseek, ftell
	struct stat info;
	fstat(desc, &info);
	
	// wczytanie pliku, a dokładniej zamapowanie go do pamięci
	// to również jest mniej przenośny, ale sprytniejszy sposób niż malloc, fread
	char* addr;
	if (null_end) {
		info.st_size++;
		addr = (char*)mmap(NULL, info.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE, desc, 0);
	} else {
		addr = (char*)mmap(NULL, info.st_size, PROT_READ, MAP_SHARED, desc, 0);
	}
	if (addr == MAP_FAILED) {
		close(desc);
		return NULL;
	}
	// ustawienie końcącego NULLa
	if (null_end) {
		addr[info.st_size]='\0';
	}
	
	// zamknięcie deskryptora
	close(desc);
	
	// przekazanie informacji o długości alokowanego obszaru
	if (length != NULL) {
		*length= (size_t) info.st_size;
	}
	
	// zwrócenie adresu
	return addr;
}

/// funkcja tworzy shader o podanym typie, na podstawie podanego pliku
/// zwracając jego numeryczny identyfikator
GLuint MakeShader(GLenum type, const char *file_path) {
	const GLchar *txt;
	size_t txt_len;
	GLuint shader;
	
	// wczytanie tekstu shadera
	txt = MapTextFile(file_path, &txt_len, 1);
	if (txt == NULL) {
		fprintf(stderr, "%s:%d - Nie można wczytać pliku shadera %d (%d: \"%s\")\n",
			__FILE__, __LINE__, file_path, errno, strerror(errno));
		return 0;
	}
	
	// utworzenie shadera
	shader = glCreateShader(type);
	// ustawienie źródła shadera
	glShaderSource(shader, 1, &txt, NULL);
	// kompilacja shadera
	glCompileShader(shader);
	
	// odmapowanie pliku z tekstem shadera
	munmap((void*)txt, txt_len);
	
	return shader;
}

/// funkcja tworzy "program" na podstawie zadanych tablic z typami shaderów i ścieżkami do plików je definujących
/// funkcja ponadto instaluje utworzony program (jeżeli use !=0)
/// funkcja zwraca numeryczny identyfikator zlinkowanego programu
GLuint MakeShadersProgram(const GLenum types [], const char * files [], int num, char use) {
	// types -> tablica wartości typu GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, ...
	// files -> tablica napisów będących ściezkami do shaderów
	// przed wywołaniem tej funkcji należy robić glewInit(); (przynjamniej gdy używamy "Mesa")
	
	// tworzy program
	GLuint program = glCreateProgram();
	
	// podłączamy do niego shadery
	int i;
	for (i=0; i<num; i++) {
		glAttachShader( program, MakeShader(types[i], files[i]) );
	}
	
	// linkujemy i aktywujemy program
	glLinkProgram(program);
	if(use) glUseProgram(program);
	
	return program;
}

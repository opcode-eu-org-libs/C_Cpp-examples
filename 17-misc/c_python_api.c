/*
 * Plik prezentuje korzystanie z skryptów Python'a w środowisku C
 *
 * kompilacja: gcc -lpython3.7m c_python_api.c
 * (wymagany pakiet: libpython3.7-dev)
 */

#include <python3.7/Python.h>

int main() {
	// inicjalizacja interpretera pythona
	Py_Initialize();
	if( !Py_IsInitialized() ) {
		puts("Błąd inicjalizacji Pythona");
		return 1;
	}
	
	// wykonanie kodu pythonowego z napisu
	PyRun_SimpleString("print('Komunikat z Pythona')");
	
	// pobranie wskaźnika na słownik pythonowych zmiennych globalnych
	PyObject* globals = PyImport_AddModule("__main__");
	
	// ustawienie zmiennej xyz na 13
	PyObject *x = PyLong_FromLong(13);
	PyObject_SetAttrString(globals, "xyz", x);
	
	// wykonanie kodu pythonowego operującego na zmiennej
	PyRun_SimpleString(
		"qwv = xyz**3\n"
		"print(xyz, qwv)"
	);
	
	// pobranie wartości zmiennej z Pythona
	PyObject* val = PyObject_GetAttrString(globals, "qwv");
	if(val) {
		printf("qwv = %d\n", PyLong_AsLong(val));
	}
	
	// oczywiście to tylko drobna próbka tego co można zrobić ...
	// możliwe jest m.in. eksportowanie do pythona bardziej złożonych typów danych,
	// tworzenie funkcji i modułów pythonowych z poziomu kodu C, wykonywanie całych plików, itd.
	
	// trochę więcej współpracy (ale w C++ z użyciem biblioteki Boost) zostało przedstawione w
	// http://vip.opcode.eu.org/#API_pythonowe_biblioteki_C
}

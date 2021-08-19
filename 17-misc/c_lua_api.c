/*
 * Plik prezentuje korzystanie z skryptów Lua w środowisku C.
 * Wraz z tym kodem udostępniany jest także c_lua_api.lua
 * który jest przykładem skryptu Lua stworzonym do współpracy z tym programem
 *
 * kompilacja: gcc -llua5.3 c_lua_api.c
 * (wymagany pakiet: liblua5.3-dev)
 */

#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

static int funkcja_dla_lua (lua_State *LS) {
	// zdejmujemy ze stosu liczbę argumentów przekazaną do funkcji
	int liczba_argumentow = lua_gettop(LS);
	int i;
	lua_Number wynik = 0;
	for (i = 1; i <= liczba_argumentow; i++) {
		int arg_type = lua_type(LS, i);
		
		// wypisujemy każdy argument
		printf("Agrument nr %d typu %s (%d) wartość ", i, lua_typename(LS, arg_type), arg_type);
		switch (arg_type) {
			case LUA_TNUMBER:
				printf("%f\n", lua_tonumber(LS, i));
				break;
			case LUA_TSTRING:
				printf("%s\n", lua_tostring(LS, i));
				break;
			default :
				printf("(nieobslugiwany typ)\n");
		}
		
		// jeżeli argument jest liczbą to sumujemy
		// w zasadzie można by to robić w powyższym case, ale chciałem pokazać funkcje lua_isnumber
		if (lua_isnumber(LS, i))
			wynik += lua_tonumber(LS, i);
	}
	printf("Wynik %f\n", wynik);
	
	// odkładamy na stos wartości zwracane przez funkcję
	lua_pushnumber(LS, wynik);
	lua_pushstring(LS, "UDAŁO SIĘ");
	
	// zwracamy liczbę argumentów odłożonych jako wynik
	return 2;
}

int main(int argc, char *argv[]) {
	// wywołujemy funkcję tworzącą nowy "kontekst" obsługi skryptów Lua
	// w rzeczywistości jest to funkcja z lauxlib obudowująca wywołania lua_newstate itd
	lua_State *LS = luaL_newstate();
	if (LS == NULL) {
		puts("Nie udało się zainicjalizować systemu Lua");
		return -1;
	}
	
	// ładujemy do wskazanego kontekstu wszystkie biblioteki języka Lua
	luaL_openlibs(LS);
	
	// korzystamy z kolejnej funkcji lauxlib
	// tym razem upraszcza ona ładowanie skryptów do "kontekstu"
	if ( luaL_loadfile(LS, "c_lua_api.lua") ) {
		// wyświetlamy błąd zwrócony przez lua - jest na szczycie stosu
		printf("Nie udało się załadować skrypt Lua %s", lua_tostring(LS, -1));
		// kończymy działanie
		lua_close(LS);
		return -1;
	}
	
	// przygotowujemy dane na których będzie operował skrypt - tablica
	lua_newtable(LS);
	
	lua_pushnumber(LS, 1); // kładziemy indeks
	lua_pushfstring (LS, "a=%d b=%s", 3, "aby"); // kładziemy wartość
	lua_rawset(LS, -3); // tworzymy parę zgrupowaną w tablicy
	
	lua_pushstring(LS, "dwa"); // powtarzamy dla kolejnych elementów tablicy ...
	lua_pushnumber(LS, 8);
	lua_rawset(LS, -3);
	
	lua_setglobal(LS, "tabelka"); // nadajemy nazwę naszej tablicy
	
	// funkcja
	lua_pushcfunction(LS, funkcja_dla_lua);
	lua_setglobal(LS, "funkcja_z_c");
	
	// wykonanie skryptu
	if ( lua_pcall(LS, 0, LUA_MULTRET, 0) ) {
		printf("Nie udało się wykonać skrypt Lua %s", lua_tostring(LS, -1));
		lua_close(LS);
		return -1;
	}
	
	printf("Wynik %f %f %s\n",
		lua_tonumber(LS, -3), lua_tonumber(LS, -2), lua_tostring(LS, -1));
	// warto zwrócić uwagę na odwrotną kolejność odwołań niż kładzenia - stos ...
	// warto także zauważyć że lua_tonumber daje nam wartość zmiennoprzecinkową
	
	// zdejmujemy wszystkie 3 wyniki ze stosu
	printf("Elementów na stosie: %d\n", lua_gettop(LS));
	lua_pop(LS, 3);
	
	// wykonanie napisu języka Lua, w tym wypadku polegające na uruchomieniu wskazanej funkcji
	// więcej na http://lua-users.org/wiki/LuaGenericCall
	luaL_dostring(LS, "return jakas_funkcja(2.13)");
	printf("Zwróciło: %f\n", lua_tonumber(LS, -1));
	
	// niszczymy mechanizm obsługi Lua
	lua_close(LS);
}

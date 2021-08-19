# OnWire

## z użyciem I2C i DS2482

Katalog zawiera mikro-projekt związany z odczytem temperatur z czujników 1 wire (DS18B20).

* [1wire_rom.c](1wire_rom.c) – odczyt adresu urządzenia 1wire
* [1wire_temperatura.c](1wire_temperatura.c) – odczyt temperatury z termometru o adresie podanym w linii poleceń
* [1wire_temperatura-zabbix.c](1wire_temperatura-zabbix.c) – system odczytu temperatur w oparciu o bazę czujników w SQLite z wysyłaniem wyników do Zabbixa

* [i2c-1wire.h](i2c-1wire.h) – funkcje do obsługi kontrolera 1wire z interfejsem I2C (DS2482)
* [zabbix.c](zabbix.c) i [zabbix.h](zabbix.h) – minimalistyczna implementacja sendera zabbixowego
* [error_reporting.h](error_reporting.h) – plik nagłówkowy definiujący makra odpowiedzialne za wyświetlanie bądź zapisywanie do logu systemowego informacji zgłaszanych przez program

## z użyciem GPIO

Możliwa jest też obsługa OneWire na GPIO.
Minimalistyczne takie podejście z aktywnym czekaniem służące do odczytu numeru ROM poprzez port LPT zaprezentowane jest w podkatalogu [on_GPIO](on_GPIO).

* [on_GPIO/gpio-1wire.h](on_GPIO/gpio-1wire.h) – prosta implementacja OneWire na GPIO z aktywnym czekaniem, może być używana na platformie PC jak też w mikrokontrolerach
* [on_GPIO/onewire_get_rom-pc.c](on_GPIO/onewire_get_rom-pc.c) – odczyt adresu urządzenia 1wire poprzez GPIO dla komputerów PC z portem LPT

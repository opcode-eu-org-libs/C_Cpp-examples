# Modbus

## minimalny master RTU

Katalog zawiera mikro-projekt związany z implementacją mastera protokołu Modbus RTU.

* [modbus_rtu.c](modbus_rtu.c) i [modbus_rtu.h](modbus_rtu.h) – prosta implementacja funkcji związanych z komunikacją poprzez protokół Modbus RTU
* [modbus_read_write.c](modbus_read_write.c) – program wykorzystujący implementację z `modbus_rtu.c` do odczytu podanego rejestru modbusowego, bądź zapisu do niego podanej wartości
* [modbus_listener.c](modbus_listener.c) – program wykorzystujący implementację z `modbus_rtu.c` do podsłuchiwania komunikacji Modbus

* [error_reporting.h](error_reporting.h) – plik nagłówkowy definiujący makra odpowiedzialne za wyświetlanie bądź zapisywanie do logu systemowego informacji zgłaszanych przez program
* [crc.c](crc.c) – obliczanie sumy kontrolnej CRC w standardzie modbusowym (zaczerpnięta z [CRC Implementation Code in C by Michael Barr](http://www.netrino.com/Embedded-Systems/How-To/CRC-Calculation-C-Code), public domain)

## libmodbus

Wsparcie dla Modbus RTU i TCP oferuje biblioteka [libmodbus](https://libmodbus.org/).
Przykładowym narzędziem ją wykorzystującym jest [mbpoll](https://github.com/epsilonrt/mbpoll).

* [modbusTCP.c](modbusTCP.c) – prosty program używający biblioteki libmodbus do wysłania zapytania o grupę rejestrów do urządzenia slave Modbus TCP.

## protokół Modbus

Pełną specyfikację protokołu można znaleźć na [https://modbus.org/specs.php](https://modbus.org/specs.php).

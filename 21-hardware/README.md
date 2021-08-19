# Bliżej sprzętu i jądra

1. [zabawy_pamiecia.c](zabawy_pamiecia.c)
	* program pokazujący jak alokowane są zmienne (na stosie i poza nim)
2. [ups_off.c](ups_off.c)
	* użycie portu COM w roli GPIO do komunikacji z UPSem
3. [zawieszacz.c](zawieszacz.c)
	* program pokazujący jak wyłączyć przerwania aby komputer zajmował się tylko naszym programem
4. [czekanie.c](czekanie.c)
	* metody precyzyjnego odczekiwania interwałów czasowych (rejestr procesora RDTSC)
5. [priorytety.c](priorytety.c)
	* ustawianie priorytetu nice dla naszego procesu
	* ustawianie priorytetów i kolejkowania czasu rzeczywistego – alternatywa dla wyłączania przerwań
6. [test_libusb.c](test_libusb.c)
	* użycie biblioteki libusb pozwalającej na obsługę USB w przestrzeni użytkownika (a nie module jądra)
7. [OneWire](OneWire)
	* mikro projekt demonstrujący korzystanie z OneWire za pośrednictwem konwertera I2C-OneWire (DS18B20)
	* zawiera także obsługę bazy SQLite oraz minimalistyczny kod wysyłający dane do serwera Zabbix
	* zawiera prosty przykład obsługi Onewire na GPIO (np. porcie LPT)
8. [Modbus](Modbus)
	* mikro projekt demonstrujący prostą własną implementację mastera Modbus RTU
	* przykład użycia libmodbus

## Moduły jądra

Obsługa sprzętu może (a często nawet musi) być realizowana w ramach jądra systemu operacyjnego.
W przypadku Linuxa odbywa się to w ramach modułów jądra.

Kiedyś w tym miejscu znajdowały się przykładowe kody modułów, jednak jako że były one tworzone dla wczesnych jąder 2.6, a API Linuxa trochę się zmieniło od tego czasu to uległy dezaktualizacji.
Dlatego zachęcam do zapoznania się ze świeższym przykładem w [Writing a Simple Linux Kernel Module](https://blog.sourcerer.io/writing-a-simple-linux-kernel-module-d9dc3762c234).

Warto dodać iż podobnie jak prezentowany tam kod tworzy i używa urządzenie w `/dev` można korzystać także z plików w systemach `/proc` i `/sys` pozwalających na konfigurację, modyfikację działania modułu w trakcie jego pracy.
Jeżeli (tak jak autor tamtego artykułu) określimy licencję naszego modułu jako GPL (zalecaną dla modułów jądra),
	to możemy też bez problemu posiłkować się przykładami rozwiązań (czy też całymi fragmentami kodu) z modułów jądra wchodzących w skład jego źródeł dostępnych na [https://www.kernel.org/](https://www.kernel.org/).
Więcej o modułach także w: [The Linux Kernel Module Programming Guide](http://www.faqs.org/docs/kernel/) i [Linux Device Drivers](http://lwn.net/Kernel/LDD3/).

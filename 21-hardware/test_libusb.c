/*
 * Plik prezentuję użycie biblioteki libusb pozwalającej na obsługę urządzeń USB (komunikacji z urządzeniem USB)
 * w warstwie użytkownika (bez konieczności tworzenia modułu jądra).
 * 
 * Przykład operuje na układzie konwertera USB–UART FT232RL.
 * 
 * Kompilacja gcc -lusb test_libusb.c
 *
 */

#include <stdio.h>
#include <usb.h>

int main(void) 
{ 
	/// struktury dla magistali i urządzenia
	struct usb_bus *bus; 
	struct usb_device *dev; 

	/// inicjalizacja
	usb_init(); 
	usb_find_busses(); 
	usb_find_devices(); 

	/// wyszukiwanie urządzenia
	char find = 0;
	for (bus = usb_busses; bus && find == 0; bus = bus->next)
		for (dev = bus->devices; dev; dev = dev->next)
			if ( dev->descriptor.idVendor == 0x0403
				 && dev->descriptor.idProduct == 0x6001
//				 && dev->descriptor.iSerialNumber == 0x0000
			   )
			{
				find = 1;
				break;
			}
	if (find != 1) {
		puts("nie znaleziono urządzenia");
		return -1;
	}
	free(bus); // bus nie będzie nam już potrzebne, znalezione urządzenie mamy w dev
	
	/// obsługa urządzenia
	usb_dev_handle *usbdev; 
	usbdev = usb_open(dev);
	if (!usbdev) {
		puts("błąd otwarcia urządzenia"); 
		return -2;
	}
	
	// tworzymy interfejs na urządzeniu
	if (usb_claim_interface(usbdev, 0)) {
		puts("błąd utworzenia interfejsu");
		puts(" upewnij się że nik nie korzysta z urządzenia, spóbój `rmmod ftdi_sio`");
		usb_close (usbdev);
		return -3;
	}
	
	/// komunikacja z urządzeniem
	// ustawienia konfiguracyjne:
	//	usb_control_msg(usbdev, requesttype, request, value, index, *bytes, size, timeout);
	// wysłanie danych:
	//	usb_bulk_write(usbdev, ep, *bytes, size, timeout);
	// odczyt danych:
	//	usb_bulk_read(usbdev, ep, *bytes, size, timeout);
	//	UWAGA: bufor odbiorczy musi być zdeklarowany jako static unsigned char []
	
	// reset device
	if (usb_control_msg(usbdev, 0x40, 0, 0, 0, NULL, 0, 3000) != 0)
		puts("błąd resetu układu");
//	if (usb_control_msg(usbdev, 0x40, 3, 16696, 0, NULL, 0, 3000) != 0)
//		puts("Błąd ustawienia prędkości");
	
	// send data
	char * aaa="abbcb";
	int i;
	for (i=0; i<5; i++) {
		printf("Wysłano: %d\n", usb_bulk_write(usbdev, 0x02, aaa+i, 1, 3000));
		// UWAGA: wysyłana porcja danych nie może być zbyt duża !!!
		sleep(1);
	}
	
	puts("Czekam na dane wejściowe");
	int j, k;
	for (j=0; j<5; j++) {
		// read data
		static unsigned char buf[3];
		i = usb_bulk_read(usbdev, 0x81, buf, 3, 3000);
		// UWAGA: bufor mniejszy od 3 znakówe nie ma sensu ze względu na sposób transmisji ...
		if (i==3) {
			printf("%d : ", j);
			for (k=0; k<i; k++)
				printf("%X ", buf[k]);
			puts("");
		} else {
			j--;
		}
		
		usleep(200000);
	}
		
		
	/// zakończenie
	usb_release_interface(usbdev, 0);
	usb_close (usbdev);
	return 0; 
}

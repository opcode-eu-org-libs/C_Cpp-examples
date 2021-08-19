#include <stdio.h>

#include <stdint.h>   // typy o ustalonej długości bitowej
#include <stdbool.h>  // typ bool

void print_binary(uint32_t x) {
	bool ctr = false;
	
	for (int i=31; i>=0; --i) { // w argumencie użyliśmy uint32_t, więc wiemy że x zawsze będzie miał 32 bity
		int y = (x >> i) & 0x1; // wyciągam kolejny bit (w dobrej kolejności -- pętla zmniejsza wartość i)
		                        // w tym celu przesuwamy liczbę o i pozycji w prawo (operatorem >> i)
		                        // i uwzględniamy tylko najmłodszy (najbardziej z prawej) bit przy pomocy bitowego AND z 0x1
		if (ctr || i==0) {     // jeżeli był już niezerowy to ...
			printf("%d", y); // ...  wypisuje
		} else if (y == 1){ // jeżeli nie było niezerowego a teraz jest to ...
			printf("%d", y); // ... wypisuje i ...
			ctr = true; // ... zaznaczam ze był niezerowy
		}
	}
	printf("\n");
}

void print_binary_with_zero(uint32_t x) {
	for (int i=31; i>=0; i--)
		printf("%d", (x >> i) & 0x1);
	printf("\n");
}


int main() {
	uint32_t a = 0x14, b = 0xff;
	
	puts( "przesunięcia bitowe, a = " );
	print_binary_with_zero( a );
	puts( " - w prawo" );
	for (int i=0; i<7; ++i) {
		print_binary_with_zero( a >> i );
	}
	puts( " - w lewo" );
	for (int i=0; i<7; ++i) {
		print_binary_with_zero( a << i );
	}
	// warto zwrócić uwagę że w obu wypadkach dopełnianie jest zerami ...
	
	// warto także zauważyć że (x >> y) odpowiada x / (2 do potęgi y)
	printf("%d == %d\n", a >> 3, a / 8);
	// natomiast (x << y) odpowiada x * (2 do potęgi y)
	printf("%d == %d\n", a << 3, a * 8);
	
	
	puts( "operacje logiczne na bitach" );
	printf("a = "); print_binary(a);
	printf("b = "); print_binary(b);
	
	printf("~a = "); print_binary(~a); // NEGACJA bitowa
	
	printf("a & b = "); print_binary(a & b); // AND na poszczególnych bitach
	                                         // wynik będzie miał bity o wartości 1 na tych pozycjach gdzie w obu liczbach bity miały wartość 1
	printf("a | b = "); print_binary(a | b); // OR  na poszczególnych bitach
	                                         // wynik będzie miał bity o wartości 1 na tych pozycjach gdzie w jednej z liczb bity miały wartość 1
	printf("a ^ b = "); print_binary(a ^ b); // XOR na poszczególnych bitach
	                                         // wynik będzie miał bity o wartości 1 na tych pozycjach gdzie w bity miały różną wartość w obu liczbach
	
	puts( " - operacje te są przemienne" );
	printf("b & a = "); print_binary(b & a); // AND na poszczególnych bitach
	printf("b | a = "); print_binary(b | a); // OR  na poszczególnych bitach
	printf("b ^ a = "); print_binary(b ^ a); // XOR na poszczególnych bitach
	
	puts( " - vs 'zwykłe' logiczne" );
	// należy mieć świadomość że zachowują się one inaczej od (zwykłych) operacji logicznych:
	printf("!a = "); print_binary(!a);
	printf("a && b = "); print_binary(a && b);
	printf("a || b = "); print_binary(a || b);
	
	puts( "\nprzykładowe zastosowania" );
	// operacje te są stosowane głównie gdy potrzebujemy operować na bitach jakiejś liczby 
	// np. gdy mamy 8 bitowy port I/O do którego są podpięte LEDy i chcemy zgasić lub zapalić tylko jeden z nich nie zmieniając stanu pozostałych
	// old = old | 1 << numer_LED_do_wlaczenia;   old = old & ~(1 << numer_LED_do_wylaczenia)
	//
	// są także używane do podziału adresu IP na adres sieci i hosta w danej sieci:
	
	puts( " adres IP 192.168.25.13/23 ... czyli z długością prefixu 23 bity" );
	
	uint32_t numer_ip       = 0xC0A8190D;              // 192.168.25.13
	printf("  → Numer IP:              "); print_binary_with_zero(numer_ip);
	
	uint32_t maska_podsieci = (0xFFFFFFFF << (32-23)); // 255.255.254.0
	// maska jest 32 bitową liczbą (w IPv4) i ma ustawione na 1 tyle bitów od lewej (najstarszych) ile wynosi długość prefixu
	printf("  → Maska podsieci:        "); print_binary_with_zero(maska_podsieci);
	
	uint32_t adres_sieci = numer_ip & maska_podsieci;
	// logiczny operator bitowy & wykonuje logiczne AND na odpowiadających bitach swoich argumentów
	printf("  → Adres sieci:           "); print_binary_with_zero(adres_sieci);

	uint32_t adres_rozgloszeniowy = adres_sieci | ( ~ maska_podsieci);
	// logiczny operator bitowy | wykonuje logiczne OR na odpowiadających bitach swoich argumentów
	// logiczny operator bitowy ~ wykonuje negacje każdego bitu swojego argumentu
	printf("  → Adres rozgłoszeniowy:  "); print_binary_with_zero(adres_rozgloszeniowy);
	
	
	printf("\nAdres IP to: %X,\n", numer_ip);
	printf("    co jak łatwo przeliczyć odpowiada 192.168.25.13:\n        192=0xC0, 168=0xA8, 25=0x19, 13=0x0D\n");
	printf("\nMaska podsieci to: %X,\n", maska_podsieci);
	printf("    co jak łatwo przeliczyć odpowiada 255.255.254.0:\n        255=0xFF, 254=0xFE\n");
	printf("\nAdres sieci to: %X,\n", adres_sieci);
	printf("    co jak łatwo przeliczyć odpowiada 192.168.24.0:\n         192=0xC0, 168=0xA8, 24=0x18\n");
	printf("\nAdres rozgłoszeniowy to: %X,\n", adres_rozgloszeniowy);
	printf("    co jak łatwo przeliczyć odpowiada 192.168.25.255:\n       192=0xC0, 168=0xA8, 25=0x19, 255=0xFF\n");
}

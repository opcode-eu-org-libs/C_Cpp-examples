/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje użyteczne funkcje związane z uzyskiwaniem informacji o czasie
 * 
 * część z omawianych w tym przykładzie funkcji określona jest w standardzie języka C (np. time, strftime),
 * inne jedynie w standardach POSIXowych (np. tzset, gettimeofday),
 * a jeszcze inne są w POSIX ale będą dodana do standardu C (np. localtime_r)
 *
 */

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

size_t time2str(
	const time_t* time, const char *format, const char* timeZone,
	char *buf, size_t bufLen, struct tm *tmPtr
) {
	struct tm tmTmp;
	if (!tmPtr) {
		tmPtr = &tmTmp;
	}
	
	char oldTZ[128];
	if (timeZone) {
		strncpy(oldTZ, getenv("TZ"), 128);
		setenv("TZ", timeZone, 1);
		tzset();
	}
	
	if (buf) {
		localtime_r(time, tmPtr);
		bufLen = strftime(buf, bufLen, format, tmPtr);
	} else {
		bufLen = 0;
	}
	
	if (timeZone) {
		setenv("TZ", oldTZ, 1);
		tzset();
	}
	
	return bufLen;
}

int main() {
	setenv("TZ", "Europe/Warsaw", 1);
	
	time_t czas = time(0);
		// czas mierzony w sekundach od początku epoki (czyli 1970-01-01 00:00:00 UTC),
		// patrz: man 2 time
		// możemy też korzystać z wywołania: time(&czas);
	printf("Od początku epoki upłynęło %d sekund\n", czas);

	// następnie możemy utworzyć struktórę zawierającą czas rozłożony:
	struct tm czas_rozlozony_utc;
	gmtime_r(&czas, &czas_rozlozony_utc);
	// jest też localtime zwracająca czas w lokalnej strefie czasowej oraz inne funkcje
	//  patrz man 3 gmtime
	struct tm czas_rozlozony_local;
	localtime_r(&czas, &czas_rozlozony_local);
	// uwaga na wywołanie czas_rozlozony_local=localtime(&czas);
	// może nadpisać ono strukturę czas_rozlozony_utc
	// (zwróci wskaźnik do tego samego obszaru co poprzednie wywołanie)

	char czas_napis [100];
	// następnie możemy czas ten sformatować przy pomocy strftime
	strftime(czas_napis, sizeof(czas_napis), "%Y-%m-%d %H:%M:%S %Z (%s)", &czas_rozlozony_utc);
		// więcej o napisach formatujących - man 3 strftime
	printf("Mamy teraz (UTC): %s\n", czas_napis);

	strftime(czas_napis, sizeof(czas_napis), "%Y-%m-%d %H:%M:%S %Z (%s)", &czas_rozlozony_local);
	printf("Mamy teraz (local): %s\n", czas_napis);

	// albo odwołać się bezpośrednio do składowych struktury tm
	printf("Aktualna godzina w UTC to: %d\n", czas_rozlozony_utc.tm_hour);
	
	// wato zwrócić uwagę iż niektóre z pozycji napisu formatującego funkcji strftime korzystają z
	// strefy czasowej ustalonej w zmiennej środowiskowej co ma niemiłe skutki w połączeniu z gmtime()
	// jak widać na powyższym przykładzie timestamp dla czas_rozlozony_utc jest nie poprawny gdyż
	// funkcja ta zakłada że podana struktura jest czasu lokalnego u nas != UTC
	
	// aby zobaczyć to lepiej możemy zrobić to dla chwili zero
	// (która powinna odpowiadać 1970-01-01 00:00:00 UTC
	czas = 0;
	gmtime_r(&czas, &czas_rozlozony_utc);
	localtime_r(&czas, &czas_rozlozony_local);
	// należy pamiętać że funkcję powyzsze w wariancie bez _r są niebezpieczne przy wielowątkowości
	
	strftime(czas_napis, sizeof(czas_napis), "%Y-%m-%d %H:%M:%S %Z (%s)", &czas_rozlozony_utc);
	printf("t=0 z gmtime: %s\n", czas_napis);
	strftime(czas_napis, sizeof(czas_napis), "%Y-%m-%d %H:%M:%S %Z (%s)", &czas_rozlozony_local);
	printf("t=0 z localtime: %s\n", czas_napis);
	
	// widzimy już że problem jest w funkcji wykonywanej na strukturze z rozłożonym czasem w UTC
	setenv("TZ", "UTC", 1);
	strftime(czas_napis, sizeof(czas_napis), "%Y-%m-%d %H:%M:%S %Z (%s)", &czas_rozlozony_utc);
	printf("t=0 z gmtime: %s\n", czas_napis);
	strftime(czas_napis, sizeof(czas_napis), "%Y-%m-%d %H:%M:%S %Z (%s)", &czas_rozlozony_local);
	printf("t=0 z localtime: %s\n", czas_napis);
	// daje to poprawny rezultat dla UTC oraz niepoprawny dla lokalnego ...
	// co ciekawe strefa czasowa (%Z) zawsze jest poprawna, należy więc uważać
	// gdy korzystamy z gmtime oraz napisów formatujących zależnych od strefy czasowej ...
	
	// można to rozwiązać odpowiednim manipulownaiem strefą czasową,
	// np. tak jak jest to zrobione w time2str(),
	// która zastępuje bezpośrednie wywołanie gmtime_r/localtime_r oraz strftime
	// oraz dodatkowo pozwala na pobranie czasu z dowolnej strefy:
	time2str(&czas, "%Y-%m-%d %H:%M:%S %Z (%s)", "UTC", czas_napis, sizeof(czas_napis), 0);
	printf("t=0 z time2str UTC:      %s\n", czas_napis);
	time2str(&czas, "%Y-%m-%d %H:%M:%S %Z (%s)", "Europe/Warsaw", czas_napis, sizeof(czas_napis), 0);
	printf("t=0 z time2str Warsaw:   %s\n", czas_napis);
	time2str(&czas, "%Y-%m-%d %H:%M:%S %Z (%s)", "America/New_York", czas_napis, sizeof(czas_napis), 0);
	printf("t=0 z time2str New_York: %s\n", czas_napis);
	
	// mamy również funkcję mktime,
	//  która na podstawie struktury tm wyrażonej w czasie lokalnym tworzy timestamp

	// oraz funkcję gettimeofday pobierającą czas z większą dokładnością
	struct timeval czas2;
	gettimeofday(&czas2, NULL);
	printf("Od początku epoki upłynęło %d sekund i %d mikrosekund\n", czas2.tv_sec, czas2.tv_usec);
	// podobnie do gettimeofday działa clock_gettime(CLOCK_REALTIME, &time);
}

/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje ustalanie nice oraz priorytetów czasu rzeczywistego
 *
 * program z oczywistych względów powinien być uruchamiany z prawami root'a ...
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sched.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <errno.h>

int main ()
{
	// nice = -20
	// ta funkcja działa tylko na bieżącym procesie - nie pobiera PIDu
	// tak naprawdę wywoływanie jej w tym programie nie ma sensu
	// gdyż zaraz zmienimy politykę kolejkowania (i statyczny priorytet) na nieobsługującą nice'a
	setpriority (PRIO_PROCESS, 0, -20);
	
	system("ps -l"); // listujemy informacje o procesach ...
	// zwróć uwagę na wartość nice w kolumnie NI
	
	/// priorytet statyczny (rt_priority), priorytet czasu rzeczywistego
	
	// deklaracje zmiennych
	pid_t pid;
	struct sched_param sp;
	
	
	pid = getpid(); // pobranie identyfikatora bieżącego procesu
	sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
	// uwaga: przy wysokim priorytecie (tak jak w tym przykładzie)
	//        może nie być możliwe zabicie programu
	// przy niższych priorytetach proces powinno dać się zabić kombinacja Ctrl+C ...
	
	// ustawienie trybu szeregowania oraz priorytetu
	// funkcja ta może być wykorzystana do nadawania priorytetu dowolnemu procesowi
	// (nie tylko bieżącemu), wystarczy ze znamy jego PID
	if ( sched_setscheduler(pid, SCHED_RR, &sp) ) {
		perror("ERROR");
		return -1;
	}
	// w tym miejscu warto wspomnieć o metodach szeregowania:
	// * SCHED_RR jest to proste szeregowanie z kwantem czasu (proces dostaje czas,
	//   a po jego wykorzystaniu lub oddaniu procesora brany jest kolejny
	//   a ten idzie na koniec kolejki)
	// * SCHED_FIFO podobnie do SCHED_RR tyle że jedynym powodem dla którego proces może
	//   przestać być wykonywany jest to iż sam zrezygnuje (jego kwant czasu jest nieskończony)
	// * SCHED_OTHER zależy od systemu, najczęściej podobnie do SCHED_RR tyle że miejsce
	//   na którym ląduje w kolejce zależy od tego jak dużo używał CPU (im więcej tym dalej)
	// * SCHED_SPORADIC (nie implementowana aktualnie w Linuxie) jest polityką szeregowania
	//   typu serwera sporadycznego (procesy mogą chodzić z wysokim priorytetem przez zadany
	//   kwant czasu potem - do czasu odnowienia priorytetu serwera sporadycznego ich
	//   priorytet jest obniżany)
	//
	// warto zauważyć iż SCHED_FIFO wraz z sched_yield() pozwala na implementowanie szeregowania
	// powodującego przełączanie zadań tylko gdy ione tego sobie życzą (podobnie jak w PicOSie ...)
	//
	// oczywiście dla każdego z priorytetów istnieje osobna kolejka i dopóki są gotowe zadania
	// w kolejce o wyższym priorytecie, kolejka o niższym nie ma nic do powiedzenia
	
	
	// metoda ta wydaje się równie skuteczna jak opisane w zawieszacz.c wyłączanie przerwań,
	// przy czym jest rozwiązaniem bardziej eleganckim oraz umożliwiającym korzystanie z wielu
	// funkcji bibliotecznych (co nie jest możliwe przy wyłączaniu obsługi przerwań)
	
	
	// zastąpienie aktualnie wykonywanego programu przez program który chcemy tak uruchomić ...
	// używamy ps aby pokazać jak wygląda w nim taki proces
	execlp("ps", "ps", "-l", 0);
	// zwróć uwagę na wartość priorytetu w kolumnie PRI
	
	// więcej o szeregowaniu procesów pod Linuxem: `man 2 sched_setscheduler`
}

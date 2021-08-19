/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C++
 * prezentuje:
 *  - tworzenie programów wielowątkowych
 *  - korzystanie z mutexów
 *  - korzystanie z asynchronicznego io jako timera
 *  - korzystanie z kolejki C++
 *  - podatwy obsługi czasu ...
 *
 * kompilacja: g++ -lboost_date_time -lboost_system -lboost_thread -lpthread watki.cpp
 * 
 * (wymagane pakiety: libboost-date-time-dev libboost-system-dev libboost-thread-dev)
 */


#include <queue>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>
#include <iostream>

std::queue<std::string> kolejka;

boost::mutex mutex;

int globalna=13;

class Watki {
public:
	int parametr_watku;
	void operator()() {
		std::cout << "to wątek z parametrem " << parametr_watku;
		std::cout << " globalna = " << globalna << std::endl;
		boost::this_thread::sleep(boost::posix_time::seconds(5));
		std::cout << "a teraz (nadal) z parametrem " << parametr_watku;
		std::cout << " globalna = " << globalna << std::endl;
		
		
		for (int i=0; i<6; i++) {
			boost::this_thread::sleep(boost::posix_time::millisec(700));
			boost::mutex::scoped_lock scoped_lock(mutex);
			// powyższy mutex blokuje od teraz do końca bloku
			// warto też wspomnieć o muteksach odpornych na blokowanie
			// już zablokowanego przez nas samych - boost::recursive_mutex
			
			if (!kolejka.empty()) {
				std::cout << i << ") odebrałem z kolejki " << kolejka.front() << std::endl;
				kolejka.pop();
			}
		}
		
	}
};

void zdarzenie(const boost::system::error_code&) {
	std::cout << "budzik start" << std::endl;
	boost::this_thread::sleep(boost::posix_time::seconds(5));
	std::cout << "budzik stop" << std::endl;
}


int main() {
	Watki funktor_watek1;
	
	funktor_watek1.parametr_watku=56;
	boost::thread watek1(funktor_watek1);
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	funktor_watek1.parametr_watku=78;
	globalna=58;
	
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	
	mutex.lock();
	kolejka.push("Ala ma kota");
	kolejka.push("Kot ma Ale");
	mutex.unlock();
	
	std::cout << "2 pierwsze elementy są w kolejce" << std::endl;
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	
	mutex.lock();
	kolejka.push("Abecadlo");
	kolejka.push("z pieca spadło");
	mutex.unlock();
	
	std::cout << "2 kolejne elementy są w kolejce" << std::endl;
	
	// asynchroniczne IO jako alarm
	boost::asio::io_service ios;
	boost::asio::deadline_timer budzik(ios, boost::posix_time::seconds(2));
	budzik.async_wait(zdarzenie);
	std::cout << "czekamy na budzik" << std::endl;
	ios.run(); // uruchomienie pętli obsługi
	std::cout << "po budziku" << std::endl;
}

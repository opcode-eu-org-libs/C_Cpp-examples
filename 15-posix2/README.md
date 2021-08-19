# Trochę więcej niż podstawy programowania Unixa/Linuxa

1. [watki.c](watki.c)
	* aplikacje wielowątkowe z wykorzystaniem *pthread*
2. [watki_clone.c](watki_clone.c)
	* aplikacje wielowątkowe z wykorzystaniem `clone()`
3. [semafory_posix.c](semafory_posix.c)
	* używanie semaforów, pamięci współdzielonej, timerów, kolejek komunikatów oraz asynchronicznego wyjścia zgodnych ze standardem [POSIX](https://pl.wikipedia.org/wiki/POSIX)
4. [semafory_ipc.c](semafory_ipc.c)
	* wykorzystywanie semaforów i innych mechanizmów SystemV [IPC](https://pl.wikipedia.org/wiki/Komunikacja_mi%C4%99dzyprocesowa) (svipc) do komunikacji między procesami
5. [tty-listener.c](tty-listener.c)
	* program wykorzystujący urządzenia vcs i vcsa do podebrania ekranu (tekstu) z terminala tty

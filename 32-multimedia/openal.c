/**
 * program prezentuje podstawy korzystania z biblioteki OpenAL
**/

// nagłówki OpenAL
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// dokumentacja biblioteki dostępna na:
// http://connect.creativelabs.com/openal/Documentation/Forms/AllItems.aspx

// kompilacja: gcc -lopenal -lalut openal.c
// (wymagane pakiety: libopenal-dev libalut-dev)

const int ILOSC_ZRODEL = 2;
typedef struct {
	ALbyte* plik;
	ALfloat pitch;
	ALfloat gain;
	ALfloat pozycja[3];
	ALfloat kierunek[3];
	ALuint czy_zapetlone;
} zrodlo_desc;

zrodlo_desc OPIS_ZRODEL[2] = {
	{
		"tlo.wav",
		1.0,
		1.0,
		{0.0,0.0,0.0},
		{0.0,0.0,0.0},
		AL_TRUE
	},
	{
		"dzwiek.wav",
		1.0,
		1.0,
		{0.0,0.0,0.0},
		{0.0,0.0,0.0},
		AL_FALSE
	}
};

typedef struct {
	ALfloat pozycja[3];
	ALfloat kierunek[3];
	ALfloat orientacja[6];
} sluchacz_desc;

sluchacz_desc OPIS_SLUCHACZA = {
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0, 0, 0, 0}
};


int main(int argc, char *argv[]) {
	alutInit(NULL, 0);
	if(alGetError() != AL_NO_ERROR) {
		puts("Nie można zainicjalizować biblioteki ALUT");
		exit;
	}
	
	/// utworzenie buforów dzwiękowych
	ALuint bufory_dzwiekowe[ILOSC_ZRODEL];
	alGenBuffers(ILOSC_ZRODEL, bufory_dzwiekowe);
	if(alGetError() != AL_NO_ERROR) {
		puts("Nie można zainicjalizować buforów OpenAL");
		exit;
	}
	
	/// utworzenie źródeł dźwiękowych
	ALuint zrodla_dzwiekowe[ILOSC_ZRODEL];
	alGenSources(ILOSC_ZRODEL, zrodla_dzwiekowe);
	
	int i;
	for (i=0; i<ILOSC_ZRODEL; i++) {
		/// ładowanie buforu
		ALenum format;
		ALsizei size;
		ALvoid* data;
		ALsizei freq;
		ALboolean loop;
		
		alutLoadWAVFile(OPIS_ZRODEL[i].plik, &format, &data, &size, &freq, &loop);
		alBufferData(bufory_dzwiekowe[i], format, data, size, freq);
		alutUnloadWAV(format, data, size, freq);
		// bufory_dzwiekowe[i] = alutCreateBufferFromFile(NAZWY_PLIKOW[i]);
		
		/// ustawianie parametrów źródła
		// w ogólności ten sam bufor możemy wykorzystać w kilku źródłach
		// ale tutaj tak nie robimy, bo chcemy aby każde źródło odgrywało niezależny plik
		alSourcei (zrodla_dzwiekowe[i], AL_BUFFER,   bufory_dzwiekowe[i]);
		alSourcef (zrodla_dzwiekowe[i], AL_PITCH,    OPIS_ZRODEL[i].pitch);
		alSourcef (zrodla_dzwiekowe[i], AL_GAIN,     OPIS_ZRODEL[i].gain);
		alSourcefv(zrodla_dzwiekowe[i], AL_POSITION, OPIS_ZRODEL[i].pozycja);
		alSourcefv(zrodla_dzwiekowe[i], AL_VELOCITY, OPIS_ZRODEL[i].kierunek);
		alSourcei (zrodla_dzwiekowe[i], AL_LOOPING,  OPIS_ZRODEL[i].czy_zapetlone);
	}
	
	/// ustalamy pozycję słuchacza
	alListenerfv(AL_POSITION,    OPIS_SLUCHACZA.pozycja);
	alListenerfv(AL_VELOCITY,    OPIS_SLUCHACZA.kierunek);
	alListenerfv(AL_ORIENTATION, OPIS_SLUCHACZA.orientacja);
	
	// zaczynamy odtwarzać źródło 0
	alSourcePlay(zrodla_dzwiekowe[0]);
	// będzie odtwarzane w pętli dopóki nie zatrzymamy go poprzez
	//  alSourceStop(zrodla_dzwiekowe[0]);
	// lub
	//  alSourcePause(zrodla_dzwiekowe[0]);
	
	/// czekamy chwile, ale dźwięk się odtwarza
	sleep(10);
	
	i=0;
	while(i<5) {
		ALint stan_zrodla;
		alGetSourcei(zrodla_dzwiekowe[1], AL_SOURCE_STATE, &stan_zrodla);
		if(stan_zrodla != AL_PLAYING) {
			i++;
			// przemieszczamy źródło w losowe położenie
			OPIS_ZRODEL[1].pozycja[0]=(rand()%100)/10.0;
			OPIS_ZRODEL[1].pozycja[1]=(rand()%100)/10.0;
			OPIS_ZRODEL[1].pozycja[2]=(rand()%100)/10.0;
			alSourcefv(zrodla_dzwiekowe[1], AL_POSITION, OPIS_ZRODEL[1].pozycja);
			
			// odtwarzamy
			alSourcePlay(zrodla_dzwiekowe[1]);
		}
	}
	
	/// zakończenie
	alDeleteSources(ILOSC_ZRODEL, zrodla_dzwiekowe);
	alDeleteBuffers(ILOSC_ZRODEL, bufory_dzwiekowe);
	alutExit();
}

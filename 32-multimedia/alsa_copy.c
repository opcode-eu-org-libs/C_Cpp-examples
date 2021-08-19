/*
 * plik ma na celu prezentację ciekawych zagadnień z programowania w języku C / C++
 * prezentuje korzystanie z biblioteki dźwiękowej ALSA (odczyt i zapis dźwięku do urządzenia)
 *
 * kompilacja: gcc -lasound alsa_copy.c
 *
 * praktycznym wykorzystaniem tego programu jest implementacja pętli pomiędzy
 * wejściem liniowym a wyjściem głośnikowym w karcie dźwiękowej opartej o ICH6 + CMI9880
 *
 * o API asoundlib - http://www.alsa-project.org/alsa-doc/alsa-lib/
 * o dmix'ie - http://alsa.opensrc.org/DmixPlugin
 *
 * podobny program można zrealizować operując bezpośrednio na /dev/dsp
 * jednak obecnie obsługa audio realizowana jest raczej powyżej ALSA (np. w pulseaudio)
 * niż poniżej tej warstwy, więc zajmowanie się surowym /dev/dsp ma jeszcze mniej sensu
 * niż surową biblioteką ALSA
 */

#include <alsa/asoundlib.h>

static char *alsa_device = "default";
snd_pcm_t *h_in;
snd_pcm_t *h_out;
unsigned short buf[16*512];

int err;

int main(void) {
	// otwarcie urządzenia
	err = snd_pcm_open(&h_in, alsa_device, SND_PCM_STREAM_CAPTURE, 0);
	if (err < 0) {
		printf("ERROR (1a): %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
	
	err = snd_pcm_open(&h_out, alsa_device, SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("ERROR (1b): %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
	
	
	// ustawienie parametrów urządzenia
	err = snd_pcm_set_params(h_in, SND_PCM_FORMAT_U16, SND_PCM_ACCESS_RW_INTERLEAVED, 1, 16000, 1, 800000);
	if (err < 0) {
		printf("ERROR (2a): %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
	
	err = snd_pcm_set_params(h_out, SND_PCM_FORMAT_U16, SND_PCM_ACCESS_RW_INTERLEAVED, 1, 16000, 1, 800000);
	if (err < 0) {
		printf("ERROR (2b): %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
	
	// pętla kopiująca
	while(1) {
		err = snd_pcm_readi (h_in, buf, 8*512);
		if (err == -EPIPE) {
			puts("ERROR (3a): snd_pcm_prepare\n");
			// zapobiega wywalaniu się programu na błędach odczytu z urządzenia
			snd_pcm_prepare(h_in);
		} else if (err < 0) {
			printf("ERROR (3a): %s\n", snd_strerror(err));
		}
		
		err = snd_pcm_writei(h_out, buf, err);
		if (err < 0) {
			err = snd_pcm_recover(h_out, err, 0);
		}
		if (err < 0) {
			printf("ERROR (3b): %s\n", snd_strerror(err));
			break;
		}
	}
	
	snd_pcm_close(h_in);
	snd_pcm_close(h_out);
}

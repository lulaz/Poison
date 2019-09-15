#include "audio_header.h"

char* locat_audio;
char file_name_audio[] = "\\recorded_audio.bin";

/*
************************************************************
Author:       Luis Lázaro-Carrasco Hernández
Description:  Grabador de audio utilizando la api mmeapi.h
************************************************************
*/

DWORD WINAPI audio_main(LPVOID lpParameter) {

	// Lleno el WAVEFORMATEX struct para indicar el formato de nuestro audio grabado
	WAVEFORMATEX wfx = {};
	wfx.wFormatTag = WAVE_FORMAT_PCM;       // PCM is standard
	wfx.nChannels = 2;                      // 2 channels = stereo sound
	wfx.nSamplesPerSec = 44100;             // Samplerate.  44100 Hz
	wfx.wBitsPerSample = 16;                // 16 bit samples

	// Otros formatos:
	wfx.nBlockAlign = wfx.wBitsPerSample * wfx.nChannels / 8;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;


	// Abro nuestro dispositivo de grabacion 'waveIn' 
	HWAVEIN wi;
	waveInOpen(&wi,            // fill our 'wi' handle
		WAVE_MAPPER,    // use default device (easiest)
		&wfx,           // tell it our format
		NULL, NULL,      // we don't need a callback now
		CALLBACK_NULL | WAVE_FORMAT_DIRECT   // tell it we do not need a callback
	);

	// Ahora que tenemos nuestro dispositivo, necesitamos seleccionar sus buffers (con sus cabeceras)
	// poner el audio grabado en algun path, en este caso pasado por parametro

	char buffers[2][44100 * 2 * 2 / 2];    // 2 buffers, each half of a second long
	WAVEHDR headers[2] = { {},{} };           // initialize them to zeros
	for (int i = 0; i < 2; ++i)
	{
		headers[i].lpData = buffers[i];             // give it a pointer to our buffer
		headers[i].dwBufferLength = 44100 * 2 * 2 / 2;      // tell it the size of that buffer in bytes

		// Se prepara cada cabecera
		waveInPrepareHeader(wi, &headers[i], sizeof(headers[i]));

		// Y lo añado a la cola
		// Una vez empezamos a grabar, los buffers encolados se iran llenando con los datos de audio
		waveInAddBuffer(wi, &headers[i], sizeof(headers[i]));
	}
	
	// Se comprueba si se ha dado algun path para colocar nuestro audio grabado
	// Si este parametro es null se colocara en C:\Users\%USER%\AppData\Local\Temp por defecto
		if (lpParameter == NULL) {
			char var_name[] = "temp";
			char location_var[MAX_PATH];
			GetEnvironmentVariableA(var_name, location_var, MAX_PATH);
			strcat_s(location_var, file_name_audio);
			locat_audio = location_var;
			std::cout << "locat_audio:" << locat_audio;
			std::cout << std::endl;	
			//			locat_audio = "recorded_audio.bin";
			//std::cout << "locat_audio:" << locat_audio;
			//std::cout << std::endl;
		}
		else
			locat_audio = (char*)lpParameter;

	// Se van a copiar los datos de audio al archivo binario
	// Este archivo .bin se puede posteriormente importar en bruto en el audacity para escucharlo correctamente
	// En formato Signed 32-bit PCM
	std::ofstream outfile(locat_audio, std::ios_base::out | std::ios_base::binary);

	// se comienza a grabar
	waveInStart(wi);

	// Ahora que estamos grabando, hay que checkear los buffers para ver si se han llenado.
	// Si se llenan, hay que copiar su contenido al archivo y reubicarlos a la cola,
	// de esta forma pueden llenarse de nuevo indefinidamente.

	while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000))  // Continua el bucle hasta que se recibe un evento de escape, esto se mandara mediante el codigo 106 para finalizar la grabacion

	{
		for (auto& h : headers)      // se comprueba cada cabecera
		{
			if (h.dwFlags & WHDR_DONE)           // esta la cabecera llena?
			{
				// Si, lo copiamos a nuestro archivo
				outfile.write(h.lpData, h.dwBufferLength);

				// lo reubicamos en la cola
				h.dwFlags = 0;          // clear the 'done' flag
				h.dwBytesRecorded = 0;  // tell it no bytes have been recorded

				// lo reañadimos
				waveInPrepareHeader(wi, &h, sizeof(h));
				waveInAddBuffer(wi, &h, sizeof(h));
			}
		}
	}

	// Una vez se manda el evento escape, se para de grabar y se limpian cabeceras
	waveInStop(wi);
	for (auto& h : headers)
	{
		waveInUnprepareHeader(wi, &h, sizeof(h));
	}
	waveInClose(wi);

	return NULL;

}


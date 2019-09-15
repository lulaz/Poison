#include "Infinite_loop_header.h"

DWORD WINAPI pi_dec(LPVOID lpParameter)
{
	const int n = 999999999;
	const int dim = ((10 * n) / 3);
	int i, j, k, resto, digant, nueves, aux;
	int pi[dim + 1];

	for (i = 1; i <= dim; i++) pi[i] = 2;
	nueves = 0; digant = 0;
	for (i = 1; i <= n; i++)
	{
		resto = 0;
		for (j = dim; j >= 1; j--)
		{
			aux = 10 * pi[j] + resto*j;
			pi[j] = aux % (2 * j - 1);
			resto = aux / (2 * j - 1);
		}
		pi[1] = resto % 10;
		resto = resto / 10;
		if (resto == 9) nueves++;
		else if (resto == 10)
		{
			//for (k = 1; k <= nueves; k++) printf("0");
			digant = 0;
			nueves = 0;
		}
		else
		{
			digant = resto;
			if (nueves != 0)
			{
				//for (k = 1; k <= nueves; k++) printf("9");
				nueves = 0;
			}
		}
	}

	int a = Num_dec_before_thread;
	HANDLE hThread;
	if (i > a) {
		hThread = CreateThread(NULL, NULL, pi_dec, NULL, NULL, NULL);
		a = a + Num_dec_before_thread;
	}

	//printf("%i", digant);
	//scanf("%i");
	return NULL;
}
#include "echo.h"

int echo(char **args) {
	int i=1;
	while (args[i] != NULL) printf("%s ",args[i++]);
	printf("\n");
	return 1;
}
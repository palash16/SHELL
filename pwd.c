#include "pwd.h"

int pwd(void) {
	char *currdir = malloc(1000*sizeof(char));
	getcwd(currdir,1000);
	printf("%s\n",currdir);
	free(currdir);
	return 1;
}
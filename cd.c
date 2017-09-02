#include "cd.h"

char home[100];

void getHome() {
	getcwd(home,100);
}

int cd(char **args,char home[]) {
	//printf("%s\n",home);
	if (args[1] == NULL) chdir(home);
	else {
		if (strcmp(args[1],"~") == 0) { 
			if (chdir(home) != 0) perror("shell");
		}	
		else {
			if(chdir(args[1]) != 0) perror("shell");		
		}
	}
	return 1;
}	
#include "launchCmds.h"


int launchCmds(char **args) {
	int i = 1,bg = 0;
	while(args[i] != 0) {
		if (strcmp(args[i++],"&") == 0) bg = 1; 
	}
	int pid = fork(),wpid,status;
	if (pid == 0) {
		if (execvp(args[0],args) == -1) perror("shell");
		exit(EXIT_FAILURE);
	}
	else if (pid < 0) perror("shell");
	else { 
		if (bg == 0 ) {
			wpid = waitpid(pid,&status,WUNTRACED);
			while ((WIFEXITED(status)<=0) && (WIFSIGNALED(status)<=0)) {
				wpid = waitpid(pid,&status,WUNTRACED);
			}	
		}
	}
	return 1;
}
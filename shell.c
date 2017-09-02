#include "global.h"
#include "cd.h"
#include "echo.h"
#include "pwd.h"
#include "ls.h"
#include "pinfo.h"
#include "launchCmds.h"

char home[100];

// void getHome() {
// 	getcwd(home,100);
// }

char *readInput(void) {
	char *line;
	int bufsize = 1024;
	line = (char *)malloc(bufsize*sizeof(char));
	// error handling
	int c,pos=0;
	while(1) {
		c = getchar();
		if (c == EOF || c == '\n') {
			line[pos++] = '\0';
				//printf("%s%d\n",line,(int)strlen(line));
				return line;	
		}
		else line[pos++] = c;
		if (pos >= bufsize) {
			bufsize += bufsize;
			line = realloc(line,bufsize*sizeof(char));
			//error handling
		} 
	}
	free(line);
}

char **separateCmds(char *line) {
	int bufsize = 64,position = 0;
	char **tokens = malloc(bufsize*sizeof(char));
	//put error handling
	char *token;

	token = strtok(line,Delim);
	//printf("%s",token);
	while(token != NULL) {
		//printf("yay");
		tokens[position++] = token;
		if (position > bufsize) {
			bufsize += 64;
			tokens = (char**)realloc(tokens,bufsize*sizeof(char));
			//put error handling
		}
		token = strtok(NULL,Delim);
	}
	tokens[position] = NULL;
	//printf("%s",tokens[0]);
	return tokens;
	free(tokens);
}

void getPrompt() {
	char *username = getenv("USERNAME");
	char *hostname = malloc(20*sizeof(char));
	char *currdir = malloc(100*sizeof(char)); 
	int i;
	getcwd(currdir,100);
	int currlen = strlen(currdir);
	int homelen = strlen(home);
	currdir[currlen++] = 47; 
	gethostname(hostname,20);
	if (currlen >= homelen) {
		int flag = 0;
		for(i=0;i<homelen;i++) {
			if(currdir[i] != home[i]) {
				flag = 1;
			}
		}
		int j = 0;
		if (flag == 0) {
			currdir[j++] = '~';
			currdir[j++] = '/';
			for(i=homelen;i<currlen;i++) currdir[j++] = currdir[i];
			while(j<currlen) currdir[j++] = 0;
		}
	}	
	printf("<%s@%s:%s> ",username,hostname,currdir);
	free(hostname);
	free(currdir);
}

int executeCmds(char **args) {
	if (args[0] == NULL) return 1;
	//if (strcmp(args[0],"cd") == 0) return cd(args);
	else if (strcmp(args[0],"pwd") == 0) return pwd();
	else if (strcmp(args[0],"exit") == 0) exit(0);
	else if (strcmp(args[0],"echo") == 0) return echo(args);
	else if (strcmp(args[0],"cd") == 0) return cd(args,home);
	else if (strcmp(args[0],"pinfo")==0) return pinfo(args,home);
	else if (strcmp(args[0],"ls") == 0) return ls(args);
	else return launchCmds(args);
}

int main() {
	int status = 1;
	getHome();
		home[strlen(home)] = 47;
	do {
		//printf("\n");
		getPrompt();
		char *line = readInput();

		char *temp;
		//printf("1\n");
		while (line) {
			temp = strsep(&line,";"); 
			char **args = separateCmds(temp);
			//printf("2\n");
			int status = executeCmds(args);
			//printf("\n");
			if (status == 0 ) break; 
			
		}
	} while(status);
	
	return 0;
}

// #include <stdio.h>
// #include <sys/types.h>
// #include <pwd.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <sys/wait.h>
// #include <sys/stat.h>
// #include <string.h>
// #include <dirent.h>
// #include <time.h>
// #include <grp.h>

#include "global.h"
#include "cd.h"
#include "echo.h"
#include "pwd.h"
#include "ls.h"
#include "pinfo.h"
// #include "launchCmds.h"
#include "redirect.h"

// #define Delim " \t\n\r\a"  

char home[100];
int processid = 1;
pid_t fgid = 0;
char fgname[100];
typedef struct node {
	int proc_no;
	pid_t proc_id;
	char pname[1000];
	struct node * next;
} Node;

Node * head;
Node * end;
Node * behind,* ahead;

// void getHome() {
// 	getcwd(home,100);
// }

void getPrompt() {
	char *username = getenv("USER");
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

// void input_redirect(char **args) {
// 	int i =0,in;
// 	char filename[100];
// 	while(args[i] != NULL) {
// 		if (strcmp(args[i],'<') == 0) {
// 			strcpy(filename,args[i+1]);
// 			args[i] = NULL;
// 			args[i+1] = NULL:
// 		}
// 		i++;
// 	}
// 	in = open(filename,O_RDONLY, 0);
// 	dup2(in,STDIN_FILENO);
// 	close(in);
// }

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
	// int i = 0;
	// while(tokens[i] != NULL) {
	// 	if ((strcmp(tokens[i],60) == 0)) {//|| (strcmp(args[i],62) == 0)) {
	// 		input_redirect(tokens);
	// 	}
	// }
	//printf("%s",tokens[0]);
	return tokens;
	free(tokens);
}

void listProc(pid_t pid , char *arg) {
	Node * temp = (Node *)malloc(sizeof(Node));
	temp -> proc_no = processid++;
	temp -> proc_id = pid;
	strcpy(temp -> pname,arg);
	temp -> next = NULL;
	end -> next = temp;
	end = temp;
	return;
}

void sigStop() {
	if (fgid != 0) {
		kill(fgid,SIGTSTP);
		listProc(fgid,fgname);
		fgid = 0;
		strcpy(fgname,"\0");
		kill(fgid,SIGCONT);
		signal(SIGTSTP,sigStop);
	}
	fflush(stdout);
	return;
}

int deleteProcess(int proid) {
	//int proid = atoi(arg);
	int status;
	if (head->next != NULL) {
		behind = head->next;
		if (behind->next != NULL && behind -> proc_no != proid) {
			ahead = behind->next;
			while (ahead != NULL) {
				if (ahead -> proc_id == proid) {
					printf("Process %s  with pid [%d] exited with status %d \n", ahead->pname, ahead->proc_id,WEXITSTATUS(status));//
					if (ahead -> next == NULL) end = behind;
					behind -> next = ahead -> next;
					free(ahead);
					break;
				}
				else {
					behind = ahead;
					ahead = behind -> next;
				}
			}
		}
		else {
			if (behind -> proc_no == proid) {	
				printf("Process %s  with pid [%d] exited with status %d \n", behind->pname, behind->proc_id,WEXITSTATUS(status));
				head -> next = behind -> next;
				if (head->next == NULL) end = head;
				free(behind);
				return 1;
			}
		}
	}
	return 1; 
}

int launchCmds(char **args) {
	int i = 1,bg = 0;
	while(args[i] != 0) {
		if (strcmp(args[i],"&") == 0) {
			args[i] = NULL;
			bg = 1;
		}
		i++;	 
	}
	int pid = fork(),wpid,status;
	if (pid == 0) {
		if (execvp(args[0],args) == -1) perror("shell");
		exit(EXIT_FAILURE);
	}
	else if (pid < 0) perror("shell");
	else { 
		if (bg == 0 ) {
			fgid = pid;
			strcpy(fgname,args[0]);
			wpid = waitpid(pid,&status,WUNTRACED);
			while ((WIFEXITED(status)<=0) && (WIFSIGNALED(status)<=0)) {
				wpid = waitpid(pid,&status,WUNTRACED);
			}
		}	
		else {
			listProc(pid,args[0]);
		}		
	}
	return 1;
}

// int pwd(void) {
// 	char *currdir = malloc(1000*sizeof(char));
// 	getcwd(currdir,1000);
// 	printf("%s\n",currdir);
// 	free(currdir);
// }

// int echo(char **args) {
// 	int i=1;
// 	while (args[i] != NULL) printf("%s ",args[i++]);
// 	printf("\n");
// 	return 1;
// }

// int cd(char **args) {
// 	//printf("%s\n",home);
// 	if (args[1] == NULL) chdir(home);
// 	else {
// 		if (strcmp(args[1],"~") == 0) { 
// 			if (chdir(home) != 0) perror("shell");
// 		}	
// 		else {
// 			if(chdir(args[1]) != 0) perror("shell");		
// 		}
// 	}
// 	return 1;
// }	

// int pinfo(char **args,char *path) {
// 	char ProcessPath[1000];
// 	strcpy(ProcessPath,"/proc/");
// 	if(args[1]==NULL)
// 		strcat(ProcessPath,"self");
// 	else
// 		strcat(ProcessPath,args[1]);

// 	// strcat(ProcessPath,"/stat");
// 	char finalpath[100],execpath[1000],relativepath[1000];
// 	strcpy(finalpath,ProcessPath);
// 	strcat(finalpath,"/stat");


// 	//to print pid and process status
// 	int pid;
// 	char status,name[50];
// 	FILE *stat1=fopen(finalpath,"r");
// 	fscanf(stat1,"%d %s %c",&pid,name,&status);
// 	fprintf(stdout," pid: %d\n Process Status: %c\n",pid,status);
// 	fclose(stat1);


// 	//to print memory
// 	strcpy(finalpath,ProcessPath);
// 	strcat(finalpath,"/statm");

// 	int memsize;
// 	FILE *mem=fopen(finalpath,"r");
// 	fscanf(mem,"%d",&memsize);
// 	fprintf(stdout, "Memory: %d\n",memsize);
// 	fclose(mem);


// 	//to print executable path
// 	strcpy(finalpath,ProcessPath);
// 	strcat(finalpath,"/exe");
// 	readlink(finalpath,execpath,sizeof(execpath));

// 	int i=0,pathL=strlen(path);
// 	while(i<pathL) {
// 		if(path[i]!=execpath[i])
// 			break;
// 		i++;
// 	}

// 	if(i==pathL) {
// 		relativepath[0]='~';
// 		relativepath[1]='\0';
// 		strcat(relativepath,(const char *)&execpath[pathL]);
// 	}
// 	else {
// 		strcpy(relativepath,execpath);
// 		relativepath[strlen(execpath)]='\0';
// 	}
// 	int j=0;
// 	while(execpath[j])
// 		execpath[j++]='\0';

// 	fprintf(stdout,"Executable Path: %s\n",relativepath);
// 	return 1;
// }

// int ls(char **args) {
// 	struct dirent **dent;
// 	DIR* dir;
// 	struct stat mystat;
// 	int n,i = 1,l=0,a=0,pos = 0,j=0;
// 	char *buf;
// 	struct passwd *user;
// 	struct group *group;
// 	unsigned char mod[13];
// 	struct tm *strtime;
// 	time_t now;
// 	int year; 
// 	buf = malloc(1024*sizeof(char));
// 	while(args[i] != NULL) {
// 		j = 0;
// 		if (args[i][j] == 45) {
// 			j = 1;
// 			while(args[i][j] != 0) {
// 				if(args[i][j] == 108) l = 1;
// 				if(args[i][j] == 97)  a = 1;
// 				j++;
// 			}
// 		}
// 		else {
// 			if (i > 0) pos = i;
// 		}
// 		i++; 
// 	}
// 	if (pos == 0 || args[pos] == NULL) args[pos] = ".";
// 		//n = scandir(".",&dent,NULL,alphasort);		
// 	n = scandir(args[pos],&dent,NULL,alphasort);
// 	if (n < 0) perror("shell");
// 	else {
// 		for(i=0;i<n;i++) {
// 			if (l == 1) {
// 				if (a == 1) {
// 					sprintf(buf,"%s/%s",args[pos],dent[i]->d_name);
// 					stat(buf,&mystat);
// 					printf( (S_ISDIR(mystat.st_mode)) ? "d" : "-");
// 					printf( (mystat.st_mode & S_IRUSR) ? "r" : "-");
// 					printf( (mystat.st_mode & S_IWUSR) ? "w" : "-");
// 					printf( (mystat.st_mode & S_IXUSR) ? "x" : "-");
// 					printf( (mystat.st_mode & S_IRGRP) ? "r" : "-");
// 					printf( (mystat.st_mode & S_IWGRP) ? "w" : "-");
// 					printf( (mystat.st_mode & S_IXGRP) ? "x" : "-");
// 					printf( (mystat.st_mode & S_IROTH) ? "r" : "-");
// 					printf( (mystat.st_mode & S_IWOTH) ? "w" : "-");
// 					printf( (mystat.st_mode & S_IXOTH) ? "x" : "-");
// 					printf(" \t%d",(int)mystat.st_nlink);
// 					user = getpwuid(mystat.st_uid);
// 						printf(" \t%s", user->pw_name);
// 					group = getgrgid(mystat.st_gid);
// 						printf(" \t%s", group->gr_name);
// 					printf(" \t%lld",(long long)mystat.st_size);
// 					time(&now);
// 					year = localtime(&now)->tm_year;
// 					strtime = localtime(&mystat.st_ctime);
// 					if(strtime->tm_year == year)
// 						strftime(mod,13,"%b %e %R",strtime);
// 					else
// 						strftime(mod,13,"%b %e %Y",strtime);
// 					printf(" \t%s",mod );	 	
// 					printf(" \t%s\n",dent[i]->d_name);
// 				}
// 				else {
// 					if ((dent[i] ->d_name)[0] != 46) {
// 						sprintf(buf,"%s/%s",args[pos],dent[i]->d_name);
// 						stat(buf,&mystat);
// 						printf( (S_ISDIR(mystat.st_mode)) ? "d" : "-");
// 						printf( (mystat.st_mode & S_IRUSR) ? "r" : "-");
// 						printf( (mystat.st_mode & S_IWUSR) ? "w" : "-");
// 						printf( (mystat.st_mode & S_IXUSR) ? "x" : "-");
// 						printf( (mystat.st_mode & S_IRGRP) ? "r" : "-");
// 						printf( (mystat.st_mode & S_IWGRP) ? "w" : "-");
// 						printf( (mystat.st_mode & S_IXGRP) ? "x" : "-");
// 						printf( (mystat.st_mode & S_IROTH) ? "r" : "-");
// 						printf( (mystat.st_mode & S_IWOTH) ? "w" : "-");
// 						printf( (mystat.st_mode & S_IXOTH) ? "x" : "-");
// 						printf(" \t%d",(int)mystat.st_nlink);
// 						user = getpwuid(mystat.st_uid);
// 						printf(" \t%s", user->pw_name);
// 						group=getgrgid(mystat.st_gid);
// 						printf(" \t%s", group->gr_name);
// 						printf(" \t%lld",(long long)mystat.st_size);
// 						time(&now);
// 						year = localtime(&now)->tm_year;
// 						strtime = localtime(&mystat.st_ctime);
// 						if(strtime->tm_year == year)
// 							strftime(mod,13,"%b %e %R",strtime);
// 						else
// 							strftime(mod,13,"%b %e %Y",strtime);
// 						printf(" \t%s",mod );
// 						//printf(" \t%s",ctime(&mystat.st_mtime));
// 						printf(" \t%ld",mystat.st_mtime);
// 						printf(" \t%s\n",dent[i]->d_name);
// 					}	 
// 				}
// 			}	
// 			else {
// 				if (a == 1) {
// 					printf(" %s\n",dent[i]->d_name);	
// 				}
// 				else {
// 					if ((dent[i] ->d_name)[0] != 46) printf(" %s\n",dent[i]->d_name);
// 				}
// 			}	
// 			free(dent[i]);
			
// 		}
// 		free(dent);
// 	}	
// 	return 1; 

// }

int MySetenv (char **args) {
	if (args[2] == NULL) args[2] = " ";
	if (setenv(args[1],args[2],1) != 0) perror("shell");
	return 1;	
}

int MyUnsetenv (char **args) {
	if (unsetenv(args[1]) != 0) perror("shell");
	return 1;
}

int displayProcesses() {
	Node * temp = head -> next;
	char finalpath[100];
	int tmp1;
	char tmp2[100];
	char status[10];
	char proid[10];
	while(temp != NULL) {
		sprintf(proid,"%d",temp->proc_id);
		strcpy(finalpath,"/proc/");
		strcat(finalpath,proid);
		strcat(finalpath,"/stat");
		//printf("%s\n",finalpath);
		FILE *filename = fopen(finalpath,"r");
		//printf("hello\n");
		if (filename == NULL) strcpy(status,"Done");
		else fscanf(filename,"%d %s %s",&tmp1,tmp2,status); 
		printf("[%d] \t %s \t %s[%d]\n",temp->proc_no,status,temp->pname,temp->proc_id);
		temp = temp->next;
		strcpy(finalpath,"");
	} 
	return 1;
}

int SignalProcesses(char **args) {
	Node * temp = head->next;
	int sig = atoi(args[2]);
	int pno = atoi(args[1]);
	while (temp != NULL) {
		if (temp -> proc_no == pno) {
			if(kill(temp -> proc_id,sig) != 0) perror("shell");
			break;
		} 
		temp = temp->next;
	}
	return 1;
}

int fg(char **args) {
	Node * temp = head->next;
	int id = atoi(args[1]);
	int status;
	while (temp != NULL) {
		if (temp -> proc_no == id) {
			waitpid(temp->proc_id,&status,WCONTINUED);
			fgid = temp->proc_id;
			strcpy(fgname,temp->pname);
			deleteProcess(id);
			break;
		}
		temp = temp->next;
	}
	return 1;
}

void sigintHandler(int sig_num) {
	signal(SIGINT,sigintHandler);
	fflush(stdout);
}

int killBgs() {
	Node * temp = head->next;
	while(temp != NULL) {
		if(kill(temp -> proc_id,9) != 0) perror("shell");
		temp = temp -> next;
	}
	return 1;
}

// void redirect(char **args) {
void pipeHandler(char * args[]){
	// File descriptors
	int filedes[2]; // pos. 0 output, pos. 1 input of the pipe
	int filedes2[2];
	
	int num_cmds = 0;
	
	char *command[256];
	
	pid_t pid;
	
	int err = -1;
	int end = 0;
	
	// Variables used for the different loops
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	
	// First we calculate the number of commands (they are separated
	// by '|')
	while (args[l] != NULL){
		if (strcmp(args[l],"|") == 0){
			num_cmds++;
		}
		l++;
	}
	num_cmds++;
	
	// Main loop of this method. For each command between '|', the
	// pipes will be configured and standard input and/or output will
	// be replaced. Then it will be executed
	while (args[j] != NULL && end != 1){
		k = 0;
		// We use an auxiliary array of pointers to store the command
		// that will be executed on each iteration
		while (strcmp(args[j],"|") != 0){
			command[k] = args[j];
			j++;	
			if (args[j] == NULL){
				// 'end' variable used to keep the program from entering
				// again in the loop when no more arguments are found
				end = 1;
				k++;
				break;
			}
			k++;
		}
		// Last position of the command will be NULL to indicate that
		// it is its end when we pass it to the exec function
		command[k] = NULL;
		j++;		
		
		// Depending on whether we are in an iteration or another, we
		// will set different descriptors for the pipes inputs and
		// output. This way, a pipe will be shared between each two
		// iterations, enabling us to connect the inputs and outputs of
		// the two different commands.
		if (i % 2 != 0){
			pipe(filedes); // for odd i
		}else{
			pipe(filedes2); // for even i
		}
		
		pid=fork();
		
		if(pid==-1){			
			if (i != num_cmds - 1){
				if (i % 2 != 0){
					close(filedes[1]); // for odd i
				}else{
					close(filedes2[1]); // for even i
				} 
			}			
			printf("Child process could not be created\n");
			return;
		}
		if(pid==0){
			// If we are in the first command
			if (i == 0){
				dup2(filedes2[1], STDOUT_FILENO);
			}
			// If we are in the last command, depending on whether it
			// is placed in an odd or even position, we will replace
			// the standard input for one pipe or another. The standard
			// output will be untouched because we want to see the 
			// output in the terminal
			else if (i == num_cmds - 1){
				if (num_cmds % 2 != 0){ // for odd number of commands
					dup2(filedes[0],STDIN_FILENO);
				}else{ // for even number of commands
					dup2(filedes2[0],STDIN_FILENO);
				}
			// If we are in a command that is in the middle, we will
			// have to use two pipes, one for input and another for
			// output. The position is also important in order to choose
			// which file descriptor corresponds to each input/output
			}else{ // for odd i
				if (i % 2 != 0){
					dup2(filedes2[0],STDIN_FILENO); 
					dup2(filedes[1],STDOUT_FILENO);
				}else{ // for even i
					dup2(filedes[0],STDIN_FILENO); 
					dup2(filedes2[1],STDOUT_FILENO);					
				} 
			}
			
			if (execvp(command[0],command)==err){
				kill(getpid(),SIGTERM);
			}		
		}
				
		// CLOSING DESCRIPTORS ON PARENT
		if (i == 0){
			close(filedes2[1]);
		}
		else if (i == num_cmds - 1){
			if (num_cmds % 2 != 0){					
				close(filedes[0]);
			}else{					
				close(filedes2[0]);
			}
		}else{
			if (i % 2 != 0){					
				close(filedes2[0]);
				close(filedes[1]);
			}else{					
				close(filedes[0]);
				close(filedes2[1]);
			}
		}
				
		waitpid(pid,NULL,0);
				
		i++;	
	}
}


int executeCmds(char **args) {
	int i=0;
		while(args[i]!=NULL)
		{
			if(strcmp(args[i],"|") == 0)
			{
				pipeHandler(args);
				return;
			}
			i++;
		}
	if (args[0] == NULL) return 1;
	else if (strcmp(args[0],"pwd") == 0) return pwd();
	else if (strcmp(args[0],"exit") == 0 || strcmp(args[0],"quit") == 0) exit(0);
	else if (strcmp(args[0],"echo") == 0) return echo(args);
	else if (strcmp(args[0],"cd") == 0) return cd(args,home);
	else if (strcmp(args[0],"pinfo")==0) return pinfo(args,home);
	else if (strcmp(args[0],"ls") == 0) return ls(args);
	else if (strcmp(args[0],"setenv") == 0) return MySetenv(args);
	else if (strcmp(args[0],"unsetenv") == 0) return MyUnsetenv(args);
	else if (strcmp(args[0],"jobs") == 0) return displayProcesses();
	else if (strcmp(args[0],"kjobs") == 0) return SignalProcesses(args);
	else if (strcmp(args[0],"fg") == 0) return fg(args);
	else if (strcmp(args[0],"overkill") == 0) return killBgs();
	else return launchCmds(args);
}

int main()
{
	int i,status = 1,status1;
	getHome();
	home[strlen(home)] = 47;
	head = (Node *)malloc(sizeof(Node));
	head -> next = NULL;
	end = head;
	signal(SIGINT,sigintHandler);
	signal(SIGTSTP,sigStop);	
	do 
	{
		//rintf("\n");
		Node * tmp = head->next;
		Node * tmp1;
		while(tmp != NULL)
		{
			tmp1 = tmp->next;
			if (waitpid(tmp->proc_id,&status1,WNOHANG) !=0 ) {
				deleteProcess(tmp->proc_no);
			}
			tmp = tmp1;
		}
		getPrompt();
		char *line = readInput();

		char *temp;
		//printf("1\n");
		while (line)
		{
			// temp = strsep(&line,";"); 
			// char **args = separateCmds(temp);
			// //printf("2\n");
			// int status = executeCmds(args);
			// //printf("\n");
			// if (status == 0 ) break; 
			temp = strsep(&line,";"); 
			char **args = separateCmds(temp);
			//printf("2\n");
			
			int i=0,redir_flag=0,flag=0;
			// while(args[i]!=NULL)
			// {
			// 	printf("%s\n",args[i]);
			// 	i++;
			// }

			int num =0;
			while(args[num]!= NULL)
			{
				num++;
			}
			// printf("%d\n",num);
			while(args[i]!= NULL)
			{
				// printf("%s\n", args[i]);
				if(args[i][0] == 60 || args[i][0] ==62)
				{
					// printf("1\n");
					flag=1;
					redir_flag = 1;
					break;
				}
				i++;
			}

			if (redir_flag==1)
			{
				// printf("1\n");
				redirect(args,num);
			}
			if(flag==0)
			{
				int status = executeCmds(args);
				//printf("\n");
				if (status == 0 ) break;
			}								
		}		
	} while(status);
	
	return 0;
}

#include "pinfo.h"

int pinfo(char **args,char *path) {
	char ProcessPath[1000];
	strcpy(ProcessPath,"/proc/");
	if(args[1]==NULL)
		strcat(ProcessPath,"self");
	else
		strcat(ProcessPath,args[1]);

	// strcat(ProcessPath,"/stat");
	char finalpath[100],execpath[1000],relativepath[1000];
	strcpy(finalpath,ProcessPath);
	strcat(finalpath,"/stat");


	//to print pid and process status
	int pid;
	char status,name[50];
	FILE *stat1=fopen(finalpath,"r");
	fscanf(stat1,"%d %s %c",&pid,name,&status);
	fprintf(stdout," pid: %d\n Process Status: %c\n",pid,status);
	fclose(stat1);


	//to print memory
	strcpy(finalpath,ProcessPath);
	strcat(finalpath,"/statm");

	int memsize;
	FILE *mem=fopen(finalpath,"r");
	fscanf(mem,"%d",&memsize);
	fprintf(stdout, "Memory: %d\n",memsize);
	fclose(mem);


	//to print executable path
	strcpy(finalpath,ProcessPath);
	strcat(finalpath,"/exe");
	readlink(finalpath,execpath,sizeof(execpath));

	int i=0,pathL=strlen(path);
	while(i<pathL) {
		if(path[i]!=execpath[i])
			break;
		i++;
	}

	if(i==pathL) {
		relativepath[0]='~';
		relativepath[1]='\0';
		strcat(relativepath,(const char *)&execpath[pathL]);
	}
	else {
		strcpy(relativepath,execpath);
		relativepath[strlen(execpath)]='\0';
	}
	int j=0;
	while(execpath[j])
		execpath[j++]='\0';

	fprintf(stdout,"Executable Path: %s\n",relativepath);
	return 1;
}
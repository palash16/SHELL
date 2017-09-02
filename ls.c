#include "ls.h"

int ls(char **args) {
	struct dirent **dent;
	// DIR* dir;
	struct stat mystat;
	int n,i = 1,l=0,a=0,pos = 0,j=0;
	char *buf;
	struct passwd *user;
	struct group *group;
	unsigned char mod[13];
	struct tm *strtime;
	time_t now;
	int year; 
	buf = malloc(1024*sizeof(char));
	while(args[i] != NULL) {
		j = 0;
		if (args[i][j] == 45) {
			j = 1;
			while(args[i][j] != 0) {
				if(args[i][j] == 108) l = 1;
				if(args[i][j] == 97)  a = 1;
				j++;
			}
		}
		else {
			if (i > 0) pos = i;
		}
		i++; 
	}
	if (pos == 0 || args[pos] == NULL) args[pos] = ".";
		//n = scandir(".",&dent,NULL,alphasort);		
	n = scandir(args[pos],&dent,NULL,alphasort);
	if (n < 0) perror("shell");
	else {
		for(i=0;i<n;i++) {
			if (l == 1) {
				if (a == 1) {
					sprintf(buf,"%s/%s",args[pos],dent[i]->d_name);
					stat(buf,&mystat);
					printf( (S_ISDIR(mystat.st_mode)) ? "d" : "-");
					printf( (mystat.st_mode & S_IRUSR) ? "r" : "-");
					printf( (mystat.st_mode & S_IWUSR) ? "w" : "-");
					printf( (mystat.st_mode & S_IXUSR) ? "x" : "-");
					printf( (mystat.st_mode & S_IRGRP) ? "r" : "-");
					printf( (mystat.st_mode & S_IWGRP) ? "w" : "-");
					printf( (mystat.st_mode & S_IXGRP) ? "x" : "-");
					printf( (mystat.st_mode & S_IROTH) ? "r" : "-");
					printf( (mystat.st_mode & S_IWOTH) ? "w" : "-");
					printf( (mystat.st_mode & S_IXOTH) ? "x" : "-");
					printf(" \t%d",(int)mystat.st_nlink);
					user = getpwuid(mystat.st_uid);
						printf(" \t%s", user->pw_name);
					group = getgrgid(mystat.st_gid);
						printf(" \t%s", group->gr_name);
					printf(" \t%lld",(long long)mystat.st_size);
					time(&now);
					year = localtime(&now)->tm_year;
					strtime = localtime(&mystat.st_ctime);
					if(strtime->tm_year == year)
						strftime(mod,13,"%b %e %R",strtime);
					else
						strftime(mod,13,"%b %e %Y",strtime);
					printf(" \t%s",mod );	 	
					printf(" \t%s\n",dent[i]->d_name);
				}
				else {
					if ((dent[i] ->d_name)[0] != 46) {
						sprintf(buf,"%s/%s",args[pos],dent[i]->d_name);
						stat(buf,&mystat);
						printf( (S_ISDIR(mystat.st_mode)) ? "d" : "-");
						printf( (mystat.st_mode & S_IRUSR) ? "r" : "-");
						printf( (mystat.st_mode & S_IWUSR) ? "w" : "-");
						printf( (mystat.st_mode & S_IXUSR) ? "x" : "-");
						printf( (mystat.st_mode & S_IRGRP) ? "r" : "-");
						printf( (mystat.st_mode & S_IWGRP) ? "w" : "-");
						printf( (mystat.st_mode & S_IXGRP) ? "x" : "-");
						printf( (mystat.st_mode & S_IROTH) ? "r" : "-");
						printf( (mystat.st_mode & S_IWOTH) ? "w" : "-");
						printf( (mystat.st_mode & S_IXOTH) ? "x" : "-");
						printf(" \t%d",(int)mystat.st_nlink);
						user = getpwuid(mystat.st_uid);
						printf(" \t%s", user->pw_name);
						group=getgrgid(mystat.st_gid);
						printf(" \t%s", group->gr_name);
						printf(" \t%lld",(long long)mystat.st_size);
						time(&now);
						year = localtime(&now)->tm_year;
						strtime = localtime(&mystat.st_ctime);
						if(strtime->tm_year == year)
							strftime(mod,13,"%b %e %R",strtime);
						else
							strftime(mod,13,"%b %e %Y",strtime);
						printf(" \t%s",mod );
						//printf(" \t%s",ctime(&mystat.st_mtime));
						printf(" \t%ld",mystat.st_mtime);
						printf(" \t%s\n",dent[i]->d_name);
					}	 
				}
			}	
			else {
				if (a == 1) {
					printf(" %s\n",dent[i]->d_name);	
				}
				else {
					if ((dent[i] ->d_name)[0] != 46) printf(" %s\n",dent[i]->d_name);
				}
			}	
			free(dent[i]);
			
		}
		free(dent);
	}	
	return 1; 

}
#include "redirect.h"

int redirect(char **args, int num)
{
	char in[100],out[100],app[100];
	int r = 0,pid,flag1=0,flag2=0,flag3=0;
	// args[num-1][strlen(args[num-1])-1] = 0;
	args[num] = NULL;

	pid = fork();
	if (pid==0)
	{	
		for (int i=0;i<num;i++)
		{
			if (strcmp(args[i],"<")==0)
			{
				args[i] = NULL;
				strcpy(in,args[i+1]);
				flag1 = 1;
			}
			
			if (args[i]!=NULL)
			{
				if (strcmp(args[i],">")==0)
				{
					args[i] = NULL;
					strcpy(out,args[i+1]);
					flag2 = 1;
				}
			}

			if (args[i]!=NULL)
			{
				if (args[i][1]==62)
				{
					args[i] = NULL;
					strcpy(app,args[i+1]);
					flag3 = 1;
				}
			}
			// printf("%d %d\n", flag2,flag3);	
		}
		// printf("%d %d\n", flag1,flag2);
		if (flag1)
		{
			int fd;
			fd = open(in,O_RDONLY,0);
			if (fd<0)
			{perror("Could not open input file"); exit(0);}

			dup2(fd,0);
			close(fd);
		}	

		if (flag2)
		{
			struct stat buf;
			if (stat(out,&buf)==0)
			{
				int fo;	
				fo = open(out, O_WRONLY);
				if (fo<0)
				{perror("Could not open output file"); exit(0);}
				dup2(fo,1);
				close(fo);
			}
			else
			{
				int fo;
				fo = creat(out,0644);
				if (fo<0)
				{perror("Could not create output file"); exit(0);}
				dup2(fo,1);
				close(fo);
			}
		}

		if (flag3)
		{
			struct stat buffer;
			if (stat(app,&buffer)==0)
			{
				int fa;
				fa = open(app,O_APPEND | O_WRONLY);
				if (fa<0)
				{perror("Could not open output file"); exit(0);}
				dup2(fa,1);
				close(fa);
			}
			else
			{
				int fa;
				fa = creat(app,0644);
				if (fa<0)
				{perror("Could not create output file"); exit(0);}
				dup2(fa,1);
				close(fa);
			}
		}		
		if (execvp(args[0],args)<0)	
		{r=1; printf("%s: Command doesn't exist\n", args[0]);}
	}
	else
	{wait(NULL);}
	if (r!=1)
	{printf("%s with process id: %d exited normally\n",args[0],pid);}					
	return 1;
}

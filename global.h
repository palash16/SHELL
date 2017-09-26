#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <grp.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <errno.h>

#define Delim " \t\n\r\a"

// typedef struct node {
// 	int proc_no;
// 	pid_t proc_id;
// 	char pname[1000];
// 	struct node * next;
// } Node;
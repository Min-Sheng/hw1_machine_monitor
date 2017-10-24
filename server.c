#include "server.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
//void *communicate_to_client();
void a_all_pids();
void b_tids(int pid);
void c_child_pids(int pid);
void d_process_name(int pid);
void e_state(int pid);
void f_cmdline(int pid);
void g_parent_pid(int pid);
void h_all_ancients_of_pid(int pid);
void i_Vmsize(int pid);
void j_VmRSS(int pid);
void k_exit();

int main(int argc, char **argv)
{
// write someting here...
	a_all_pids();
	return 0;
}

void a_all_pids()
{
	DIR           *d;
	struct dirent *dir;
	d = opendir("/proc");
	int isPID = 0;
	int i = 0;
	char name[256];
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			strcpy(name, dir->d_name);
			while (i<strlen(name)) {
				if(name[i] > 57 || name[i] < 48) {
					isPID = 0;
					break;
				} else
					isPID = 1;
				i++;
			}
			if (isPID == 1) {
				printf("%s\n", name);
			}
		}
		closedir(d);
	}
}



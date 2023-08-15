#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "include/ua.h"

int passwd_len = 3;
char fname[64];

int passWord(char *passwd)
{
    GoString pwd = {passwd,strlen(passwd)};
    GoString sfname = {fname,strlen(fname)};
	return IsPasswdCorrest(pwd,sfname);
}

int main(int argc, char** argv)
{
    fclose(stderr);
	if(argc != 2) {
		puts("Need Archive file name");
		return 1;
	}
	strcpy(fname,argv[1]);
	printf("Cracking %s\n",fname);
	int alpha_count = 10;
	char alphabet[] = {'0','1','2','3','4','5','6','7','8','9'};
	int *ipasswd = malloc(passwd_len*sizeof(int));
	char *passwd = malloc((passwd_len+1)*sizeof(char));
	int i;
	int j;
	int update_prog = 0;
	for(i = 0;i < passwd_len;i++) {
		 ipasswd[i] = 0;
	}
	for(j = 0;j < passwd_len;j++) {
		passwd[j] = alphabet[ipasswd[j]];
	}
	printf("Begin cracking at \"%s\"\n",passwd);
	printf("Process 00%%");
	while(1) {
		i = 0;
		while(1) {
			if(ipasswd[i] < alpha_count-1) {
				ipasswd[i]++;
				if(i == passwd_len-2)
					update_prog = 1;
				break;
			} else {
				ipasswd[i] = 0;
				i++;
			}
		}
		for(j = 0;j < passwd_len;j++) {
			passwd[j] = alphabet[ipasswd[j]];
		}
		if(update_prog){
			printf("\b\b\b%d%d%%",ipasswd[passwd_len-1],ipasswd[passwd_len-2]);
			update_prog = 0;
		}
		//printf("Trying %s...",passwd);
		if(passWord(passwd)) {
			printf("\nGet passwd %s\n",passwd);
			break;
		}
	}
	free(ipasswd);
	free(passwd);
	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "include/tinycthread.h"
#include "include/UnArchive.h"
#include <unistd.h>

int thread_count = 1;
int passwd_len = 3;
char fname[64];
int alpha_count = 10;
char alphabet[] = {'0','1','2','3','4','5','6','7','8','9'};
enum {
    INCORRECT_PASSWD = 0,
    CORRECT_PASSWD,
    INVALID_FILE,
};

typedef struct Arg{
    int t_id;
    int *start_ipasswd;
    int passwd_count;
    int process;
    char *result;
} Arg;

void IPasswd2StrPasswd(const int *ipasswd, char *passwd)
{
    int j;
    for(j = 0;j < passwd_len;j++) {
        passwd[j] = alphabet[ipasswd[j]];
    }
    passwd[j] = (char)0;
}

void Int2IPasswd(int i,int *ipasswd) {
    while (1) {
        i--;
        if (i < 0)
            return;
        int j = 0;
        while (1) {
            if (ipasswd[j] < alpha_count - 1) {
                ipasswd[j]++;
                break;
            } else {
                ipasswd[j] = 0;
                j++;
            }
        }
    }
}


int passWord(char *filename,char *passwd)
{
    if(strcmp(&filename[strlen(filename)-3],"zip") == 0) {
        GoString pwd = {passwd,strlen(passwd)};
        GoString sfname = {fname,strlen(fname)};
        return IsZipPasswdCorrect(pwd,sfname);
    } else if (strcmp(&filename[strlen(filename)-3],"rar") == 0) {
        GoString pwd = {passwd,strlen(passwd)};
        GoString sfname = {fname,strlen(fname)};
        return IsRarPasswdCorrect(pwd,sfname);
    } else {
        return INVALID_FILE;
    }
}

int TestPwRange(Arg *arg)
{
    int i;
    int update_prog = 0;
    int tested_count = 0;
    char *passwd;
    do {
        passwd = malloc((passwd_len+1)*sizeof(char));
    }while (passwd == NULL);
    IPasswd2StrPasswd(arg->start_ipasswd,passwd);
    printf("[Id %d]Begin cracking %s at \"%s\" %d passwds to go. \n",arg->t_id,fname,passwd,arg->passwd_count);
    while(1) {
        i = 0;
        while(1) {
            if(arg->start_ipasswd[i] < alpha_count-1) {
                arg->start_ipasswd[i]++;
                if(i == passwd_len-3 || passwd_len < 4)
                    update_prog = 1;
                break;
            } else {
                arg->start_ipasswd[i] = 0;
                i++;
                if(i>passwd_len) {
                    arg->process = 1000;
                    free(passwd);
                    free(arg->start_ipasswd);
                    return 1;
                }
            }
        }
        tested_count++;
        if(tested_count == arg->passwd_count) {
            arg->process = 1000;
            free(passwd);
            free(arg->start_ipasswd);
            return 1;
        }
        IPasswd2StrPasswd(arg->start_ipasswd, passwd);
        if(update_prog){
            (arg->process) = (float)tested_count*1000/arg->passwd_count;
            //printf("%d\n",tested_count);
            //printf("\b\b\b%d%d%%",arg->start_ipasswd[passwd_len-1],arg->start_ipasswd[passwd_len-2]);
            update_prog = 0;
        }

        if(passWord(fname,passwd) == CORRECT_PASSWD) {
            printf("\n[Id %d]Get passwd %s\n",arg->t_id,passwd);
            arg->result = passwd;
            arg->process = -1;
            free(arg->start_ipasswd);
            return 0;
        }
    }
}

void SleepMs(long milliseconds)
{
    long seconds = milliseconds / 1000;
    long nanoseconds = (milliseconds % 1000) * 1000000L;
    thrd_sleep(&(struct timespec){.tv_sec=seconds,.tv_nsec=nanoseconds},NULL);
}

int DoCrack()
{
    int result = 0;
    int i,passwd_count_per_thread = pow(10,passwd_len)/thread_count+1;


    Arg args[thread_count];
    for (i = 0; i < thread_count; ++i) {
        int *ip;
        do {
            ip = malloc((passwd_len+2) * sizeof(int));
        }while (ip == NULL);
        for (int j = 0; j < passwd_len; ++j) {
            ip[j] = 0;
        }
        Int2IPasswd(passwd_count_per_thread*i,ip);
        args[i].process = 0;
        args[i].passwd_count = passwd_count_per_thread;
        args[i].start_ipasswd = ip;
        args[i].t_id = i;
    }

    thrd_t t[thread_count];

    for (i = 0; i < thread_count; ++i) {
        thrd_create(&t[i],&TestPwRange,&(args[i]));
    }

    SleepMs(1000);

    while (1) {
        int end = 1;
        printf("\r");
        for (i = 0; i < thread_count; ++i) {
            printf("[Id %d]%d/1000  ",i,args[i].process);
        }
        fflush(stdout);

        for (i = 0; i < thread_count; ++i) {
            if(args[i].process>=0 && args[i].process<1000)
                end = 0;
            if (args[i].process<0) {
                end = 1;
                printf("\n\nTHe Passwd is %s\n",args[i].result);
                free(args[i].result);
                result = 1;
                break;
            }

        }
        if (end)
            break;
        SleepMs(100);
    }
    return result;
}

int main(int argc, char** argv)
{
    if(argc != 3) {
        while (1) {
            printf("[CrackArchive]Input archive filename:");
            fflush(stdout);
            gets(fname);
            if (!access(fname, 0)) {
                printf("[CrackArchive]The file %s is invalid.\n",fname);
            } else {
                break;
            }
        }
        char count[16];
        printf("[CrackArchive]Input thread count:");
        fflush(stdout);
        gets(count);
        thread_count = atoi(count);
    } else {
        puts("[CrackArchive]Usage: %s filename thread_count");
        strcpy(fname,argv[1]);
        thread_count = atoi(argv[2]);
    }

    printf("[CrackArchive]Cracking %s\n",fname);
    printf("[CrackArchive]Using %d threads\n",thread_count);

    for (int i = 1; i <= 6; ++i) {
        passwd_len = i;
        printf("\n\n[CrackArchive]Setting passwd_len as %d\n",i);
        if(DoCrack()) {
            printf("\n[CrackArchive]passwd_len %d done.\n",i);
            break;
        } else {
            printf("\n[CrackArchive]passwd_len %d done with no result.\n",i);
        }
    }
    printf("Enter anything to exit");
    fflush(stdout);
    getchar();
    return 0;
}


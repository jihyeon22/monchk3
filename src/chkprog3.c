#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>

#include <getopt.h>
#include <dirent.h>
#include "board_system.h"

// ========================================================
//  program argument
// ========================================================
struct option   long_opt[] =
{
    {"help",      no_argument,        NULL,   'h'},
    {"chkproc",   required_argument,  NULL,   'c'},
    {"trycnt",    required_argument,  NULL,   't'},
    {NULL,        0,                  NULL,   0  }
};

const char  * short_opt = "hc:";

#define MAIN_RUN_INTERVAL_MIN   5
#define CROND_RUN_INTERVAL_MIN  10

#define DEFAULT_CHK_TRY_CNT     4
// =========================================================
// program option 
// =========================================================
typedef struct prg_opt_struct
{
    char    chk_proc_name[64];
    int     chk_try_cnt;
}PRG_OPT_T;

PRG_OPT_T prog_opt;

int main(int argc, char* argv[])
{
    int ret = 0;

    int c = 0;

    int check_mon_proc_name = 0;    
    // get argument
    strcpy(prog_opt.chk_proc_name, "null");
    prog_opt.chk_try_cnt =  DEFAULT_CHK_TRY_CNT;
   
    while((c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1)
    {
        switch(c)
        {
            case -1:
            case 0:
            {
                break;
            }
            case 'c': // check process 
            {
                strcpy(prog_opt.chk_proc_name, optarg);
                break;
            }
            case 't': // check try cnt 
            {
                prog_opt.chk_try_cnt = atoi(optarg);
                if ( prog_opt.chk_try_cnt <= 0 )
                    prog_opt.chk_try_cnt = DEFAULT_CHK_TRY_CNT;
                break;
            }
            case 'h':
            {
                printf("Usage: %s [OPTIONS]\n", argv[0]);
                printf("  -c, check process name\n");
                printf("  -t, check try cnt\n");
                printf("  -h, --help   print this help and exit\n");
                printf("\n");
                return (0);
            }
            case ':':
            case '?':
            {
                fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
                return (-2);
            }
            default:
            {
                fprintf(stderr, "%s: invalid option -- %c\n", argv[0], c);
                fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
                return (-2);
            }
        }
    }

    printf("   - opt : chk program : %s\r\n", prog_opt.chk_proc_name);
    printf("   - opt : chk try cnt : %d\r\n", prog_opt.chk_try_cnt);
    
    if (strcmp(prog_opt.chk_proc_name, "null") == 0 )
    {
        printf("process %s : invalid!! \r\n", prog_opt.chk_proc_name);
        return 0;
    }
        
    // check process ...
    while(prog_opt.chk_try_cnt--)
    {
        int sleep_sec = 0;
        int proc_cnt = 0;
        if ( (proc_cnt = mds_api_proc_find(prog_opt.chk_proc_name)) > 0) 
        {
            printf("process %s : found!!! => cnt [%d]\r\n", prog_opt.chk_proc_name, proc_cnt);
            ret = 1;
            break;
        }
        else
        {
            char cmd_buff[512] = {0,};
            if ( strcmp (prog_opt.chk_proc_name, "mon") == 0)
                sprintf(cmd_buff, "%s -restart &", prog_opt.chk_proc_name);
            else
                sprintf(cmd_buff, "%s &", prog_opt.chk_proc_name);

            printf("process %s : not found!!! try process run \r\n", prog_opt.chk_proc_name);
            //system_fork(prog_opt.chk_proc_name, 0, NULL);
            system(cmd_buff);
            ret = 0;
        }
        // random sleep... for crash. 
        sleep_sec = (rand()%4 +1);
        sleep(sleep_sec);
    }
    
    if ( ret == 0 )
    {
        mds_api_poweroff_and_log("chkprog3", "cannot run prog");
    }
    
    return 0;
}

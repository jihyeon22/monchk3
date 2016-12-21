#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#include <getopt.h>
#include <dirent.h>

#include "board_system.h"

#include <mdsapi/mds_api.h>

int set_crond_script()
{
	char write_buff[128];
	char read_buff[128];
	FILE * sh ;

	char crond_rules_file[512] = {0,};

	int cnt = 0;
	int ret = 0;

	sprintf(crond_rules_file, "%s%s", CROND_WORKSPACE, "root");

	memset(write_buff, 0x00, 128);
	memset(read_buff, 0x00, 128);

	// 1. remove script file...
	remove(crond_rules_file);

	cnt += sprintf(write_buff + cnt, "*/%d * * * * %s %s\n", CROND_RUN_INTERVAL_MIN1, CROND_RUN_PROG_NAME1, CROND_RUN_PROG_ARG1);
	cnt += sprintf(write_buff + cnt, "*/%d * * * * %s %s\n", CROND_RUN_INTERVAL_MIN2, CROND_RUN_PROG_NAME2, CROND_RUN_PROG_ARG2);
    
	if ( mds_api_write_data(crond_rules_file, write_buff, cnt, 0) != 0 )
		return -1;

	// sleep...
	sleep(1);

	// check write result

	if ( cnt != mds_api_read_data(crond_rules_file, read_buff, 128) )
	{
		printf("read file size is invalid!!!\r\n");
		return -1;
	}
	else
	{
		if ( strcmp(write_buff, read_buff) == 0 )
		{
			printf("write success!!!!! \r\n");
			return 0;
		}
		else
		{
			printf("read buffer is incollect!!\r\n");
			return -1;
		}
	}

}


void run_crond()
{
	int f_tmp = 10;
	int ret;
	char* crond_argv[]={SYSTEN_CRON_APP_PATH,"-c",SYSTEN_CRON_TMP, "-S",NULL};

	do {
		ret = mds_api_system_fork(SYSTEN_CRON_APP_PATH, 0, crond_argv);
		sleep(1);
	} 
	while((ret == -1) && (f_tmp--));
	
    // reset?
	if(!f_tmp) 
	{
		mds_api_poweroff_and_log("emer3", "canot fork crond");
	}
}
//====================================================

int main(int argc, char* argv[])
{
	int max_write_try = 10;
	int c = 0;
	int program_interval_min = MAIN_LOOP_RUN_INTERVAL_MIN;

    // ---------------------------------
	// make deamon
    // ---------------------------------
    pid_t pid = -1;
	if((pid = fork()) < 0) {
		exit(-1);
	} else if(pid != 0) {
		exit(0);
	}
	signal(SIGHUP, SIG_IGN);
	setsid();

	if((pid = fork()) < 0) {
		exit(-1);
	} else if(pid != 0) {
		exit(0);
	}
	signal(SIGHUP, SIG_IGN);
	setsid();
    
	// set crond script : fail check???
	while(max_write_try -- > 0 )
	{
		if ( set_crond_script() == 0)
			break;
	}

	printf("run crond process...\r\n");
	run_crond();

	// check process ...
	while(1)
	{
		if (program_interval_min-- > 0)
		{
			sleep(60);
			continue;
		}

		if ( mds_api_proc_find(MAIN_LOOP_CHK_PROG_CRON) >  0 )
		{
			printf("process %s : found!!!\r\n",MAIN_LOOP_CHK_PROG_CRON);
		}
		else
		{
			printf("process %s : not found!!!\r\n",MAIN_LOOP_CHK_PROG_CRON);
			run_crond();
		}

		program_interval_min = MAIN_LOOP_RUN_INTERVAL_MIN;
	}
}




#pragma once

#define CROND_WORKSPACE         "/tmp/"
#define DEBUG_LOG_FILE			"/var/log/mon.log"

#define SYSTEN_CRON_APP_PATH	"/sbin/crond"
#define SYSTEN_CRON_TMP			"/tmp"

#define POWEROFF_CMD			"poweroff &"

// ----------------------------
// check programs
// ----------------------------
#define MAIN_LOOP_RUN_INTERVAL_MIN	23
#define MAIN_LOOP_CHK_PROG_MON    "mon"
#define MAIN_LOOP_CHK_PROG_CRON    "crond"


// ----------------------------
// cron argument
// ----------------------------
#define CROND_RUN_INTERVAL_MIN1	    5
#define CROND_RUN_PROG_NAME1 	    "chkprog3"
#define CROND_RUN_PROG_ARG1 	    "-c emer3"

#define CROND_RUN_INTERVAL_MIN2	    5
#define CROND_RUN_PROG_NAME2 	    "chkprog3"
#define CROND_RUN_PROG_ARG2 	    "-c mon"



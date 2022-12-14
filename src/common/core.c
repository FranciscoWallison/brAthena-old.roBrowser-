/****************************************************************************!
*                _           _   _   _                                       *    
*               | |__  _ __ / \ | |_| |__   ___ _ __   __ _                  *  
*               | '_ \| '__/ _ \| __| '_ \ / _ \ '_ \ / _` |                 *   
*               | |_) | | / ___ \ |_| | | |  __/ | | | (_| |                 *    
*               |_.__/|_|/_/   \_\__|_| |_|\___|_| |_|\__,_|                 *    
*                                                                            *
*                                                                            *
* \file src/common/core.c                                                    *
* Descri??o Prim?ria.                                                        *
* Descri??o mais elaborada sobre o arquivo.                                  *
* \author brAthena, Athena, eAthena                                          *
* \date ?                                                                    *
* \todo ?                                                                    *   
*****************************************************************************/ 

#include "../common/mmo.h"
#include "../common/showmsg.h"
#include "../common/malloc.h"
#include "../common/strlib.h"
#include "core.h"
#include "../common/console.h"
#include "../common/random.h"
#ifndef MINICORE
	#include "../common/db.h"
	#include "../common/socket.h"
	#include "../common/timer.h"
	#include "../common/thread.h"
	#include "../common/sql.h"
	#include "../config/core.h"
	#include "../common/utils.h"
	#include "../common/conf.h"
	#include "../common/ers.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include "../common/winapi.h" // Console close event handling
#endif


/// Called when a terminate signal is received.
void (*shutdown_callback)(void) = NULL;

int runflag = CORE_ST_RUN;
int arg_c = 0;
char **arg_v = NULL;

char *SERVER_NAME = NULL;

#ifndef MINICORE    // minimalist Core
// Added by Gabuzomeu
//
// This is an implementation of signal() using sigaction() for portability.
// (sigaction() is POSIX; signal() is not.)  Taken from Stevens' _Advanced
// Programming in the UNIX Environment_.
//
#ifdef WIN32    // windows don't have SIGPIPE
#define SIGPIPE SIGINT
#endif

#ifndef POSIX
#define compat_signal(signo, func) signal((signo), (func))
#else
sigfunc *compat_signal(int signo, sigfunc *func) {
	struct sigaction sact, oact;

	sact.sa_handler = func;
	sigemptyset(&sact.sa_mask);
	sact.sa_flags = 0;
#ifdef SA_INTERRUPT
	sact.sa_flags |= SA_INTERRUPT;  /* SunOS */
#endif

	if(sigaction(signo, &sact, &oact) < 0)
		return (SIG_ERR);

	return (oact.sa_handler);
}
#endif

/*======================================
 *  CORE : Console events for Windows
 *--------------------------------------*/
#ifdef _WIN32
static BOOL WINAPI console_handler(DWORD c_event) {
    switch(c_event) {
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			if(shutdown_callback != NULL)
				shutdown_callback();
			else
				runflag = CORE_ST_STOP;// auto-shutdown
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

static void cevents_init() {
	if(SetConsoleCtrlHandler(console_handler,TRUE)==FALSE)
		ShowWarning(read_message("Source.common.console_event"));
}
#endif

/*======================================
 *  CORE : Signal Sub Function
 *--------------------------------------*/
static void sig_proc(int sn) {
	static int is_called = 0;

	switch(sn) {
		case SIGINT:
		case SIGTERM:
			if(++is_called > 3)
				exit(EXIT_SUCCESS);
			if(shutdown_callback != NULL)
				shutdown_callback();
			else
				runflag = CORE_ST_STOP;// auto-shutdown
			break;
		case SIGSEGV:
		case SIGFPE:
			do_abort();
			// Pass the signal to the system's default handler
			compat_signal(sn, SIG_DFL);
			raise(sn);
			break;
#ifndef _WIN32
		case SIGXFSZ:
			// ignore and allow it to set errno to EFBIG
			ShowWarning(read_message("Source.common.core_ssub_function"));
			//run_flag = 0; // should we quit?
			break;
		case SIGPIPE:
			//ShowInfo ("Broken pipe found... closing socket\n");   // set to eof in socket.c
			break;  // does nothing here
#endif
	}
}

void signals_init (void) {
	compat_signal(SIGTERM, sig_proc);
	compat_signal(SIGINT, sig_proc);
#ifndef _DEBUG // need unhandled exceptions to debug on Windows
	compat_signal(SIGSEGV, sig_proc);
	compat_signal(SIGFPE, sig_proc);
#endif
#ifndef _WIN32
	compat_signal(SIGILL, SIG_DFL);
	compat_signal(SIGXFSZ, sig_proc);
	compat_signal(SIGPIPE, sig_proc);
	compat_signal(SIGBUS, SIG_DFL);
	compat_signal(SIGTRAP, SIG_DFL);
#endif
}
#endif

#ifdef SVNVERSION
const char *get_svn_revision(void) {
	return EXPAND_AND_QUOTE(SVNVERSION);
}
#else// not SVNVERSION
const char *get_svn_revision(void) {
	static char svn_version_buffer[16] = "";
	FILE *fp;

	if(svn_version_buffer[0] != '\0')
		return svn_version_buffer;

	// subversion 1.7 uses a sqlite3 database
	// FIXME this is hackish at best...
	// - ignores database file structure
	// - assumes the data in NODES.dav_cache column ends with "!svn/ver/<revision>/<path>)"
	// - since it's a cache column, the data might not even exist
	if((fp = fopen(".svn"PATHSEP_STR"wc.db", "rb")) != NULL || (fp = fopen(".."PATHSEP_STR".svn"PATHSEP_STR"wc.db", "rb")) != NULL) {
#ifndef SVNNODEPATH
		//not sure how to handle branches, so i'll leave this overridable define until a better solution comes up
#define SVNNODEPATH brathena
#endif
		const char *prefix = "!svn/ver/";
		const char *postfix = "/"EXPAND_AND_QUOTE(SVNNODEPATH)")"; // there should exist only 1 entry like this
		size_t prefix_len = strlen(prefix);
		size_t postfix_len = strlen(postfix);
		size_t i,j,len;
		char *buffer;

		// read file to buffer
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		buffer = (char *)aMalloc(len + 1);
		fseek(fp, 0, SEEK_SET);
		len = fread(buffer, 1, len, fp);
		buffer[len] = '\0';
		fclose(fp);

		// parse buffer
		for(i = prefix_len + 1; i + postfix_len <= len; ++i) {
			if(buffer[i] != postfix[0] || memcmp(buffer + i, postfix, postfix_len) != 0)
				continue; // postfix missmatch
			for(j = i; j > 0; --j) {
				// skip digits
				if(!ISDIGIT(buffer[j - 1]))
					break;
			}
			if(memcmp(buffer + j - prefix_len, prefix, prefix_len) != 0)
				continue; // prefix missmatch
			// done
			
			snprintf(svn_version_buffer, sizeof(svn_version_buffer), "%d", atoi(buffer + j));
			break;
		}
		aFree(buffer);

		if(svn_version_buffer[0] != '\0')
			return svn_version_buffer;
	}

	// subversion 1.6 and older?
	if((fp = fopen(".svn/entries", "r")) != NULL) {
		char line[1024];
		int rev;
		// Check the version
		if(fgets(line, sizeof(line), fp)) {
			if(!ISDIGIT(line[0])) {
				// XML File format
				while(fgets(line,sizeof(line),fp))
					if(strstr(line,"revisao=")) break;
				if(sscanf(line," %*[^\"]\"%d%*[^\n]", &rev) == 1) {
					snprintf(svn_version_buffer, sizeof(svn_version_buffer), "%d", rev);
				}
			} else {
				// Bin File format
				if(fgets(line, sizeof(line), fp) == NULL) {
					ShowInfo(read_message("Source.common.svn_version_mes"));    // Get the name
				}
				if(fgets(line, sizeof(line), fp) == NULL) {
					ShowInfo(read_message("Source.common.svn_version_mes2"));    // Get the entries kind
				}
				if(fgets(line, sizeof(line), fp)) { // Get the rev numver
					snprintf(svn_version_buffer, sizeof(svn_version_buffer), "%d", atoi(line));
				}
			}
		}
		fclose(fp);

		if(svn_version_buffer[0] != '\0')
			return svn_version_buffer;
	}

	// fallback
	snprintf(svn_version_buffer, sizeof(svn_version_buffer), read_message("Source.common.svn_version_mes3"));
	return svn_version_buffer;
}
#endif

// Warning if executed as superuser (root)
void usercheck(void)
{
#ifndef _WIN32
	if(geteuid() == 0) {
		ShowWarning(read_message("Source.common.user_check"));
	}
#endif
}
void core_defaults(void) {
#ifndef MINICORE
	HCache_defaults();
#endif
	console_defaults();
	strlib_defaults();
	malloc_defaults();
#ifndef MINICORE
	libconfig_defaults();
	db_defaults();
	socket_defaults();
#endif
}
/*======================================
 *  CORE : MAINROUTINE
 *--------------------------------------*/
int main(int argc, char **argv)
{
	{
		// initialize program arguments
		char *p1 = SERVER_NAME = argv[0];
		char *p2 = p1;
		while((p1 = strchr(p2, '/')) != NULL || (p1 = strchr(p2, '\\')) != NULL) {
			SERVER_NAME = ++p1;
			p2 = p1;
		}
		arg_c = argc;
		arg_v = argv;
	}
	core_defaults();

	iMalloc->init();// needed for Show* in display_title() [FlavioJS]

	if (!(msg_silent&0x1))
		console->display_title();

	read_brathena_config();

#ifdef MINICORE // minimalist Core
	usercheck();
	do_init(argc,argv);
	do_final();
#else// not MINICORE
	set_server_type();
	usercheck();

	Sql_Init();
	rathread_init();
	DB->init();
	signals_init();

#ifdef _WIN32
	cevents_init();
#endif

	timer_init();

	/* timer first */
	rnd_init();
	srand((unsigned int)gettick());

	console->init();

	HCache->init();
	sockt->init();

	do_init(argc,argv);

	{
		// Main runtime cycle
		int next;
		while(runflag != CORE_ST_STOP) {
			next = do_timer(gettick_nocache());
			sockt->perform(next);
		}
	}

	console->final();

	do_final();

	timer_final();
	sockt->final();
	DB->final();
	rathread_final();
	ers_final();
#endif

	iMalloc->final();

	return 0;
}

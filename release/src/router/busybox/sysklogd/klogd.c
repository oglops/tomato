/* vi: set sw=4 ts=4: */
/*
 * Mini klogd implementation for busybox
 *
 * Copyright (C) 2001 by Gennady Feldman <gfeldman@gena01.com>.
 * Changes: Made this a standalone busybox module which uses standalone
 * syslog() client interface.
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * Copyright (C) 2000 by Karl M. Hegbloom <karlheg@debian.org>
 *
 * "circular buffer" Copyright (C) 2000 by Gennady Feldman <gfeldman@gena01.com>
 *
 * Maintainer: Gennady Feldman <gfeldman@gena01.com> as of Mar 12, 2001
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */

#include "libbb.h"
#include <syslog.h>


/* The Linux-specific klogctl(3) interface does not rely on the filesystem and
 * allows us to change the console loglevel. Alternatively, we read the
 * messages from _PATH_KLOG. */

#if ENABLE_FEATURE_KLOGD_KLOGCTL

# include <sys/klog.h>

static void klogd_open(void)
{
	/* "Open the log. Currently a NOP" */
	klogctl(1, NULL, 0);
}

static void klogd_setloglevel(int lvl)
{
	/* "printk() prints a message on the console only if it has a loglevel
	 * less than console_loglevel". Here we set console_loglevel = lvl. */
	klogctl(8, NULL, lvl);
}

static int klogd_read(char *bufp, int len)
{
	return klogctl(2, bufp, len);
}
# define READ_ERROR "klogctl(2) error"

static void klogd_close(void)
{
	/* FYI: cmd 7 is equivalent to setting console_loglevel to 7
	 * via klogctl(8, NULL, 7). */
	klogctl(7, NULL, 0); /* "7 -- Enable printk's to console" */
	klogctl(0, NULL, 0); /* "0 -- Close the log. Currently a NOP" */
}

#else

# include <paths.h>
# ifndef _PATH_KLOG
#  ifdef __GNU__
#   define _PATH_KLOG "/dev/klog"
#  else
#   error "your system's _PATH_KLOG is unknown"
#  endif
# endif
# define PATH_PRINTK "/proc/sys/kernel/printk"

enum { klogfd = 3 };

static void klogd_open(void)
{
	int fd = xopen(_PATH_KLOG, O_RDONLY);
	xmove_fd(fd, klogfd);
}

static void klogd_setloglevel(int lvl)
{
	FILE *fp = fopen_or_warn(PATH_PRINTK, "w");
	if (fp) {
		/* This changes only first value:
		 * "messages with a higher priority than this
		 * [that is, with numerically lower value]
		 * will be printed to the console".
		 * The other three values in this pseudo-file aren't changed.
		 */
		fprintf(fp, "%u\n", lvl);
		fclose(fp);
	}
}

static int klogd_read(char *bufp, int len)
{
	return read(klogfd, bufp, len);
}
# define READ_ERROR "read error"

static void klogd_close(void)
{
	klogd_setloglevel(7);
	if (ENABLE_FEATURE_CLEAN_UP)
		close(klogfd);
}

#endif

char log_buffer[6 * 1024 + 1];	/* Big enough to not lose msgs at bootup */
enum {
	KLOGD_LOGBUF_SIZE = sizeof(log_buffer),
	OPT_LEVEL      = (1 << 0),
	OPT_FOREGROUND = (1 << 1),
};

/* TODO: glibc openlog(LOG_KERN) reverts to LOG_USER instead,
 * because that's how they interpret word "default"
 * in the openlog() manpage:
 *      LOG_USER (default)
 *              generic user-level messages
 * and the fact that LOG_KERN is a constant 0.
 * glibc interprets it as "0 in openlog() call means 'use default'".
 * I think it means "if openlog wasn't called before syslog() is called,
 * use default".
 * Convincing glibc maintainers otherwise is, as usual, nearly impossible.
 * Should we open-code syslog() here to use correct facility?
 */

int klogd_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int klogd_main(int argc UNUSED_PARAM, char **argv)
{
	int i = 0;
	char *opt_c;
	int opt;
	int used;
	unsigned int cnt;

	opt = getopt32(argv, "c:n", &opt_c);
	if (opt & OPT_LEVEL) {
		/* Valid levels are between 1 and 8 */
		i = xatou_range(opt_c, 1, 8);
	}
	if (!(opt & OPT_FOREGROUND)) {
		bb_daemonize_or_rexec(DAEMON_CHDIR_ROOT, argv);
	}

	logmode = LOGMODE_SYSLOG;

	/* klogd_open() before openlog(), since it might use fixed fd 3,
	 * and openlog() also may use the same fd 3 if we swap them:
	 */
	klogd_open();
	openlog("kernel", 0, LOG_KERN);

	if (i)
		klogd_setloglevel(i);

	bb_signals(BB_FATAL_SIGS, record_signo);
	signal(SIGHUP, SIG_IGN);

	syslog(LOG_NOTICE, "klogd started: %s", bb_banner);

	used = 0;
	cnt = 0;
	while (!bb_got_signal) {
		int n;
		int priority;
		char *start;
		char *eor;

		/* "2 -- Read from the log." */
		start = log_buffer + used;
		n = klogd_read(start, KLOGD_LOGBUF_SIZE-1 - used);
		if (n < 0) {
			if (errno == EINTR)
				continue;
			bb_perror_msg(READ_ERROR);
			break;
		}
		start[n] = '\0';
		eor = &start[n];

		/* Process each newline-terminated line in the buffer */
		start = log_buffer;
		while (1) {
			char *newline = strchrnul(start, '\n');

			if (*newline == '\0') {
				/* This line is incomplete */

				/* move it to the front of the buffer */
				overlapping_strcpy(log_buffer, start);
				used = newline - start;
				if (used < KLOGD_LOGBUF_SIZE-1) {
					/* buffer isn't full */
					break;
				}
				/* buffer is full, log it anyway */
				used = 0;
				newline = NULL;
			} else {
				*newline++ = '\0';
			}

			/* Extract the priority */
			priority = LOG_INFO;
			if (*start == '<') {
				start++;
				if (*start) {
					/* kernel never generates multi-digit prios */
					priority = (*start - '0');
					start++;
				}
				if (*start == '>')
					start++;
			}
			/* Log (only non-empty lines) */
			if (*start) {
				syslog(priority, "%s", start);
				/* give syslog time to catch up */
				++cnt;
				if ((cnt & 0x07) == 0 && (cnt < 300 || (eor - start) > 200))
					usleep(50 * 1000);
			}

			if (!newline)
				break;
			start = newline;
		}
	}

	klogd_close();
	syslog(LOG_NOTICE, "klogd: exiting");
	if (bb_got_signal)
		kill_myself_with_sig(bb_got_signal);
	return EXIT_FAILURE;
}

/*
* echo.c --
*
*	Produce a page containing all FastCGI inputs
*
*
* Copyright (c) 1996 Open Market, Inc.
*
* See the file "LICENSE.TERMS" for information on usage and redistribution
* of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*
*/
#ifndef lint
static const char rcsid[] = "$Id: echo.c,v 1.5 1999/07/28 00:29:37 roberts Exp $";
#endif /* not lint */

#include "fcgi_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif

#include "fcgi_stdio.h"


static void PrintEnv(char *label, char **envp)
{
    printf("%s:<br>\n<pre>\n", label);
    for ( ; *envp != NULL; envp++) {
        printf("%s\n", *envp);
    }
    printf("</pre><p>\n");
}

int main ()
{
    char **initialEnv = environ;
    int count = 0;

    while (FCGI_Accept() >= 0) {
        char *contentLength = getenv("CONTENT_LENGTH");
        int len;

        printf("Content-type: text/html\r\n"
               "\r\n"
               "<title>FastCGI echo</title>"
               "<h1>FastCGI echo</h1>\n"
               "Request number %d,  Process ID: %d<p>\n", ++count, getpid());

        if (contentLength != NULL) {
            len = strtol(contentLength, NULL, 10);
        }
        else {
            len = 0;
        }

        if (len <= 0) {
            printf("No data from standard input.<p>\n");
        }
        else {
            int i, ch;
            char *buf = (char *)malloc(sizeof(char)*len);
            memset(buf, 0, len);

            printf("Standard input:<br>\n<pre>\n");
            for (i = 0; i < len; i++) {
                if ((ch = getchar()) < 0) {
                    printf("Error: Not enough bytes received on standard input<p>\n");
                    break;
                }
                buf[i] = ch;
                putchar(ch);
            }
            printf("\n</pre><p>\n");
            printf("len = %d\n", len);
            int flag = 0;
            int num = 0;
            int n = 0;
            for (i = 0; i < len; i++)
            {
                if (buf[i] == '\r')
                {
                    flag++;
                    if (flag == 1)
                    {
                        num = i + 4;
                    }
                    if (flag == 4)
                    {
                        n = i + 2;
                    }
                }
            }
            int tmplen = len - n - num;
            char *tmp = (char *)malloc(tmplen);
            memset(tmp, 0, tmplen);
            strncpy(tmp, buf+n, tmplen);
            printf("tmp : %s, tmplen = %d\n", tmp, tmplen);
            int fd;
            fd = open("buf.txt", O_CREAT | O_RDWR,0644);
            write(fd, tmp, tmplen);
            free(buf);
            free(tmp);
        }

        PrintEnv("Request environment", environ);
        PrintEnv("Initial environment", initialEnv);
    } /* while */

    return 0;
}

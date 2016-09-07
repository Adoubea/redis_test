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
#include "memstr.h"

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
            
            char *str1 = NULL;
            char *str2 = NULL;
//            FILE *fd;

            int fd;
            char tmp[256] = {0};
          
            
            
            
            str1 = memstr(buf, len, "filename");
            str2 = memstr(buf, len, "Content-Type");
            
            memcpy(tmp, str1+10, str2-str1-13);
            //strncpy(tmp, str1+10, str2-str1-13);
//            fd = fopen(tmp, "wb+");
            fd = open(tmp, O_CREAT | O_RDWR | O_TRUNC, 0644);
           // printf("tmp = %s\n", tmp);
            
            str1 = memstr(buf, len, "Content-Disposition");
            memset(tmp, 0, 256);
            //strncpy(tmp, buf, str1-buf-2);
            memcpy(tmp, buf, str1-buf-2);
            
            
            str1 = memstr(buf, len, "\r\n\r\n");
            str2 = memstr(str1+4, strlen(str1+4), tmp);
            
            int tmplen = str2-str1-4;
            char *tmpbuf = (char *)malloc(tmplen);
            
            memcpy(tmpbuf, str1+4, tmplen);
            //strncpy(tmpbuf, str1+4, tmplen);
            //printf("tmpbuf = %s\n", tmpbuf);
            
	          printf("tmplen = %d\n", tmplen);
//            fwrite(tmpbuf, 1, tmplen, fd);
           write(fd, tmpbuf, tmplen);
           
						close(fd);
            free(buf);
            free(tmpbuf);

        }

        PrintEnv("Request environment", environ);
        PrintEnv("Initial environment", initialEnv);
    } /* while */

    return 0;
}

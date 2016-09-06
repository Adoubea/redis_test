/*************************************************************************
	> File Name: demo.c
	> Author: Adoubea
	> Mail: wgx_0505@163.com
	> Created Time: 2016年09月06日 星期二 14时07分13秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fcgi_stdio.h"

int main(int argc, char *argv[])
{
	int count = 0;
	
	while (FCGI_Accept() >= 0) {
		printf("Content-type: text/html\r\n");
		printf("\r\n");
		printf("<title>Fast CGI Hello!</title>");
		printf("<h1>Fast CGI Hello!</h1>");
		printf("Request number %d running on host <i>%s</i>\n", ++count, getenv("SERVER_NAME"));
	}
	
	return 0;
}
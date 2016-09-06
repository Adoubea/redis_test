#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "make_log.h"
#include "redis_op.h"


int main(int argc, char *argv[])
{
    int ret = 0;
    char *ip = "127.0.0.1";
    char *port = "6379";
    redisContext *conn;
    char value[256] = {0};

    conn = rop_connectdb_nopwd(ip, port);
    if (conn == NULL)
    {
        ret = -1;
        printf("connect error\n");
        goto End;
    }
   
    ret = rop_set_key_value(conn, "C++", "iostream");
    if (ret == 0)
    {
        printf("rop_set_key_value error");
        goto End;
    }

    ret = rop_get_key(conn, "C++", value);
    if (ret == 0)
    {
        printf("rop_get_key error\n");
        goto End;
    }

    printf("value = %s\n", value);
End:
   
    rop_disconnect(conn);
    return ret;
}

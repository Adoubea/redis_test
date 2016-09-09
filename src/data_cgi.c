/*************************************************************************
	> File Name: data_cgi.c
	> Author: Adoubea
	> Mail: wgx_0505@163.com
	> Created Time: 2016年09月09日 星期五 14时29分15秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "fdfs_client.h" 
#include "fcgi_stdio.h"
#include "fcgi_config.h"
#include "cJSON.h"
#include "util_cgi.h"
#include "redis_op.h"
#include "make_log.h"

#define DATA_LOG_MODULE          "cgi"
#define DATA_LOG_PROC            "data"

extern char g_host_name[HOST_NAME_LEN];

int get_file_url_dynamic(char *file_id, char *file_url)
{
    int result;
    FDFSFileInfo file_info;

	if ((result=fdfs_client_init(FDFS_CLIENT_CONF)) != 0)
	{
        LOG(DATA_LOG_MODULE, DATA_LOG_PROC, "fdfs_client_init error");
		return result;
	}

    memset(&file_info, 0, sizeof(file_info));

	result = fdfs_get_file_info_ex1(file_id, true, &file_info);
	if (result != 0)
	{
		LOG(DATA_LOG_MODULE, DATA_LOG_PROC, "query file info fail, " \
			"error no: %d, error info: %s\n", \
			result, STRERROR(result));
	}
    else {
        memset(file_url, 0, FILE_NAME_LEN);
        strcat(file_url, "http://");
        strcat(file_url, file_info.source_ip_addr);
        strcat(file_url, "/");
        strcat(file_url, file_id);

        //LOG(DATA_LOG_MODULE, DATA_LOG_PROC, "file_url[%s]", file_url);
    }

	tracker_close_all_connections();
	fdfs_client_destroy();

    return result;
}

void print_file_list_json(int fromId, int count, char *cmd, char *kind)
{


    int i = 0;


    cJSON *root = NULL; 
    cJSON *array =NULL;
    char *out;
    char filename[FILE_NAME_LEN] = {0};
    char create_time[FIELD_ID_SIZE] ={0};
    char picurl[PIC_URL_LEN] = {0};
    char suffix[8] = {0};
    char pic_name[PIC_NAME_LEN] = {0};
    char file_url[FILE_NAME_LEN] = {0};
    char file_id[FILE_NAME_LEN] = {0};
    char user[128] = {0};
    int retn = 0;
    int endId = fromId + count - 1;
    int score = 0;

    RVALUES file_list_values = NULL;
    int value_num;
    redisContext *redis_conn = NULL;

    redis_conn = rop_connectdb_nopwd("127.0.0.1", "6379");
    if (redis_conn == NULL) {
        LOG(DATA_LOG_MODULE, DATA_LOG_PROC, "redis connected error");
        return;
    }

    LOG(DATA_LOG_MODULE, DATA_LOG_PROC, "fromId:%d, count:%d",fromId, count);
    file_list_values = malloc(count*VALUES_ID_SIZE);

    retn = rop_range_list(redis_conn, FILE_INFO_LIST, fromId, endId, file_list_values, &value_num);
    if (retn < 0) {
        LOG(DATA_LOG_MODULE, DATA_LOG_PROC, "redis range list error");
        rop_disconnect(redis_conn);
        return;
    }
    LOG(DATA_LOG_MODULE, DATA_LOG_PROC, "value_num=%d\n", value_num);


    root = cJSON_CreateObject();
    array = cJSON_CreateArray();
    for (i = 0;i < value_num;i ++) {
        //array[i]:
        cJSON* item = cJSON_CreateObject();

        //id
        //cJSON_AddNumberToObject(item, "id", i);
        get_value_by_col(file_list_values[i], 1, file_id, VALUES_ID_SIZE-1, 0);
        cJSON_AddStringToObject(item, "id", file_id);

        //kind
        cJSON_AddNumberToObject(item, "kind", 2);

        //title_m(filename)
        get_value_by_col(file_list_values[i], 3, filename, VALUES_ID_SIZE-1, 0);
        LOG(DATA_LOG_MODULE, DATA_LOG_PROC, "filename=%s\n", filename);

        cJSON_AddStringToObject(item, "title_m", filename);

        //title_s
        get_value_by_col(file_list_values[i], 5, user, VALUES_ID_SIZE-1, 0);
        cJSON_AddStringToObject(item, "title_s", user);

        //time
        get_value_by_col(file_list_values[i], 4, create_time, VALUES_ID_SIZE-1, 0);
        cJSON_AddStringToObject(item, "descrip", create_time);
        LOG(DATA_LOG_MODULE, DATA_LOG_PROC, "create_time=%s\n", create_time);

        //picurl_m
        memset(picurl, 0, PIC_URL_LEN);
        strcat(picurl, g_host_name);
        strcat(picurl, "/static/file_png/");


        get_file_suffix(filename, suffix);
        sprintf(pic_name, "%s.png", suffix);
        strcat(picurl, pic_name);
        cJSON_AddStringToObject(item, "picurl_m", picurl);

        //url
#if GET_URL_DYNAMIC
        get_file_url_dynamic(file_id, file_url);
#else
        get_value_by_col(file_list_values[i], 2, file_url, VALUES_ID_SIZE-1, 0);
#endif
        cJSON_AddStringToObject(item, "url", file_url);
        LOG(DATA_LOG_MODULE, DATA_LOG_PROC, "file_url=%s\n", file_url);

        //pv
        score = rop_zset_get_score(redis_conn, FILE_HOT_ZSET, file_id);
        cJSON_AddNumberToObject(item, "pv", score-1);

        //hot 
        //cJSON_AddNumberToObject(item, "hot", i%2);
        cJSON_AddNumberToObject(item, "hot", 0);


        cJSON_AddItemToArray(array, item);

    }



    cJSON_AddItemToObject(root, "games", array);

    out = cJSON_Print(root);

    LOG(DATA_LOG_MODULE, DATA_LOG_PROC,"%s", out);
    printf("%s\n", out);

    free(file_list_values);
    free(out);

    rop_disconnect(redis_conn);
}

int main(int argc, char *argv[])
{
	char fromId[5];
    char count[5];
    char cmd[20];
    char kind[10];
    char fileId[FILE_NAME_LEN];
    

    while (FCGI_Accept() >= 0)
     {
        char *query = getenv("QUERY_STRING");
        memset(fromId, 0, 5);
        memset(count, 0, 5);
        memset(cmd, 0, 20);
        memset(kind, 0, 10);
        memset(fileId, 0, FILE_NAME_LEN);

        query_parse_key_value(query, "cmd", cmd, NULL);

        if (strcmp(cmd, "newFile") == 0) {
            //请求最新文件列表命令

            query_parse_key_value(query, "fromId", fromId, NULL);
            query_parse_key_value(query, "count", count, NULL);
            query_parse_key_value(query, "kind", kind, NULL);
            LOG(DATA_LOG_MODULE, DATA_LOG_PROC, "=== fromId:%s, count:%s, cmd:%s, kind:%s", fromId, count, cmd, kind);
            cgi_init();

            printf("Content-type: text/html\r\n");
            printf("\r\n");

            print_file_list_json(atoi(fromId), atoi(count), cmd, kind);
        }
     }
     
     return 0;
}

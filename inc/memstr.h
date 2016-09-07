/*************************************************************************
	> File Name: memstr.h
	> Author: Adoubea
	> Mail: wgx_0505@163.com
	> Created Time: 2016年09月07日 星期三 09时19分13秒
 ************************************************************************/

#ifndef _MEMSTR_H
#define _MEMSTR_H

//find 'substr' from a fixed-length buffer   
//('full_data' will be treated as binary data buffer)  
//return NULL if not found  
char* memstr(char* full_data, int full_data_len, char* substr);

#endif

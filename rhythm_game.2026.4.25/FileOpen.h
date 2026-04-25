#pragma once
#include<stdio.h>
FILE* file_open(const char* file_name, const char* mode)
{
	char name[100];
	sprintf(name, "GameSystemFile/%s", file_name);
	return fopen(name, mode);
}
int neme_change(const char* file_name, const char* after_name)
{
	char name[100];
	char name1[100];
	sprintf(name, "GameSystemFile/%s", file_name);
	sprintf(name1, "GameSystemFile/%s", after_name);
	return rename(name, name1);
}
int file_delete(const char* file_name)
{
	char name[100];
	sprintf(name, "GameSystemFile/%s", file_name);
	return remove(name);
}

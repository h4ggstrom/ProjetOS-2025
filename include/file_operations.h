#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

int create_file(const char *path, const char *content);
int delete_file(const char *path);
int mycp(const char *source_path, const char *dest_path);
int mymv(const char *source_path, const char *dest_path);

#endif // FILE_OPERATIONS_H
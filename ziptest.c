#include <string.h>
#include "include/zip.h"

enum {
    INCORRECT_PASSWD = 0,
    CORRECT_PASSWD,
    INVALID_FILE,
};

int isZipPasswdCorrect(char *filename,char *passwd)
{
    zip_t *archive = NULL;
    int err = 0;
    char *fname = NULL;
    zip_file_t *file = NULL;
    zip_error_t *zerr;
    //打开zip压缩文件
    archive = zip_open(filename, ZIP_RDONLY, &err);
    if(archive == NULL)
    {
        return INVALID_FILE;
    }
//输入zip密码
    zip_set_default_password(archive, passwd);
    //获取文件名
//打开文件
    file = zip_fopen_index(archive, 0, ZIP_FL_UNCHANGED);
    if(file == NULL)
    {
        zerr = zip_get_error(archive);
        char *serr = zip_error_strerror(zerr);
        puts(serr);
        printf("%d\n",strchr(serr,'W'));
        if (strchr(serr,'W')) {
            zip_close(archive);
            return INCORRECT_PASSWD;
        } else {
            zip_close(archive);
            return CORRECT_PASSWD;
        }
    }
//关闭文件
    zip_fclose(file);
//关闭压缩文件
    zip_close(archive);
    return INCORRECT_PASSWD;
}



int main()
{
    printf("%d\n",isZipPasswdCorrect("log.zip","332"));
}
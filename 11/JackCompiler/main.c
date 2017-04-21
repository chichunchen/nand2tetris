#include "helper.h"
#include "constant.h"

int putback;
int is_LL1;
char token[MAXTOKEN];
int tokenType;

int main(int argc, const char *argv[])
{
    if (argc < 2)
        printf("usage: ./a.out <name of file or directory>\n");
    else {
        putback = 0;
        FILE *fp;
        FILE *fw;
        if (!is_directory(argv[1])) {
            fp = fopen(argv[1], "r");
            char *ptr = setFileName(argv[1]);
            fw = fopen(ptr, "w");
            compileClass(fp, fw);
            fclose(fp);
            fclose(fw);
            //tokenTest(fp, fw);
            symbolTest();
        } else {        // dir
            WIN32_FIND_DATA fdFile;
            HANDLE hFind = NULL;

            char sPath[2048];

            //Specify a file mask. *.* = We want everything!
            sprintf(sPath, "%s\\*.*", argv[1]);
            //printf("%s\n", sPath);
            if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) {
                printf("Path not found: [%s]\n", argv[1]);
                return 1;
            }

            do {
                //Find first file will always return "."
                //    and ".." as the first two directories.
                if (strcmp(fdFile.cFileName, ".") != 0
                    && strcmp(fdFile.cFileName, "..") != 0) {
                    //Build up our file path using the passed in
                    //  [sDir] and the file/foldername we just found:
                    sprintf(sPath, "%s\\%s", argv[1], fdFile.cFileName);

                    //Is the entity a File or Folder?
                    if (fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY) {
                        printf("Directory: %s\n", sPath);
                    }
                    else {
                        char *p = sPath;
                        while(*p++ != '.')
                            ;
                        if (strcmp("jack", p) == 0) {
                            fp = fopen(sPath, "r");
                            char *ptr = setFileName(sPath);
                            fw = fopen(ptr, "w");
                            compileClass(fp, fw);
                            fclose(fp);
                            fclose(fw);
                        }
                    }
                }
            } while(FindNextFile(hFind, &fdFile)); //Find the next file.

            FindClose(hFind); //Always, Always, clean things up!
        }
    }

    return 0;
}

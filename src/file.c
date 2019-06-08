#include <sys/types.h>
#include <sys/stat.h>

#include "../include/const.h"
#include "../include/file.h"

void permissions(char *file, char modeval[FMODE_LEN]) {
    struct stat st;
    if (stat(file, &st) == 0) {
        mode_t perm = st.st_mode;
        modeval[0]  = (perm & __S_IFDIR) ? 'd' : '-';
        modeval[1]  =  (perm & S_IRUSR)  ? 'r' : '-';
        modeval[2]  =  (perm & S_IWUSR)  ? 'w' : '-';
        modeval[3]  =  (perm & S_IXUSR)  ? 'x' : '-';
        modeval[4]  =  (perm & S_IRGRP)  ? 'r' : '-';
        modeval[5]  =  (perm & S_IWGRP)  ? 'w' : '-';
        modeval[6]  =  (perm & S_IXGRP)  ? 'x' : '-';
        modeval[7]  =  (perm & S_IROTH)  ? 'r' : '-';
        modeval[8]  =  (perm & S_IWOTH)  ? 'w' : '-';
        modeval[9]  =  (perm & S_IXOTH)  ? 'x' : '-';
        modeval[10] = '\0';
    } else {
        return;
    }   
}
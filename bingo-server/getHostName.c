#include "getHostName.h"

bool getIpAddr(char* name, IN_ADDR* addr) {

    HOSTENT* ptr = gethostbyname(name);
    if(ptr == NULL) {
        err_display("gethostbyname()");
        return FALSE;
    }

    memcpy(addr, ptr->h_addr, ptr->h_length);
    return TRUE;
}

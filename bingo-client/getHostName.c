#include "getHostName.h"

IN_ADDR getIpAddr(char* name) {

    // 호스트로부터 IP 주소를 받아온다
    HOSTENT* hostaddr = gethostbyname(name);
    int *add;
    if(hostaddr == NULL) {
        perror("ERROR : ");
        exit(0);
    }
    // 호스트로부터 IP 주소를 받아온다
    add = (long int*)*hostaddr->h_addr;

    IN_ADDR ipaddr;
    ipaddr.s_addr = add;
    return ipaddr;
}

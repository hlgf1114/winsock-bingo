#include "getHostName.h"

IN_ADDR getIpAddr(char* name) {

    // ȣ��Ʈ�κ��� IP �ּҸ� �޾ƿ´�
    HOSTENT* hostaddr = gethostbyname(name);
    int *add;
    if(hostaddr == NULL) {
        perror("ERROR : ");
        exit(0);
    }
    // ȣ��Ʈ�κ��� IP �ּҸ� �޾ƿ´�
    add = (long int*)*hostaddr->h_addr;

    IN_ADDR ipaddr;
    ipaddr.s_addr = add;
    return ipaddr;
}

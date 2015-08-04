#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

char* getIP(const char* interface);

char* getIP(const char* interface)
{
int iSocket = -1;
char* IP = 0;

// create a socket to be used when calling ioctl().
if ((iSocket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
{
perror("socket");
return 0;
}

// if_nameindex - return all network interface names and indexes
struct if_nameindex* pIndex = if_nameindex();
struct if_nameindex* pIndex2 = pIndex;

while ((pIndex != NULL) && (pIndex->if_name != NULL))
{
struct ifreq req;

strncpy(req.ifr_name, pIndex->if_name, IFNAMSIZ);

// ioctl - control a STREAMS device
if (ioctl(iSocket, SIOCGIFADDR, &req) < 0)
{
if (errno == EADDRNOTAVAIL)
{
++pIndex;
continue;
}
perror("ioctl");
close(iSocket);
return 0;
}

if (strncmp(interface, pIndex->if_name, strlen(interface)) == 0)
{
IP = strdup(inet_ntoa(((struct sockaddr_in*)&req.ifr_addr)->sin_addr));
break;
}
++pIndex;
}


// if_freenameindex - free memory allocated by if_nameindex
if_freenameindex(pIndex2);

close(iSocket);

return IP;
}
/*
int main(void)
{
char* IP = getIP("eth0");

if (IP)
{
printf("%s\n", IP);

free(IP);
}

return 0;
}*/

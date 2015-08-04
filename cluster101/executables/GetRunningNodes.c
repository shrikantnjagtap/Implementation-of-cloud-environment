#include <stdio.h>
#include <math.h>
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

char* getIPAddress( const char* interface );
int thirdOctet( char* ipAddress );
void getUserIdClusterIdByThirdOctet( int octet, int* userId, int* clusterId );
char* getKVMHostIPAddress( char* guestIPAddress );
char* getRunningNodes( );
/*
int main(int argc, char **argv) {

	printf( "List of running nodes in this cluster : %s\n", getRunningNodes( ) );
	return 0;

}
*/
char* getIPAddress( const char* interface ) {

	int iSocket = -1;
	char* IP = 0;

	// create a socket to be used when calling ioctl().
	if ((iSocket = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return 0;
	}

	// if_nameindex - return all network interface names and indexes
	struct if_nameindex* pIndex = if_nameindex();
	struct if_nameindex* pIndex2 = pIndex;

	while ((pIndex != NULL) && (pIndex->if_name != NULL)) {

		struct ifreq req;
		strncpy(req.ifr_name, pIndex->if_name, IFNAMSIZ);

		// ioctl - control a STREAMS device
		if (ioctl(iSocket, SIOCGIFADDR, &req) < 0) {

			if (errno == EADDRNOTAVAIL) {
				++pIndex;
				continue;
			}

			perror("ioctl");
			close(iSocket);
			return 0;

		}

		if (strncmp(interface, pIndex->if_name, strlen(interface)) == 0) {

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

int thirdOctet( char* ipAddress ) {

	char octet[3];
	int i=0;
	int count=0;

	memset( octet, '\0', sizeof(octet) );

	while( *ipAddress != '\0' ) {
		
		if( *ipAddress == '.' ) {
			
			count++;

		}
		
		if( count == 2 ) {
		
			ipAddress++;

			while( *ipAddress != '.' ) {
				
				octet[i++] = *ipAddress;

				ipAddress++;

			}

			return atoi(octet);

		}

		ipAddress++;

	}

	return -1;
}

void getUserIdClusterIdByThirdOctet( int octet, int* userId, int* clusterId ) {

	*userId = ceil( octet/16.0 );
	*clusterId = octet%16;

	if( *clusterId == 0 ) {

		*clusterId=16;

	}

}

// Find out KVM Host's IP Address given this Guest's IP Address

char* getKVMHostIPAddress( char* guestIPAddress ) {

	const int BUFFER_SIZE = 16;
	char* buffer = ( char * )malloc( BUFFER_SIZE );	// Store KVM host's IP address into buffer

	int i=0;
	int count=0;

	memset( buffer, '\0', BUFFER_SIZE );

	while( *guestIPAddress != '\0' ) {
		
		if( *guestIPAddress == '.' ) {
			
			count++;

		}

		buffer[i] = *guestIPAddress;	// Copy current character of guestIPAddress to buffer's current position
		
		if( count == 3 ) {
		
			guestIPAddress++;
			i++;

			buffer[i] = '1';

			return buffer;

		}

		guestIPAddress++;
		i++;

	}

	return NULL;

}

char* getRunningNodes( ) {
	
	int userId=0, clusterId=0;
	char command[256];
	const int BUFFER_SIZE = 128;
	char* buffer = ( char * )malloc( BUFFER_SIZE );
	FILE* output=NULL;

	memset( command, '\0', sizeof(command) );
	memset( buffer, '\0', BUFFER_SIZE );
	
	getUserIdClusterIdByThirdOctet( thirdOctet( getIPAddress( "eth0" ) ), &userId, &clusterId );

	// Execute bash script on host machine using SSH
	
	sprintf( command, "ssh %s 'bash /home/cluster101/executables/getRunningNodes.sh %d %d'", getKVMHostIPAddress( getIPAddress( "eth0" ) ), userId, clusterId );

	output = popen( command, "r" );

	while( fgets(buffer, BUFFER_SIZE, output) != 0 ) {
	}
	pclose(output);

	return buffer;
}

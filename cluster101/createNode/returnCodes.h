#define SUCCESS 			0
#define ARG_LOW				1
#define DB_ERROR			2 //NO DATA BASE ENTRY
#define XML_CREATION		3 //most probably sudo problem
#define IMAGE_RESIZE		4
#define COPY_SCRIPT_MASTER	5
#define MASTER_NODE			6
#define COPY_SCRIPT_CLIENT	7
#define CLIENT_NODE			8




#define CONN_ERROR			9  //connection error
#define XML_OPEN			10  //unable to load xml from file
#define XML_READ			11
#define LIBVIRT_INIT		12
#define LIBVIRT_CONN		13
#define LIBVIRT_XML_OPEN	14 //error in virDomainCreateXml
#define LIBVIRT_NO_DOMAIN	15		//virDomainLookupByName() -- NO DOMAIN

#define CONNECTION_NOT_CLOSED 16
#define DOMAIN_DESTROY		17
#define XML_READ			11

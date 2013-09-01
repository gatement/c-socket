#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<netdb.h>
#include<stdio.h>

#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)


/*
#include <netdb.h>
struct hostent {
	char  *h_name;            // official name of host 
	char **h_aliases;         // alias list 
	int    h_addrtype;        // host address type 
	int    h_length;          // length of address
	char **h_addr_list;       // list of addresses
}
*/

int getlocalip(char *ip)
{
    char host[100] = {0};
    if (gethostname(host, sizeof(host)) < 0)
        return -1;

    struct hostent *hp;
    if ((hp = gethostbyname(host)) == NULL)
        return -1;

    //  #define h_addr h_addr_list[0]
    strcpy(ip, inet_ntoa(*(struct in_addr *)hp->h_addr_list[0]));

    return 0;
}

int main(void)
{
    char host[100] = {0};
    if (gethostname(host, sizeof(host)) < 0)
        ERR_EXIT("gethostname error");

    struct hostent *hp;
    if ((hp = gethostbyname(host)) == NULL)
        ERR_EXIT("gethostbyname error");

	printf("name: %s, addrLen: %d\n", hp->h_name, hp->h_length);

    int i = 0;
	printf("aliases:\n");
    while (hp->h_aliases[i] != NULL)
    {
        printf("%s\n", hp->h_aliases[i]);
        i++;
    }

	i = 0;
	printf("addrs:\n");
    while (hp->h_addr_list[i] != NULL)
    {
        printf("%s\n", inet_ntoa(*(struct in_addr *)hp->h_addr_list[i]));
        i++;
    }

    char ip[16] = {0};
    getlocalip(ip);
    printf("\nlocal ip: %s\n" , ip);
    return 0;
}

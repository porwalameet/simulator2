/*
 * Some of the code to generate the ping packets is taken from
 * https://www.cs.utah.edu.~swalton/listings/sockets/programs/part4/chap18/myping.c
 */
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include <fcntl.h>
#include <netdb.h>
#include <limits.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include "../common/log.h"
#include "bgp.h"
/* somewhat unix-specific */ 
#include <sys/time.h>
#include <unistd.h>
/* curl stuff */ 
#include <curl/curl.h>
 

FILE *fp;
FILE *fbgpStats;

void bgpDisplay (void *buf, int bytes, jsonData_t* jsonData) {
    int i;
    struct iphdr *ip = buf;
    char src[INET_ADDRSTRLEN];
    char dst[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &ip->saddr, src, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ip->daddr, dst, INET_ADDRSTRLEN);
    i = inet_ntoa(ip->daddr);
#ifdef DEBUG
    for (i=0;i<bytes; i++) {
        if (!(i&15)) log_debug(fp, "%2X: ", i);
        log_debug(fp, "%2X ", ((unsigned char*)buf)[i]);
    }
#endif
    log_info(fp, "IPv%d:hdr-size=%d pkt-size=%d protocol=%d TTL=%d",
    ip->version, (ip->ihl)*4, ntohs(ip->tot_len), ip->protocol, ip->ttl);
    log_info(fp, "src: %s, dst: %s", src, dst);

    fflush(fp);
}


void* bgpStart(void *args) {
	char filePath[100];

	jsonData_t* jsonData = (jsonData_t*)args;

	sprintf(filePath, "/var/monT/");
	sprintf(&filePath[strlen("/var/monT/")], "%d", jsonData->custID);
	sprintf(&filePath[strlen(filePath)], "/bgp_stats");
	fbgpStats = fopen(filePath, "a");
	log_info(fbgpStats, "\n ****BGP started: custID: %d, server:%s", 
			jsonData->custID, jsonData->serverIP);

	sprintf(filePath, "/var/monT/");
	sprintf(&filePath[strlen("/var/monT/")], "%d", jsonData->custID);
	sprintf(&filePath[strlen(filePath)], "/bgp_logs");
	fp = fopen(filePath, "a");
	log_info(fp, "**  BGP started: custID: %d, server:%s **", 
			jsonData->custID, jsonData->serverIP);


	bgp_main(jsonData, fbgpStats, fp);

	// TBD: For now use this to ensure that the listener runs and is 
	// waiting for pkts
	printf("\n Tool Started : Running BGP "); fflush(stdout);
	//mainShell();
	while(1) {
		sleep(5); 
		continue;
	}

	fclose(fp);
	fclose(fbgpStats);
	return 0;
}


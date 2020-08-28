#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "transfer.h"

void transport(FILE *filePointer, int sockfd);

ssize_t total=0;

int main(int argc, char* argv[])
{
//Client part,handle to socket module as sender part.(From it document)
    if (argc != 3) 
    {
        perror("[-] Usage:send_file filepath <IPaddress>");
        exit(1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("[-] system can't allocate sockfd");
        exit(1);
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_port = htons(port);
    serveraddr.sin_family = AF_INET;
	
    if (inet_pton(AF_INET, argv[2], &serveraddr.sin_addr) < 0){
        perror("[-] Invalid Ip Address.");
        exit(1);}

    if (connect(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0){
        perror("[-] Connection Error");
        exit(1);}
    
    char *filename = basename(argv[1]); 
    if (filename == NULL){
        perror("[-] File not found");
        exit(1);}
    char buff[BUFFSIZE] = {0};
    strncpy(buff, filename, strlen(filename));
    if (send(sockfd, buff, BUFFSIZE, 0) == -1){
        perror("[-] system can't sent file");
        exit(1);}
    
	//Get filename , open.
    FILE *filePointer = fopen(argv[1], "rb");
    if (filePointer == NULL) {
        perror("[-] File Not Found,Pleast try again");
        exit(1); }

    transport(filePointer, sockfd);
    printf("[+] Send Success\nNumBytes = %ld\n", total);
    fclose(filePointer);
    close(sockfd);
    return 0;
}
//Handling with file name's size ,sending.
void transport(FILE *filePointer, int sockfd) {
    int n; 
    char sendline[MAX_LINE] = {0}; 
    while ((n = fread(sendline, sizeof(char), MAX_LINE, filePointer)) > 0) {
	    total = total + n ;
        if (n != MAX_LINE && ferror(filePointer)){
            perror("[-] Error : can't read file");
            exit(1);}
        if (send(sockfd, sendline, n, 0) == -1){
            perror("[-] system can't send file");
            exit(1);}
        memset(sendline, 0, MAX_LINE);}
}

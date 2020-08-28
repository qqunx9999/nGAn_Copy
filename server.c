#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "transfer.h"

void writefile(int sockfd, FILE *filePointer);
ssize_t total=0;

int main(int argc, char *argv[]) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
//Server : allowcate path ,handle with all of possible case with socket module.
    if (sockfd == -1) {
        perror("[-] Error : Can't allocate");
        exit(1);}
    
    struct sockaddr_in clientaddr, serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) == -1) {
        perror("[-] Error : system can't bind.");
        exit(1);}

    if (listen(sockfd, l_port) == -1) {
        perror("[-] Error : server can listen nothing.");
        exit(1);}

    socklen_t addrlen = sizeof(clientaddr);
    int cfd = accept(sockfd, (struct sockaddr *) &clientaddr, &addrlen);
	
    if (cfd == -1) {
        perror("[-] Error : Connection failed.");
        exit(1);}
	
    close(sockfd); 
    char filename[BUFFSIZE] = {0}; 

	
    if (recv(cfd, filename, BUFFSIZE, 0) == -1) {
        perror("[-] Error : Can't get file name.");
        exit(1);}

	//File Part
    FILE *filePointer = fopen(filename, "wb");
    if (filePointer == NULL) {
        perror("[-] Error : can not open file");
        exit(1);}
    
    char addr[INET_ADDRSTRLEN];
    printf("[+] Start receive file: %s from %s\n", filename, inet_ntop(AF_INET, &clientaddr.sin_addr, addr, INET_ADDRSTRLEN));
    writefile(cfd, filePointer);
    printf("[+] Receive Success, NumBytes = %ld\n", total);
    

    fclose(filePointer);
    close(cfd);
    return 0;}

void writefile(int sockfd, FILE *filePointer){
    ssize_t n;
    char buff[MAX_LINE] = {0};
    while ((n = recv(sockfd, buff, MAX_LINE, 0)) > 0) {
	    total = total + n;
        if (n == -1){
            perror("[-] Error : Receive File Error");
            exit(1);}
        
        if (fwrite(buff, sizeof(char), n, filePointer) != n){
            perror("[-] Error : write file failed.");
            exit(1);}
        memset(buff, 0, MAX_LINE);}}

#include <stdio.h>
#include <fcntl.h>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "md5_hash/hashlibpp.h"
#define SERVERPORT "9801"
#define IP_ADDR "localhost"
const int NUMBYTES_PER_REQUEST = 1448;
using namespace std;
string constructRequest(int offset, int numBytes)
{
    string s = "";
    s += "Offset: ";
    s += to_string(offset*numBytes);
    s += "\n";
    s += "NumBytes: ";
    s += to_string(numBytes);
    s += "\n\n";
    return s;
}
std::pair<string, int> parseOutput(string s)
{
    int index = 0;
    if(s[0]=='E')
    {
        return make_pair("", -1);
    }
    int offsetValue = 0;
    while(s[index]!='\n' && index<s.size())
    {
        if(s[index]>='0' && s[index]<='9')
        {
            offsetValue = offsetValue*10 + s[index] - '0';
        }
        index++;
    }
    index++;
    while(s[index]!='\n' && index<s.size())
    {
        index++;
    }
    index++;
    index++;
    if(index>=s.size())
    {
        return make_pair("", -1);
    }
    string lineData = s.substr(index, min(1448, int(s.size())-index));
    return make_pair(lineData, offsetValue);
}
int extractSize(string s)
{
    cout<<"in ext size, s is "<<s<<endl;
    int i = 0;
    int size = 0;
    while(i<s.size())
    {
        if(s[i]!='\n' && s[i]>='0' && s[i]<='9')
        {
            size = size*10 + (s[i] - '0');
        }
        i++;
    }
    return size;
}
int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(IP_ADDR, SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    string fileValue = "";
// loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }
    connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    char buf[1000 + NUMBYTES_PER_REQUEST];
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    addr_len = sizeof their_addr;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(sockfd, &writefds);
    struct timeval timeOutSet;
    timeOutSet.tv_sec = 0;
    timeOutSet.tv_usec=5000;
    int x = select(sockfd+1, NULL, &writefds,  NULL, &timeOutSet);
    cout<<FD_ISSET(sockfd, &writefds)<<endl;
    numbytes = send(sockfd, "SendSize\n\n", strlen("SendSize\n\n"), 0); 
    //cout<<numbytes<<endl;           
    //cout<<"data sent successfully sockfd is "<<sockfd<<endl;
    if (numbytes == -1) {
        perror("talker: sendto");
        exit(1);
    }
    //numbytes = recvfrom(sockfd, buf, 100 , 0,(struct sockaddr *)&their_a
    while (true)
    {
        numbytes = send(sockfd, "SendSize\n\n", strlen("SendSize\n\n"), 0); 
        FD_SET(sockfd, &readfds);
        x = select(sockfd+1, &readfds, NULL, NULL, &timeOutSet);
        if(FD_ISSET(sockfd, &readfds))
        {
            numbytes = recv(sockfd, buf, 100, 0);
            break;
            cout<<buf<<endl;
        }
    }
    x = select(sockfd+1, &readfds, NULL, NULL, &timeOutSet);
    if(FD_ISSET(sockfd, &readfds))
    {
        numbytes = recv(sockfd, buf, 100, 0);
        cout<<buf<<endl;
    }
    // numbytes = recvfrom(sockfd, buf, 100 , 0,(struct sockaddr *)&their_addr, &addr_len);
    if (numbytes == -1) {
        perror("recvfrom");
        exit(1);
    }
    cout<<"Received "<<buf<<endl;
    int fileSize = extractSize(buf);
    cout<<"File size is "<<fileSize<<endl;
    int numPackets = (fileSize + NUMBYTES_PER_REQUEST - 1)/NUMBYTES_PER_REQUEST;
    cout<<"The number of packets we need to send for are "<<numPackets<<'\n';
    set<int> packetsLeft;
    for(int i = 0; i<numPackets; i++)
    {
        packetsLeft.insert(i);
    }
    int reclines = 0;
    vector<string> packetContents(numPackets);
    while(!packetsLeft.empty())
    {
        usleep(5000);
        int sendFor = *packetsLeft.begin();
        //cout<<"sendFor is "<<sendFor<<'\n';
        string s = constructRequest(sendFor, NUMBYTES_PER_REQUEST);
        //cout<<s.c_str()<<endl;
        int x = select(sockfd+1,NULL, &writefds, NULL, &timeOutSet);
        numbytes = send(sockfd, s.c_str(), s.size(), 0);
        if (numbytes == -1) {
            perror("talker: sendto");
            exit(1);
        }
        //cout<<"request sent successfully"<<endl;
        char buf[NUMBYTES_PER_REQUEST + 1000];
        FD_SET(sockfd, &readfds);
        int xy = select(sockfd+1, &readfds, NULL, NULL, &timeOutSet);
        if(FD_ISSET(sockfd, &readfds))
        {
            numbytes = recv(sockfd, buf, NUMBYTES_PER_REQUEST + 1000 , 0);
            reclines++;
        }
        else
        {
            //cout<<"socket not ready to read \n, currently received "<<reclines<<endl;
            continue;
        }
        //cout<<buf<<endl;
        std::pair<string, int> lineData = parseOutput(buf);
        if(lineData.first=="" || lineData.second!=NUMBYTES_PER_REQUEST*sendFor)
        {
            continue;
        }
        // if(lineData.first=="")
        // {
        //     continue;
        // }
        //cout<<"successfully received packet with offset "<<sendFor<<'\n';
        packetContents[sendFor] = lineData.first;
        fileValue += lineData.first;
        //cout<<lineData.first.length()<<endl;
        packetsLeft.erase(packetsLeft.begin());
        //cout<<"packets left are "<<packetsLeft.size()<<endl;
    }

    char valBuf[200];
    //cout<<"Number of packets is "<<numPackets<<endl;
    string s = "SendFile\n\n";
    hashwrapper *myWrapper = new md5wrapper();
    string hashValue = myWrapper->getHashFromString(fileValue);
    string finalSubmission = "Submit: kavya@col334-672\n";
    finalSubmission += "MD5: " + hashValue + "\n\n";
    numbytes = send(sockfd, finalSubmission.c_str(), finalSubmission.size(), 0);
    freeaddrinfo(servinfo);
    //printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
    close(sockfd);
    return 0;
 }
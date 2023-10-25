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
#include <chrono>
#include <fstream> 
#define SERVERPORT "9801"
#define IP_ADDR "localhost"
const int NUMBYTES_PER_REQUEST = 1448;
int SLEEP_TIME = 4000;
int EST_RTT_TIMEOUT = 10000;
const int SELECT_TIMEOUT = 2000;
using namespace std;
int lastSquished = 0;
int numSquished = 0;
// bool parseResult(char* responseBuffer, int size)
// {
//     int index = 0;
//     while(index + )
//     string squished = responseBuffer.substr(index, strlen("Squished\n"));
//     if(squished=="Squished\n")
//     {
//         return true;
//     }
//     return false;
// }
int dynamicRateAdjust(int &sleepTime)
{
    // float sleepTimeFloat = (float)sleepTime;
    // if(lastSquished==1 && numSquished>=100)
    // {
    //     numSquished = 0;
    //     sleepTimeFloat = sleepTimeFloat * 1.1;
    //     cout<<"setting sleep time to "<<sleepTimeFloat<<endl;
    // }
    // else if(lastSquished==0)
    // {
    //     sleepTimeFloat = sleepTimeFloat * 0.9;
    //             cout<<"setting sleep time to "<<sleepTimeFloat<<endl;

    // }
    // sleepTime = (int)sleepTimeFloat;
    return sleepTime;
}
int sendMessage(string s, int socketDescriptor, fd_set& writeDescriptors, struct timeval setTimeOut)
{
    int bytesSent = 0;
    FD_SET(socketDescriptor, &writeDescriptors);
    int x = select(socketDescriptor+1, NULL, &writeDescriptors,  NULL, &setTimeOut);
    if(FD_ISSET(socketDescriptor, &writeDescriptors))
    {
        bytesSent = send(socketDescriptor, s.c_str(), s.size(), 0);
        //cout<<"message sent successfully "<<s<<endl;
        return 1;
    }
    return 0;
}
int readMessage(char* responseBuffer, int lenBuffer, int socketDescriptor, fd_set& readDescriptors, struct timeval setTimeOut)
{
    int bytesReceived = 0;
    FD_SET(socketDescriptor, &readDescriptors);
    int x = select(socketDescriptor+1, &readDescriptors, NULL, NULL, &setTimeOut);
    if(FD_ISSET(socketDescriptor, &readDescriptors))
    {
        bytesReceived = recv(socketDescriptor, responseBuffer, lenBuffer, 0);
        //cout<<"message received successfully"<<endl;
        return 1;
    }
    return 0;
}
int initializeSocket()
{
    int socketDescriptor;
    struct addrinfo connectionInfo;
    struct addrinfo *serverAddressListHead;
    struct addrinfo *serverAddress;
    int returnValue;
    int numbytes;
    memset(&connectionInfo, 0, sizeof connectionInfo);
    connectionInfo.ai_family = AF_INET; 
    connectionInfo.ai_socktype = SOCK_DGRAM;
    if ((returnValue = getaddrinfo(IP_ADDR, SERVERPORT, &connectionInfo, &serverAddressListHead)) != 0) {
        printf("unable to get server address");
        return 1;
    }
    
    for(serverAddress = serverAddressListHead; serverAddress != NULL; serverAddress = serverAddress->ai_next) {
        if ((socketDescriptor = socket(serverAddress->ai_family, serverAddress->ai_socktype,
            serverAddress->ai_protocol)) == -1) {
            continue;
        }
        break;            cout<<"unable to connect"<<'\n';

    }
    if (serverAddress == NULL) {
        cout<<"socket creation failed, terminating program"<<'\n';
        return 1;
    }
    connect(socketDescriptor, serverAddressListHead->ai_addr, serverAddressListHead->ai_addrlen);
    //cout<<"socket creation succeeded"<<'\n';
    freeaddrinfo(serverAddressListHead);
    return socketDescriptor;
}
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
std::pair<bool, std::pair<string, int>> parseOutput(string s)
{
    //cout<<s<<endl;
    bool isSquished = false;
    int index = 0;
    if(s[0]=='E')
    {
        return make_pair(false, make_pair("", -1));
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
    int numbytes = 0;
    while(s[index]!='\n' && index<s.size())
    {
        if(s[index]>='0' && s[index]<='9')
        {
            numbytes = numbytes*10 + s[index] - '0';
        }
        index++;
    }
    index++;
    if(index>=s.size())
    {
        return make_pair(false, make_pair("", -1));
    }
    try{
        string squished = s.substr(index, strlen("Squished\n"));
        //cout<<"index is "<<index<<" s is "<<squished<<endl;
        if(squished=="Squished\n")
        {
            index+=strlen("Squished\n");
            isSquished = true;
        }
    }
    catch(int x)
    {
        ;
    }
    index++;
    string lineData = s.substr(index, min(numbytes, int(s.size())-index));
    //cout<<"lineData is "<<lineData<<endl;
    return make_pair(isSquished, make_pair(lineData, offsetValue));
}
int extractSize(string s)
{
    //cout<<"in ext size, s is "<<s<<endl;vayu.iitd.ac.in
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
    //ofstream packetLogging("log.txt");
    int socketDescriptor = initializeSocket();
    std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();
    char responseBuffer[1000 + NUMBYTES_PER_REQUEST];
    fd_set readDescriptors;
    FD_ZERO(&readDescriptors);
    fd_set writeDescriptors;
    FD_ZERO(&writeDescriptors);
    struct timeval setTimeOut;
    setTimeOut.tv_sec = 0;
    setTimeOut.tv_usec=SELECT_TIMEOUT;
    while (true)
    {
        usleep(SLEEP_TIME*10);
        if(!sendMessage("SendSize\nReset\n\n", socketDescriptor, writeDescriptors, setTimeOut))
        {
            continue;
        }
        if(!readMessage(responseBuffer, 1000 + NUMBYTES_PER_REQUEST, socketDescriptor, readDescriptors, setTimeOut))
        {
            continue;
        }
        else
        {
            //cout<<"responseBuffer: "<<responseBuffer<<endl;
            break;
        }
    }
    readMessage(responseBuffer, socketDescriptor, 1000 + NUMBYTES_PER_REQUEST, readDescriptors, setTimeOut);
    int fileSize = extractSize(responseBuffer);
    int numPackets = (fileSize + NUMBYTES_PER_REQUEST - 1)/NUMBYTES_PER_REQUEST;
    vector<string> packetContents(numPackets);
    string fileValue = "";
    int packetsReceived = 0;
    set<int> notReceived;
    for(int i=0;i<numPackets;i++)
    {
        notReceived.insert(i);
    }
    int sendFor = 0;
    while(!notReceived.empty())
    {
        usleep(dynamicRateAdjust(SLEEP_TIME));
        if(std::lower_bound(notReceived.begin(), notReceived.end(), sendFor) == notReceived.end())
        {
            sendFor = *notReceived.begin();
        }
        else
        {
            sendFor = *std::lower_bound(notReceived.begin(), notReceived.end(), sendFor);
        }
        string requestString = constructRequest(sendFor, NUMBYTES_PER_REQUEST);
        if(!sendMessage(requestString, socketDescriptor, writeDescriptors, setTimeOut))
        {
            continue;
        }
        std::chrono::time_point<std::chrono::system_clock> currTime = std::chrono::system_clock::now();
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(currTime-startTime);
        cout<<"S"<< " " << sendFor*NUMBYTES_PER_REQUEST<<" "<<microseconds.count()<<endl;
        sendFor = (sendFor+1)%numPackets;
        char responseBuffer[NUMBYTES_PER_REQUEST + 1000];
        if(!readMessage(responseBuffer, 1000 + NUMBYTES_PER_REQUEST, socketDescriptor, readDescriptors, setTimeOut))
        {
            continue;
        }
        currTime = std::chrono::system_clock::now();
        microseconds = std::chrono::duration_cast<std::chrono::microseconds>(currTime-startTime);
        std::pair<bool, std::pair<string, int>> response = parseOutput(responseBuffer);
        std::pair<string, int> lineData = response.second;
        lastSquished = response.first;
        numSquished = (numSquished + 1);
        cout<<"R"<< " " << lineData.second<<" "<<microseconds.count()<<endl;
        if(lineData.first=="")
        {
            continue;
        }
        if(notReceived.find(lineData.second/NUMBYTES_PER_REQUEST)!=notReceived.end())
        {
            packetContents[lineData.second/NUMBYTES_PER_REQUEST] = lineData.first;
            notReceived.erase(notReceived.find(lineData.second/NUMBYTES_PER_REQUEST));
            //cout<<"erasing packet "<<lineData.second/NUMBYTES_PER_REQUEST<<endl;
        }
        //cout<<"here"<<endl;
        // packetsReceived++;
    }
    fileValue = "";
    for(int i=0; i<numPackets; i++)
    {
        //cout<<i<<": "<<packetContents[i]<<endl;
        fileValue+=packetContents[i];
    }
    hashwrapper *myWrapper = new md5wrapper();
    string hashValue = myWrapper->getHashFromString(fileValue);
    string finalSubmission = "Submit: kavya@col334-672\n";
    finalSubmission += "MD5: " + hashValue + "\n\n";
    int SubmittedBytes = send(socketDescriptor, finalSubmission.c_str(), finalSubmission.size(), 0);
    // sleep(10);
    int i=0;
    while (i<50)
    {
        int SubmittedBytes = send(socketDescriptor, finalSubmission.c_str(), finalSubmission.size(), 0);
        readMessage(responseBuffer, NUMBYTES_PER_REQUEST + 1000, socketDescriptor, readDescriptors, setTimeOut);
        //cout<<responseBuffer<<endl;
        i+=1;
    }
    
    // int SubmittedBytes = send(socketDescriptor, finalSubmission.c_str(), finalSubmission.size(), 0);
    // readMessage(responseBuffer, NUMBYTES_PER_REQUEST + 1000, socketDescriptor, readDescriptors, setTimeOut);
    // cout<<responseBuffer<<endl;
    // readMessage(responseBuffer, NUMBYTES_PER_REQUEST + 1000, socketDescriptor, readDescriptors, setTimeOut);
    // cout<<responseBuffer<<endl;
    // readMessage(responseBuffer, NUMBYTES_PER_REQUEST + 1000, socketDescriptor, readDescriptors, setTimeOut);
    // cout<<responseBuffer<<endl;
    // readMessage(responseBuffer, NUMBYTES_PER_REQUEST + 1000, socketDescriptor, readDescriptors, setTimeOut);
    // cout<<responseBuffer<<endl;
    close(socketDescriptor);
    return 0;
 }
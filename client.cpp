// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <sstream>
#include <stdlib.h>
#include <netdb.h>

using namespace std; 

int main(int argc, char const *argv[]) {
	
	int client_fd = 0, valread1 = 0,valread2 = 0, status = 0; 
	struct addrinfo hints;
	struct addrinfo * res, *p;

	if (argc != 2){
		printf(" Hostname missing !\n");
		printf(" Usage: ./client localhost\n");
		return -1;
	}

	// fill up struct 'hints'
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	const char * USC_Id = "5351873181"; 
	char buffer[1024] = {0}; 
	char last3Digits[3];  
	int lastDigits = 0;
	memcpy(last3Digits,&USC_Id[7],3);

	stringstream ss(last3Digits);
	ss >> lastDigits;
	int transactionId = lastDigits % 255;
	int servPortToConnectTo = 4000 + lastDigits;

	stringstream z;
	z << servPortToConnectTo;
	const char * port = (z.str()).c_str();
	
	if((status = getaddrinfo(argv[1],port, &hints, & res)) != 0){
		printf("Error in getting address\n");
		return -1;
	}

	if ((client_fd = socket(res -> ai_family, res -> ai_socktype, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	}

	printf("client: connecting to 127.0.0.1");
	if (connect(client_fd, res -> ai_addr, res -> ai_addrlen) < 0)
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 

	freeaddrinfo(res);

	// Sending data during Discovery Phase ... 
	stringstream s;
	s << transactionId;
	const char * strTrnscnDiscoverID = (s.str()).c_str();

	printf("\nSending the following Transaction ID to server: %s\n",strTrnscnDiscoverID); 

	if(send(client_fd , strTrnscnDiscoverID , strlen(strTrnscnDiscoverID) , 0) < 0){
		perror("Send failed");
		exit(EXIT_FAILURE); 
	}


	// Receiving data during Offer Phase ...
	if((valread1 = recv(client_fd , buffer, 1024, 0)) <= 0){
		if(valread1 == 0){
			printf("Remote connection closed !\n");
		}
		else {
			perror("Receive failed");
			exit(EXIT_FAILURE); 
		}
	} 

	char * ptr = strrchr(buffer,'.');
	for(int i = 1; i <= 4 ; i++){
		ptr++;
	}

	char IPreceived [strlen(buffer) - strlen(ptr)];
	memcpy(IPreceived,buffer,strlen(buffer) - strlen(ptr));

	printf("Received the following:\n");
	printf("IP address: %s\n",IPreceived);
	printf("Transaction ID: %s\n",ptr); 


	// Sending data during Request Phase ...
	srand(time(0));
	int transctnRequestID = rand() % 256;
	stringstream reqID;
	reqID << transctnRequestID;
	const char * strTrnscnReqID = (reqID.str()).c_str();

	printf("Formally requesting the following server:\n");
	printf("IP address: %s\n",IPreceived);
	printf("Transaction ID: %s\n",strTrnscnReqID);

	if(send(client_fd, strTrnscnReqID, strlen(strTrnscnReqID), 0) < 0){
		perror("Send failed");
		exit(EXIT_FAILURE); 
	}


	// Receiving data during Acknowledgement Phase ...
	char bufferAck[1024] = {0};
	if((valread2 = recv(client_fd, bufferAck, 1024, 0)) <= 0){
		if(valread2 == 0){
			printf("Remote connection closed !\n");
		}
		else {
			perror("Receive failed");
			exit(EXIT_FAILURE); 
		}
	}

	printf("Officially connected to IP Address: %s\n",IPreceived);
	printf("client: received '%s'\n", bufferAck);


	// Closing the client socket
	close(client_fd);

	return 0; 
}
// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>           
#include <stdio.h>          
#include <sys/socket.h>    
#include <stdlib.h>       
#include <netinet/in.h>  
#include <string.h>
#include <sstream>
#include <sys/types.h>
#include <netdb.h> 

using namespace std;

int main(int argc, char const *argv[]) {

	int server_fd = 0, child_socket = 0;
	int valread1 = 0,valread2 = 0, status = 0;
	int opt = 1;

	struct addrinfo hints;
	struct addrinfo * res, *p;
	
	char buffer[1024] = {0};

	// fill up struct 'hints'
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;   
	
	const char * USC_Id = "5351873181";
	char last3Digits[3];                     
	memcpy(last3Digits,&USC_Id[7],3);
	int lastDigits = 0;
	stringstream ss(last3Digits);
	ss >> lastDigits;

	int serverPort = 4000 + lastDigits;

	stringstream z;
	z << serverPort;
	const char * port = (z.str()).c_str();
	
	if((status = getaddrinfo(NULL,port, &hints, &res)) != 0){  
		printf("Error in getting address\n");
		return -1;
	}

	// Creating socket file descriptor
	if ((server_fd = socket(res -> ai_family, res -> ai_socktype, 0)) < 0)
	{
		perror("Socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
	&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	
	// Forcefully attaching socket to the calculated port '4181'
	if (bind(server_fd, res -> ai_addr, res -> ai_addrlen) < 0)
	{
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(res);

	printf("server: waiting for connections...\n");
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_storage their_addr;
	socklen_t addr_size;

	if ((child_socket = accept(server_fd, (struct sockaddr *)&their_addr, &addr_size)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}


    // Receiving data during Discovery Phase ... 
	printf("server: got connection from 127.0.0.1\n");
	if((valread1 = recv( child_socket , buffer, 1024, 0)) <=0 ){
		if(valread1 == 0){
				printf("Remote connection closed !\n");
		}
		else {
			perror("Receive failed");
			exit(EXIT_FAILURE); 
		}
	}

	printf("Received the following Transaction ID from client: %s\n", buffer);


	// Sending data during Offer Phase ...
	srand(time(0));
	int IPadd[3];
	string IPOffer = "";
	for(int i = 0; i < 3; i++){
	   IPadd[i] = rand() % 256;
	   stringstream a;
	   a << IPadd[i];
	   IPOffer = IPOffer + a.str() + ".";
	}

	IPOffer = IPOffer + buffer;
	const char * str = IPOffer.c_str();

	int transctnOfferID = rand() % 256;
	stringstream s;
	s << transctnOfferID;
	const char * strTrnscnOfferID = (s.str()).c_str();

	printf("Sending the following to Client:\n");
	printf("IP address: %s\n", str);

	if(send(child_socket , str , strlen(str) , 0 ) < 0){
		perror("send failed");
		exit(EXIT_FAILURE); 
	}

	printf("Transaction ID: %s\n", strTrnscnOfferID);

	if(send(child_socket , strTrnscnOfferID , strlen(strTrnscnOfferID) , 0) < 0){
		perror("send failed");
		exit(EXIT_FAILURE); 
	}


	// Receiving data dusring Request phase ...
	char bufferRequest[1024] = {0};

	if((valread2 = recv(child_socket , bufferRequest, 1024, 0)) <=0 ){
		if(valread2 == 0){
			printf("Remote connection closed !\n");
		}
		else {
			perror("Receive failed");
			exit(EXIT_FAILURE); 
		}
	}

	printf("Received the following request:\n");
	printf("Transaction ID: %s\n", bufferRequest);


    // Sending data during Acknowledgement Phase ...
	int transctnAckID = rand() % 256;
	stringstream b;
	b << transctnAckID;
	const char * strTrnscnAckID = (b.str()).c_str();

	printf("Sending following acknowledgment:\n");
	printf("IP address: %s\n",str );
	printf("Transaction ID: %s\n",strTrnscnAckID);

	if(send(child_socket, strTrnscnAckID, strlen(strTrnscnAckID), 0) < 0){
		perror("Send failed");
		exit(EXIT_FAILURE); 
	}


	// Closing the child socket only
	close(child_socket);

	return 0;
}
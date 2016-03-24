#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h> 

#include <jansson.h>

#define PORT 5012

int sessionState(char buffer[]);
void clientHandler(int sock);

int main(int argc, char const *argv[]){
	/*Variables servidor*/
	int serverSockfd , clientSockfd, clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int pid;


	serverSockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (serverSockfd < 0){
		perror("ERROR opening socket");
      exit(1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
   	serv_addr.sin_addr.s_addr = INADDR_ANY;
   	serv_addr.sin_port = htons(PORT);

   	if (bind(serverSockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("Error binding");
      exit(1);
   }

   listen(serverSockfd,5);
   clilen = sizeof(cli_addr);

   while(1){

   		clientSockfd = accept(serverSockfd,(struct sockaddr *) &cli_addr, &clilen);

   		if (clientSockfd < 0){
   			perror("Error aceptando");
   			exit(1);
   		}

   		pid = fork();

   		if (pid == 0)
   		{
   			close(serverSockfd);
   			clientHandler(clientSockfd);

   		}else{
   			close(clientSockfd);
   		}
   }

	return 0;
}


void clientHandler(int sock){
	int n,activeSession = 0;
	char buffer[256];
	char Exit[] = "exit";


	while(1){
		bzero(buffer,256);
		n = read(sock, buffer, 255);
		if (n < 0){
			perror("Error leyendo del socket");
			exit(1);
		}

		if (activeSession == 1){
			printf("Session activa\n");
			exit(1);
		}else{
			write(sock,"{\"error\":{\"message\":\"Unauthorized\",\"code\":401}}",47);
			activeSession = sessionState(buffer);
		}	
	}

}

int sessionState(char *buffer){
	json_t *user;
	json_error_t error;

	user = json_loads(buffer,0,&error);

	if(!json_is_array(user)){
    	fprintf(stderr, "error: root is not an array\n");
    	json_decref(user);
    	return 1;
	}else{
		printf("Lo que enviaste si es un JSON\n" );
	}

	return 1;
}









#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h> 

#include <jansson.h>
#include <uuid/uuid.h>
#include "error.c"


#define PORT 5012

int sessionState(char buffer[]);
void clientHandler(int sock);
int login(json_t user);
int signup(json_t user);
int chekcToken(json_t user);
void sessionError(int sock, int error);


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

   		clientSockfd = accept(serverSockfd,(struct sockaddr *) &cli_addr,(socklen_t *) &clilen);

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
	json_t *data;
	json_error_t error;

	while(1){
		bzero(buffer,256);
		n = read(sock, buffer, 255);
		if (n < 0){
			perror("Error leyendo del socket");
			exit(1);
		}

		if (activeSession == 1){
			data = json_loads((const char *)data,0,&error);
			if (!data){
				write(sock,ERROR_BAD_REQUEST,sizeof(ERROR_BAD_REQUEST));
			}else{

			}
			exit(1);
		}else{
			write(sock, ERROR_UNAUTHORIZED, sizeof(ERROR_UNAUTHORIZED));
			activeSession = sessionState(buffer);
			if (activeSession == 1){
				/*Generar token*/

			}else{
				/*Espeficicar error*/
				sessionError(sock,activeSession);
			}
		}	
	}

}

int sessionState(char *data){
	json_t *user;
	json_error_t error;
	json_int_t instruction;
	
	printf("%s\n",data );
	user = json_loads(data,0,&error);

	if(!user){
	    fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
	    return 0;
	}

	if(!json_is_object(user)){
    	fprintf(stderr, "error: root is not an object\n");
    	json_decref(user);
    	return 0;
	}

	instruction = json_integer_value(json_object_get(user, "instruction"));

	switch(instruction){
		case 0:	/*Login*/
			return login(*user);
		break;
		case 1:	/*SignUp*/
			return signup(*user);
		break;
		case 2:	/*Check Token*/
			return chekcToken(*user);
		break;
		default:
			return 0;
		break;
	}
	return 1;
}


int login(json_t user){
	/*Comprobar*/

	/*Errores
		Npi Error -1
		Usuario o contraseña invalida 201
	*/
	return 1;
}
int signup(json_t user){
	/*Insertar usuario*/
	/*Errores
		Npi Error -1
		Ya existe el Usuario 202

	*/
	return 1;
}
int chekcToken(json_t user){
	/*Checar Tocken*/
	/*Errores
		Npi Error -1
		Token invalido 209
	*/
	return 1;
}


void sessionError(int sock, int error){
	switch(error){
		case -1:	//Error desconocido
			write(sock, ERROR_UNKNOW, sizeof(ERROR_UNKNOW));
		break;
		case 201:	//Usuario o contraseña invalida 201
			write(sock, ERROR_FAILED_LOGIN, sizeof(ERROR_FAILED_LOGIN));
		break;
		case 202:	//Ya existe el Usuario 202
			write(sock, ERROR_USERNAME_TAKEN, sizeof(ERROR_USERNAME_TAKEN));
		break;
		case 209:	//Token invalido 209
			write(sock, ERROR_INVALID_TOKEN, sizeof(ERROR_INVALID_TOKEN));
		break;
		default:
			write(sock, ERROR_UNKNOW, sizeof(ERROR_UNKNOW));
		break;
	}
}






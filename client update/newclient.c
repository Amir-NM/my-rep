#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define MAX 5000
#define MAXMAX 5000
#define PORT 12345
#define SA struct sockaddr

int client_socket, server_socket;
struct sockaddr_in servaddr, cli;
char AuthToken[50], channel[21];

int make_sucket();
int Accountmenu();
int Login();
int Register();
int Mainmenu();
int Createchannel();
int Joinchannel();
int Logout();
int Chatmenu();
int Sendmessage();
int Refresh();
int Channelmembers();
int Leavechannel();
char * Strjson();
void Array1json();
void Array2json();

int main() {
	printf("Welcome!!!\n\n");
	int n = 1;
	while(1) {
		if(n == 13) {
			break;
		}
		switch(n) {
			case 1 :
				printf("_______________________________________________________________________________________________________________\n");
				n = Accountmenu();
				break;
			case 2 :
				printf("\n");
				n = Register();
				break;
			case 3 :
				printf("\n");
				n = Login();
				break;
			case 4 :
				printf("_______________________________________________________________________________________________________________\n");
				n = Mainmenu();
				break;
			case 5 :
                                printf("\n");
                                n = Createchannel();
                                break;
                        case 6 :
                                printf("\n");
                                n = Joinchannel();
                                break;
                        case 7 :
                                printf("\n");
                                n = Logout();
                                break;
                        case 8 :
                                printf("_______________________________________________________________________________________________________________\n");
                                n = Chatmenu();
                                break;
                        case 9 :
                                printf("\n");
                                n = Sendmessage();
                                break;
                        case 10 :
                                printf("\n");
                                n = Refresh();
                                break;
                        case 11 :
                                printf("\n");
                                n = Channelmembers();
                                break;
                        case 12 :
                                printf("\n");
                                n = Leavechannel();
                                break;

	}
		}
	return 0;
}

int make_sucket() {
	printf("\n");
	// Create and verify socket
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("Socket creation failed...\n");
		return 0;
	}
	else
		printf("Socket successfully created..\n");

	// Assign IP and port
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// Connect the client socket to server socket
	if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection to the server failed...\n");
		return 0;
	}
	else
		printf("Successfully connected to the server..\n");
	return 1;
}

int Accountmenu() {
	printf("ACCOUNT MENU\n\nIn order to :\n\nLogin enter 1\nRegister enter 2\nExit enter 0\n\n");
	int choice;
	scanf("%d", &choice);
	switch (choice) {
		case 1 :
			return 3;
		case 2 :
			return 2;
		case 0 :
			return 13;
	}
}

int Login() {
	char user_name[21];
	char password[11];
	printf("To login\n\nEnter your user name :\n");
	scanf("%s", user_name);
	printf("\nEnter your password :\n");
	scanf("%s", password);
	char buffer[MAX] = "login ";
	strcat(buffer, user_name);
	strcat(buffer, ", ");
	strcat(buffer, password);
	strcat(buffer, "\n");
	if (make_sucket() == 1) {
		send(client_socket, buffer, sizeof(buffer), 0);
		bzero(buffer, sizeof(buffer));
		recv(client_socket, buffer, sizeof(buffer), 0);
		shutdown(client_socket, SHUT_RDWR);
	}
	else {
		return 1;
	}
	printf("\n");
	char jsonstr[MAX];
	strcpy(jsonstr, buffer);
    char *type = Strjson(jsonstr, "type");
	strcpy(jsonstr, buffer);
    char *content = Strjson(jsonstr, "content");
	if(*type == 'E') {
		printf("%s\n", content);
		return 1;
	}
	printf("You are successfully logged in!!!\n");
	bzero(AuthToken, sizeof(AuthToken));
	strcpy(AuthToken, content);
printf("%s\n%s\n", type, content);
	return 4;
}

int Register() {
        char user_name[21];
        char password[11];
        printf("To register\n\nEnter your user name (The maximum is 20 characters) :\n");
        scanf("%s", user_name);
        printf("\nEnter your password (The maximum is 10 characters) :\n");
        scanf("%s", password);
        char buffer[MAX] = "register ";
        strcat(buffer, user_name);
        strcat(buffer, ", ");
        strcat(buffer, password);
        strcat(buffer, "\n");
        if (make_sucket() == 1) {
                send(client_socket, buffer, sizeof(buffer), 0);
                bzero(buffer, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                shutdown(client_socket, SHUT_RDWR);
        }
	else {
		return 1;
	}
	printf("\n");
	char jsonstr[MAX];
	strcpy(jsonstr, buffer);
	char *type = Strjson(jsonstr, "type");
	strcpy(jsonstr, buffer);
	char *content = Strjson(jsonstr, "content");
	switch (*type) {
		case 'S' :
			printf("Congratulations!!! You are successfully registered.\n");
			break;
		case 'E' :
			printf("Unfotunatly, %s\n", content);
	}
	return 1;
}

int Mainmenu() {
	printf("MAIN MENU\n\nIn order to:\n\nCreate a channel enter 1\nJoin a channel enter 2\nLogout enter 0\n\n");
	int choice;
	scanf("%d", & choice);
	switch(choice) {
		case 1 :
			return 5;
		case 2 :
			return 6;
		case 0 :
			return 7;
	}
}

int Createchannel() {
	printf("To create a channel :\n\nPlease enter your channels name (The maximum is 20 characters) :\n");
	char channelname[21];
	scanf("%s", channelname);
	char buffer[MAX] = "create channel ";
	strcat(buffer, channelname);
	strcat(buffer, ", ");
	strcat(buffer, AuthToken);
	strcat(buffer, "\n");
	if (make_sucket() == 1) {
                send(client_socket, buffer, sizeof(buffer), 0);
                bzero(buffer, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                shutdown(client_socket, SHUT_RDWR);
        }
        else {
                return 4;
        }
        printf("\n");
        char jsonstr[MAX];
		strcpy(jsonstr, buffer);
	    char *type = Strjson(jsonstr, "type");
		strcpy(jsonstr, buffer);
    	char *content = Strjson(jsonstr, "content");
	if (*(type) == 'S') {
		printf("Your channel is successfully created!!!\n");
		strcpy(channel, channelname);
		return 8;
	}
		printf("%s\n", content);

		return 4;
}

int Joinchannel() {
	printf("To join a channel :\n\nPlease enter the channels name :\n");
	char channelname[21];
        scanf("%s", channelname);
        char buffer[MAX] = "join channel ";
        strcat(buffer, channelname);
        strcat(buffer, ", ");
        strcat(buffer, AuthToken);
        strcat(buffer, "\n");
	if (make_sucket() == 1) {
                send(client_socket, buffer, sizeof(buffer), 0);
                bzero(buffer, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                shutdown(client_socket, SHUT_RDWR);
        }
        else {
                return 4;
        }
        printf("\n");
        char jsonstr[MAX];
		strcpy(jsonstr, buffer);
    	char *type = Strjson(jsonstr, "type");
		strcpy(jsonstr, buffer);
    	char *content = Strjson(jsonstr, "content");
        if (*(type) == 'S') {
                printf("You have successfully joined the channel!!!\n");
                strcpy(channel, channelname);
                return 8;
        }
        printf("%s\n", content);
        return 4;
}

int Logout() {
	char buffer[MAX] = "logout ";
        strcat(buffer, AuthToken);
	strcat(buffer, "\n");
	if (make_sucket() == 1) {
                send(client_socket, buffer, sizeof(buffer), 0);
                bzero(buffer, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                shutdown(client_socket, SHUT_RDWR);
        }
        else {
                return 4;
        }
        printf("\n");
        printf("You are successfully loged out.\n");
        return 1;
}

int Chatmenu() {
	printf("CHAT MENU\n\nYour channel's name is %s\n\nIn order to :\n\nSend a message enter 1\nRefresh enter 2\nSee the channel members enter 3\nLeave the channel enter 0\n\n", channel);
	int choice;
	scanf("%d", &choice);
	switch (choice) {
		case 1 :
			return 9;
		case 2:
			return 10;
		case 3:
			return 11;
		case 0:
			return 12;
	}
}

int Sendmessage() {
	printf("To send message :\n\nPlease enter your message (maximum is 120 characters) :\n");
	char message[MAX], b;
	int i=0;
	getchar();
	b = getchar();
	while(b != '\n') {
		message[i] = b;
		b = getchar();
		i++;
	}
	char buffer[MAXMAX] = "send ";
        strcat(buffer, message);
        strcat(buffer, ", ");
        strcat(buffer, AuthToken);
        strcat(buffer, "\n");
        if (make_sucket() == 1) {
                send(client_socket, buffer, sizeof(buffer), 0);
                bzero(buffer, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                shutdown(client_socket, SHUT_RDWR);
        }
        else {
                return 8;
        }
        printf("\n");
	char jsonstr[MAX];
	strcpy(jsonstr, buffer);
    char *type = Strjson(jsonstr, "type");
	strcpy(jsonstr, buffer);
    char *content = Strjson(jsonstr, "content");
	if(*(type) == 'S')
		printf("Your message is successfully sent!!!\n");
	else
		printf("%s\n", content);
	return 8;
}

int Refresh() {
	printf("Refreshing...\n");
	char buffer[MAXMAX] = "refresh ";
        strcat(buffer, AuthToken);
        strcat(buffer, "\n");
        if (make_sucket() == 1) {
                send(client_socket, buffer, sizeof(buffer), 0);
                bzero(buffer, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                shutdown(client_socket, SHUT_RDWR);
        }
        else {
                return 8;
        }
	printf("\n");
	char jsonstr[MAX];
	strcpy(jsonstr, buffer);
    char *type = Strjson(jsonstr, "type");
	strcpy(jsonstr, buffer);
	if(*(type) == 'L') {
		Array1json(jsonstr);
	}
	else{
		char *content = Strjson(jsonstr, "content");
		printf("%s\n", content);
	}
	return 8;
}

int Channelmembers() {
	printf("Geting channel members...\n");
	char buffer[MAX] = "channel members ";
	strcat(buffer, AuthToken);
        strcat(buffer, "\n");
        if (make_sucket() == 1) {
                send(client_socket, buffer, sizeof(buffer), 0);
                bzero(buffer, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                shutdown(client_socket, SHUT_RDWR);
        }
        else {
                return 8;
        }
        printf("\n");
        char jsonstr[MAX];
	strcpy(jsonstr, buffer);
    char *type = Strjson(jsonstr, "type");
	strcpy(jsonstr, buffer);
	if(*(type) == 'L') {
        Array2json(jsonstr);
	}
	else {
		char *content = Strjson(jsonstr, "content");
		printf("%s\n", content);
	}
	return 8;
}

int Leavechannel() {
	char buffer[MAX] = "leave ";
        strcat(buffer, AuthToken);
        strcat(buffer, "\n");
        if (make_sucket() == 1) {
                send(client_socket, buffer, sizeof(buffer), 0);
                bzero(buffer, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);
                shutdown(client_socket, SHUT_RDWR);
        }
        else {
                return 8;
        }
        printf("\n");
        char jsonstr[MAX];
		strcpy(jsonstr, buffer);
    	char *type = Strjson(jsonstr, "type");
		strcpy(jsonstr, buffer);
    	char *content = Strjson(jsonstr, "content");
        printf("You successfully left the channel.\n");
        return 4;
}

char * Strjson(char *str, const char *b) {
    char *x = (char *)malloc(100);
    x = strtok(str, "\"");
    x = strtok(NULL, "\"");
    while(strcmp(x, b) != 0) {
        x = strtok(NULL, "\"");
        x = strtok(NULL, "\"");
        x = strtok(NULL, "\"");
        x = strtok(NULL, "\"");
    }
    x = strtok(NULL, "\"");
    x = strtok(NULL, "\"");
    return x;
}

void Array1json(char *str) {
    str = strtok(str, "[");
    str = strtok(NULL, "]");
    char *x = strtok(str, "\"");
    x = strtok(NULL, "\"");
    while (x != NULL){
        x = strtok(NULL, "\"");
        x = strtok(NULL, "\"");
        printf("%s : ", x);
        x = strtok(NULL, "\"");
        x = strtok(NULL, "\"");
        x = strtok(NULL, "\"");
        x = strtok(NULL, "\"");
        printf("%s\n", x);
        x = strtok(NULL, "\"");
        x = strtok(NULL, "\"");
    }
}

void Array2json(char *str) {
    str = strtok(str, "[");
    str = strtok(NULL, "]");
    char *x = strtok(str, "\"");
    while(x != NULL) {
        printf("%s\n", x);
        x = strtok(NULL, "\"");
        x = strtok(NULL, "\"");
    }
}

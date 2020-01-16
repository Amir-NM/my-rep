#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "cJSON.h"
#define MAX 121
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
        const cJSON *type;
        const cJSON *content;
        cJSON *cjson_str = cJSON_Parse(buffer);
        type = cJSON_GetObjectItemCaseSensitive(cjson_str, "type");
        content = cJSON_GetObjectItemCaseSensitive(cjson_str, "content");
	if(*(type -> valuestring) == 'E') {
		printf("%s\n", content -> valuestring);
		cJSON_Delete(cjson_str);
		return 1;
	}
	printf("You are successfully logged in!!!\n");
	bzero(AuthToken, sizeof(AuthToken));
	strcpy(AuthToken, content -> valuestring);
	cJSON_Delete(cjson_str);
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
        const cJSON *type;
        const cJSON *content;
        cJSON *cjson_str = cJSON_Parse(buffer);
        type = cJSON_GetObjectItemCaseSensitive(cjson_str, "type");
        content = cJSON_GetObjectItemCaseSensitive(cjson_str, "content");
	switch (*(type -> valuestring)) {
		case 'S' :
			printf("Congratulations!!! You are successfully registered.\n");
			break;
		case 'E' :
			printf("Unfotunatly, %s\n", content -> valuestring);
	}
	cJSON_Delete(cjson_str);
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
        const cJSON *type;
        const cJSON *content;
        cJSON *cjson_str = cJSON_Parse(buffer);
        type = cJSON_GetObjectItemCaseSensitive(cjson_str, "type");
        content = cJSON_GetObjectItemCaseSensitive(cjson_str, "content");
	if (*(type -> valuestring) == 'S') {
		printf("Your channel is successfully created!!!\n");
		strcpy(channel, channelname);
		return 8;
	}
		printf("%s\n", content -> valuestring);
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
        const cJSON *type;
        const cJSON *content;
        cJSON *cjson_str = cJSON_Parse(buffer);
        type = cJSON_GetObjectItemCaseSensitive(cjson_str, "type");
        content = cJSON_GetObjectItemCaseSensitive(cjson_str, "content");
        if (*(type -> valuestring) == 'S') {
                printf("You have successfully joined the channel!!!\n");
                strcpy(channel, channelname);
                return 8;
        }
        printf("%s\n", content -> valuestring);
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
        const cJSON *type;
        const cJSON *content;
        cJSON *cjson_str = cJSON_Parse(buffer);
        type = cJSON_GetObjectItemCaseSensitive(cjson_str, "type");
        content = cJSON_GetObjectItemCaseSensitive(cjson_str, "content");
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
        const cJSON *type;
        const cJSON *content;
        cJSON *cjson_str = cJSON_Parse(buffer);
        type = cJSON_GetObjectItemCaseSensitive(cjson_str, "type");
        content = cJSON_GetObjectItemCaseSensitive(cjson_str, "content");
	if(*(type -> valuestring) == 'S')
		printf("Your message is successfully sent!!!\n");
	else
		printf("%s\n", buffer);
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
        const cJSON *type;
        const cJSON *content;
	const cJSON *message;
        cJSON *cjson_str = cJSON_Parse(buffer);
        type = cJSON_GetObjectItemCaseSensitive(cjson_str, "type");
        content = cJSON_GetObjectItemCaseSensitive(cjson_str, "content");
	if(*(type -> valuestring) == 'L') {
		cJSON_ArrayForEach(message, content) {
			cJSON *sender = cJSON_GetObjectItemCaseSensitive(message, "sender");
			cJSON *Content = cJSON_GetObjectItemCaseSensitive(message, "content");
			printf("%s : %s\n", sender -> valuestring, Content -> valuestring);
		}
	}
	else
		printf("%s\n", content -> valuestring);
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
        const cJSON *type;
        const cJSON *content;
	const cJSON *member;
        cJSON *cjson_str = cJSON_Parse(buffer);
        type = cJSON_GetObjectItemCaseSensitive(cjson_str, "type");
        content = cJSON_GetObjectItemCaseSensitive(cjson_str, "content");
	if(*(type -> valuestring) == 'L') {
                cJSON_ArrayForEach(member, content) {
                        printf("%s\n", member -> valuestring);
		}
	}
	else
		printf("%s\n", content -> valuestring);
	return 8;
}

int Leavechannel() {
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
                return 8;
        }
        printf("\n");
        const cJSON *type;
        const cJSON *content;
        cJSON *cjson_str = cJSON_Parse(buffer);
        type = cJSON_GetObjectItemCaseSensitive(cjson_str, "type");
        content = cJSON_GetObjectItemCaseSensitive(cjson_str, "content");
        printf("You successfully left the channel.\n");
        return 4;
}

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#define MAX 80
#define PORT 12345
#define SA struct sockaddr

typedef struct {
        char username[21];
        int channel;
        char *token;
        int line;
}user;
typedef struct {
        char name[21];
        char **members;
        int membernum;
        int lines;
}channel;

int server_socket, client_socket;
struct sockaddr_in server, client;
char *buffer;
user **users;
int usernum = 0;
channel **channels;
int channelnum = 0;

void Register();
void Login();
void Createchannel();
void Joinchannel();
void Send();
void Refresh();
void Channelmembers();
void Leave();
void Logout();
void Channelchaeck();
char * token();
void Makejson();
void Addjson();
void Array1json();
void Array2json();

int main() {
        users = (user **)malloc(1000 * sizeof(user*));
        channels = (channel **)malloc(500 * sizeof(channel));
	char command[15];
        char secondcommand[15];
	buffer = (char *)malloc(5000 * sizeof(char));
	// Create and verify socket
        server_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (server_socket == -1) {
                printf("Socket creation failed...\n");
                exit(0);
        }
        else
                printf("Socket successfully created..\n");

        // Assign IP and port
        bzero(&server, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(PORT);

        // Bind newly created socket to given IP and verify
        if ((bind(server_socket, (SA*)&server, sizeof(server))) != 0) {
                printf("Socket binding failed...\n");
                exit(0);
        }
        else
                printf("Socket successfully bound..\n");

        // Now server is ready to listen and verify
        if ((listen(server_socket, 5)) != 0) {
                printf("Listen failed...\n");
                exit(0);
        }
        else
                printf("Server listening..\n");
	while(true) {
		// Accept the data packet from client and verify
	        socklen_t len = sizeof(client);
        	client_socket = accept(server_socket, (SA*)&client, &len);
        	if (client_socket < 0) {
                	printf("Server accceptance failed...\n");
                	exit(0);
        	}
        	else
                	printf("Server acccepted the client..\n");
		bzero(buffer, 5000);

                // Read the message from client and copy it to buffer
                recv(client_socket, buffer, 5000, 0);
		sscanf(buffer, "%s", command);
		if(strcmp(command, "register") == 0)
			Register();
		if(strcmp(command, "login") == 0)
                        Login();
                if(strcmp(command, "create") == 0) {
                        sscanf(buffer, "%*s %s", secondcommand);
                        if(strcmp(secondcommand, "channel") == 0)
                                Createchannel();
                }
                if(strcmp(command, "join") == 0) {
                        sscanf(buffer, "%*s %s", secondcommand);
                        if(strcmp(secondcommand, "channel") == 0)
                                Joinchannel();
                }
                if(strcmp(command, "send") == 0)
                        Send();
                if(strcmp(command, "refresh") == 0)
                        Refresh();
                if(strcmp(command, "channel") == 0) {
                        sscanf(buffer, "%*s %s", secondcommand);
                        if(strcmp(secondcommand, "members"))
                                Channelmembers();
                }
                if(strcmp(command, "leave") == 0)
                        Leave();
                if(strcmp(command, "logout") == 0)
                        Logout();
                Channelchaeck();
        }
	return 0;
}

void Register() {
        printf("registery request recieved.\n");
        int i = 0;
        char username[21], password[11];
        sscanf(buffer, "%*s %s %s", username, password);
        while(username[i] != ',')
                i++;
        username[i] = '\0';
        char drc[50] = "resources/users/";
        strcat(drc, username);
        strcat(drc, ".txt");
        if(access(drc, F_OK) == -1) {
                FILE *f = fopen(drc, "w");
                fprintf(f, "%s %s\n", username, password);
                fclose(f);
                printf("%s registered.\n", username);
                bzero(buffer, 5000);
                Makejson(buffer, "type", "Successful");
                Addjson(buffer, "content", "");
                send(client_socket, buffer, 5000, 0);
		printf("respond : %s\n", buffer);
        }
        else {
                bzero(buffer, 5000);
                Makejson(buffer, "type", "Error");
                Addjson(buffer, "content", "the username exists.");
                send(client_socket, buffer, 5000, 0);
                printf("respond : %s\n", buffer);
        }
}

void Login() {
        printf("login request recieved.\n");
        int i = 0;
        char username[21], password[11], key[11];
        sscanf(buffer, "%*s %s %s", username, password);
        while(username[i] != ',')
                i++;
        username[i] = '\0';
        for(i = 0; i < usernum; i++) {
                if(strcmp(users[i]->username, username) == 0) {
                        bzero(buffer, 5000);
                        Makejson(buffer, "type", "Error");
                        Addjson(buffer, "content", "This username is already logged in.");
                        send(client_socket, buffer, 5000, 0);
                        printf("respond : %s\n", buffer);
                        return;
                }
        }
        char drc[50] = "resources/users/";
        strcat(drc, username);
        strcat(drc, ".txt");
        if(access(drc, F_OK) == -1) {
                bzero(buffer, 5000);
                Makejson(buffer, "type", "Error");
                Addjson(buffer, "content", "This username is not registerd.");
                send(client_socket, buffer, 5000, 0);
                printf("respond : %s\n", buffer);
        }
        else {
                FILE *f = fopen(drc, "r");
                fscanf(f, "%*s %s", key);
                fclose(f);
                if(strcmp(password, key) == 0) {
                        users[usernum] = (user *)malloc(sizeof(user));
                        strcpy(users[usernum]->username, username);
                        users[usernum]->token = token();
                        bzero(buffer, 5000);
                        Makejson(buffer, "type", "AuthToken");
                        Addjson(buffer, "content", users[usernum]->token);
                        send(client_socket, buffer, 5000, 0);
                        printf("respond : %s\n", buffer);
                        usernum++;
                }
                else {
                        bzero(buffer, 5000);
                        Makejson(buffer, "type", "Error");
                        Addjson(buffer, "content", "Password is not correct.");
                        send(client_socket, buffer, 5000, 0);
                        printf("respond : %s\n", buffer);
                }
        }
}

void Createchannel() {
        int i, k = -1;
        char channelname[21], token[33];
        sscanf(buffer, "%*s %*s %s %s", channelname, token);
        for(i = 0; i < usernum; i++) {
                if(strcmp(users[i]->token, token) == 0) {
                        k = i;
                        break;
                }
        }
        if(k == -1) {
                bzero(buffer, 5000);
                Makejson(buffer, "type", "Error");
                Addjson(buffer, "content", "you're not a valid user.");
                send(client_socket, buffer, 5000, 0);
                printf("respond : %s\n", buffer);
        }
        else {
                i = 0;
                while(channelname[i] != ',')
                        i++;
                channelname[i] = '\0';
                char drc[50] = "resources/channels/";
                strcat(drc, channelname);
                strcat(drc, ".txt");
                if(access(drc, F_OK) == -1) {
                        FILE *f = fopen(drc, "w");
                        fprintf(f, "server  %s created channel\n", users[k]->username);
                        fclose(f);
                        channels[channelnum] = (channel *)malloc(sizeof(channel));
                        strcpy(channels[channelnum]->name, channelname);
                        channels[channelnum]->members = (char **)malloc(500 * sizeof(char *));
                        channels[channelnum]->members[0] = users[k]->username;
                        channels[channelnum]->membernum = 1;
                        channels[channelnum]->lines = 1;
                        users[k]->channel = channelnum;
                        users[k]->line = 0;
                        channelnum++;
                        printf("channel named %s created by %s.\n", channelname, users[k]->username);
                        bzero(buffer, 5000);
                        Makejson(buffer, "type", "Successful");
                        Addjson(buffer, "content", "");
                        send(client_socket, buffer, 5000, 0);
			printf("respond : %s\n", buffer);
                }
                else {
                        bzero(buffer, 5000);
                        Makejson(buffer, "type", "Error");
                        Addjson(buffer, "content", "a channel with this name exists.");
                        send(client_socket, buffer, 5000, 0);
                        printf("respond : %s\n", buffer);
                }
        }
}

void Joinchannel() {
        int i, k = -1;
        char channelname[21], token[33], c;
        sscanf(buffer, "%*s %*s %s %s", channelname, token);
        for(i = 0; i < usernum; i++) {
                if(strcmp(users[i]->token, token) == 0) {
                        k = i;
                        break;
                }
        }
        if(k == -1) {
                bzero(buffer, 5000);
                Makejson(buffer, "type", "Error");
                Addjson(buffer, "content", "you're not a valid user.");
                send(client_socket, buffer, 5000, 0);
                printf("respond : %s\n", buffer);
        }
        else {
                i = 0;
                while(channelname[i] != ',')
                        i++;
                channelname[i] = '\0';
                char drc[50] = "resources/channels/";
                strcat(drc, channelname);
                strcat(drc, ".txt");
                if(access(drc, F_OK) == -1) {
                        bzero(buffer, 5000);
                        Makejson(buffer, "type", "Error");
                        Addjson(buffer, "content", "channel with this name doesn't exist.");
                        send(client_socket, buffer, 5000, 0);
                        printf("respond : %s\n", buffer);
                }
                else {
                        int j = -1;
                        for(i = 0; i < channelnum; i++) {
                                if(strcmp(channels[i]->name, channelname) == 0) {
                                        j = i;
                                        break;
                                }
                        }
                        FILE *f = fopen(drc, "a");
                        fprintf(f, "server  %s joined the channel.\n", users[k]->username);
                        fclose(f);
                        if(j == -1) {
                                channels[channelnum] = (channel *)malloc(sizeof(channel));
                                strcpy(channels[channelnum]->name, channelname);
                                channels[channelnum]->members = (char **)malloc(500 * sizeof(char *));
                                channels[channelnum]->members[0] = users[k]->username;
                                channels[channelnum]->membernum = 1;
                                users[k]->line = 0;
                                channels[channelnum]->lines = 0;
                                FILE *f = fopen(drc, "r");
                                do {
                                c = (char)fgetc(f);
                                        if (c == '\n')
                                                channels[channelnum]->lines++;
                                } while(c != EOF);
                                fclose(f);
                                users[k]->channel = channelnum;
                                channelnum++;
                        }
                        else {
                                channels[j]->members[channels[j]->membernum] = users[k]->username;
                                channels[j]->membernum++;
                                users[k]->channel = j;
                                channels[j]->lines++; 
                                users[k]->line = 0;
                        }
                        printf("%s joined channel named %s.\n", users[k]->username, channelname);
                        bzero(buffer, 5000);
                        Makejson(buffer, "type", "Successful");
                        Addjson(buffer, "content", "");
                        send(client_socket, buffer, 5000, 0);
			printf("respond : %s\n", buffer);
                }
        }

}

void Send() {
        int k = -1, i;
        char message[121], token[33];
        sscanf(buffer, "%*s %[^,]%*c %s", message, token);
        for(i = 0; i < usernum; i++) {
                if(strcmp(users[i]->token, token) == 0) {
                        k = i;
                        break;
                }
        }
        if(k == -1) {
                bzero(buffer, 5000);
                Makejson(buffer, "type", "Error");
                Addjson(buffer, "content", "you're not a valid user.");
                send(client_socket, buffer, 5000, 0);
                printf("respond : %s\n", buffer);
        }
        else {
                char drc[50] = "resources/channels/";
                strcat(drc, channels[users[k]->channel]->name);
                strcat(drc, ".txt");
                FILE *f = fopen(drc, "a");
                fprintf(f, "%s  %s\n", users[k]->username, message);
                fclose(f);
                channels[users[k]->channel]->lines++;
                printf("message sent to the channel.\n");
                bzero(buffer, 5000);
                Makejson(buffer, "type", "Successful");
                Addjson(buffer, "content", "");
                send(client_socket, buffer, 5000, 0);
		printf("respond : %s\n", buffer);
        }
}

void Refresh() {
        int k = -1, i;
        char token[33], sender[21], message[121];
        sscanf(buffer, "%*s %s", token);
        for(i = 0; i < usernum; i++) {
                if(strcmp(users[i]->token, token) == 0) {
                        k = i;
                        break;
                }
        }
        if(k == -1) {
                bzero(buffer, 5000);
                Makejson(buffer, "type", "Error");
                Addjson(buffer, "content", "you're not a valid user.");
                send(client_socket, buffer, 5000, 0);
                printf("respond : %s\n", buffer);
        }
        else {
                char drc[50] = "resources/channels/";
                strcat(drc, channels[users[k]->channel]->name);
                strcat(drc, ".txt");
                FILE *f = fopen(drc, "r");
                for(i = 0; i < users[k]->line; i++) {
                        fscanf(f, "%*[^\n]%*c");
                }
                Makejson(buffer, "type", "List");
                Addjson(buffer, "content", "[]");
                for(i = users[k]->line; i < channels[users[k]->channel]->lines; i++) {
                        fscanf(f, "%s  %[^\n]",sender, message);
                        Array1json(buffer, sender, message);
                }
                send(client_socket, buffer, 5000, 0);
                users[k]->line = channels[users[k]->channel]->lines;
                printf("respond : %s\n", buffer);
        }
}

void Channelmembers() {
        int k = -1, i;
        char token[33], sender[21], message[121];
        sscanf(buffer, "%*s %*s %s", token);
        for(i = 0; i < usernum; i++) {
                if(strcmp(users[i]->token, token) == 0) {
                        k = i;
                        break;
                }
        }
        if(k == -1) {
                bzero(buffer, 5000);
                Makejson(buffer, "type", "Error");
                Addjson(buffer, "content", "you're not a valid user.");
                send(client_socket, buffer, 5000, 0);
                printf("respond : %s\n", buffer);
        }
        else {
                Makejson(buffer, "type", "List");
                Addjson(buffer, "content", "[]");
                for(i = 0; i < channels[users[k]->channel]->membernum; i++) {
                        Array2json(buffer, channels[users[k]->channel]->members[i]);
                }
                send(client_socket, buffer, 5000, 0);
                printf("respond : %s\n", buffer);
        }
}

void Leave() {
        int k = -1, i, l = 0;
        char token[33], sender[21], message[121];
        sscanf(buffer, "%*s %s", token);
        for(i = 0; i < usernum; i++) {
                if(strcmp(users[i]->token, token) == 0) {
                        k = i;
                        break;
                }
        }
        if(k == -1) {
                bzero(buffer, 5000);
                Makejson(buffer, "type", "Error");
                Addjson(buffer, "content", "you're not a valid user.");
                send(client_socket, buffer, 5000, 0);
                printf("respond : %s\n", buffer);
        }
        else {
                for(i = 0; i < channels[users[k]->channel]->membernum; i++) {
                        if(l == 1) {
                                channels[users[k]->channel]->members[i - 1] = channels[users[k]->channel]->members[i];
                        }
                        if(strcmp(users[k]->username, channels[users[k]->channel]->members[i])) {
                                l = 1;
                        }
                }
		char drc[50] = "resources/channels/";
                strcat(drc, channels[users[k]->channel]->name);
                strcat(drc, ".txt");
                FILE *f = fopen(drc, "a");
                fprintf(f, "server  %s left the channel\n", users[k]->username);
                fclose(f);
                channels[users[k]->channel]->lines++;
                channels[users[k]->channel]->membernum--;
                bzero(buffer, 5000);
                Makejson(buffer, "type", "Successful");
                Addjson(buffer, "content", "");
                send(client_socket, buffer, 5000, 0);
                printf("%s left channel %s.\n", users[k]->username, channels[users[k]->channel]->name);
		printf("respond : %s\n", buffer);
        }
}

void Logout() {
        int k = -1, i, l = 0;
        char token[33], sender[21], message[121];
        sscanf(buffer, "%*s %s", token);
        for(i = 0; i < usernum; i++) {
                if(strcmp(users[i]->token, token) == 0) {
                        k = i;
                        break;
                }
        }
        if(k == -1) {
                bzero(buffer, 5000);
                Makejson(buffer, "type", "Error");
                Addjson(buffer, "content", "you're not a valid user.");
                send(client_socket, buffer, 5000, 0);
                printf("respond : %s\n", buffer);
        }
        else {
                free(users[k]);
                for(i = k; i < (usernum - 1); i++) {
                        users[i] = users[i + 1];
                }
                usernum--;
                Makejson(buffer, "type", "Successful");
                Addjson(buffer, "content", "");
                send(client_socket, buffer, 5000, 0);
                printf("user loged out.\n");
		printf("respond : %s\n", buffer);
        }
}

void Channelchaeck() {
        int i, j;
        for(i = 0; i < channelnum; i++) {
                if (channels[i]->membernum == 0) {
                        free(channels[i]);
                        for(j = i; j < channelnum - 1; j++) {
                                channels[j] = channels[j + 1];
                        }
                        i--;
                        channelnum--;
                }
        }
}

char * token() {
        char *a = (char *)malloc(33 * sizeof(char));
        char chars[] = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm-_1234567890";
        for (int i = 0; i < 32; i++)
                a[i] = chars[rand() % 63];
        a[32] = '\0';
        return a;
}

void Makejson(char *str, const char *b, const char *c) {
        sprintf(str, "{\"%s\":\"%s\"}", b, c);
}

void Addjson(char *x, const char *a, const char *b) {
        x = strtok(x, "}");
        strcat(x, ",\"");
        strcat(x, a);
        strcat(x, "\":\"");
        strcat(x, b);
        strcat(x, "\"}");
}

void Array1json(char *str, const char *a, const char *b) {
        int i = 0, k = 0;
        while (str[i] != ']') {
                i++;
        }
        str = strtok(str, "]");
        if(str[i - 1] != '[') {
                strcat(str, ",");
        }
        strcat(str, "{\"sender\":\"");
        strcat(str, a);
        strcat(str, "\",\"content\":\"");
        strcat(str, b);
        strcat(str, "\"}]}");
}

void Array2json(char *str, const char *a) {
        int i = 0, k = 0;
        while (str[i] != ']') {
                i++;
        }
        str = strtok(str, "]");
        if(str[i - 1] != '[') {
                strcat(str, ",");
        }
        strcat(str, "\"");
        strcat(str, a);
        strcat(str, "\"]}");
}

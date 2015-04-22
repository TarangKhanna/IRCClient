
#include <time.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <gtk/gtk.h>
using namespace std;
char * host;
char * user;
char * password;
char * sport;
char * args;
int port;

#define MAX_MESSAGES 100
#define MAX_MESSAGE_LEN 300
#define MAX_RESPONSE (20 * 1024)

int lastMessage = 0;

GdkPixbuf *create_pixbuf(const gchar * filename)
{
	GdkPixbuf *pixbuf;
	GError *error = NULL;
	pixbuf = gdk_pixbuf_new_from_file(filename, &error);
	if (!pixbuf) {
		fprintf(stderr, "%s\n", error->message);
		g_error_free(error);
	}

	return pixbuf;
}

int open_client_socket(char * host, int port) {
	// Initialize socket address structure
	struct  sockaddr_in socketAddress;

	// Clear sockaddr structure
	memset((char *)&socketAddress, 0, sizeof(socketAddress));

	// Set family to Internet 
	socketAddress.sin_family = AF_INET;

	// Set port
	socketAddress.sin_port = htons((u_short)port);

	// Get host table entry for this host
	struct  hostent  *ptrh = gethostbyname(host);
	if (ptrh == NULL) {
		perror("gethostbyname");
		exit(1);
	}

	// Copy the host ip address to socket address structure
	memcpy(&socketAddress.sin_addr, ptrh->h_addr, ptrh->h_length);

	// Get TCP transport protocol entry
	struct  protoent *ptrp = getprotobyname("tcp");
	if (ptrp == NULL) {
		perror("getprotobyname");
		exit(1);
	}

	// Create a tcp socket
	int sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
	if (sock < 0) {
		perror("socket");
		exit(1);
	}

	// Connect the socket to the specified server
	if (connect(sock, (struct sockaddr *)&socketAddress,
		sizeof(socketAddress)) < 0) {
		perror("connect");
		exit(1);
	}

	return sock;
}

int sendCommand(char * host, int port, char * command, char * user,
	char * password, char * args, char * response) {
	int sock = open_client_socket(host, port);

	// Send command
	write(sock, command, strlen(command));
	write(sock, " ", 1);
	write(sock, user, strlen(user));
	write(sock, " ", 1);
	write(sock, password, strlen(password));
	write(sock, " ", 1);
	write(sock, args, strlen(args));
	write(sock, "\r\n", 2);

	// Keep reading until connection is closed or MAX_REPONSE
	int n = 0;
	int len = 0;
	while ((n = read(sock, response + len, MAX_RESPONSE - len))>0) {
		len += n;
	}

	//printf("response:%s\n", response);

	close(sock);
}

void printUsage()
{
	printf("Usage: talk-client host port user password args\n");
	exit(1);
}  

void add_user(char * host, int port, char * user,
	char * password)
{
	// Try first to add user in case it does not exist.
    char response[MAX_RESPONSE];
	sendCommand(host, port, "ADD-USER", user, password, "", response);

	if (strcmp(response, "OK\r\n") == 0) {
		printf("User %s added\n", user);
	}
}

void create_room(char * host, int port, char * user,
	char * password, char * args) {
    char response[MAX_RESPONSE];
	sendCommand(host, port, "CREATE-ROOM", user, password, args, response);

	if (strcmp(response, "OK\r\n") == 0) {
		printf("User %s added\n", user);
	}
}

void enter_room(char * host, int port, char * user,
	char * password, char * args) {
    char response[MAX_RESPONSE];
	sendCommand(host, port, "ENTER-ROOM", user, password, args, response);

	if (strcmp(response, "OK\r\n") == 0) {
		printf("User %s added\n", user);
	}
}

void leave_room(char * host, int port, char * user,
	char * password, char * args) {
    char response[MAX_RESPONSE];
	sendCommand(host, port, "LEAVE-ROOM", user, password, args, response);

	if (strcmp(response, "OK\r\n") == 0) {
		printf("User %s added\n", user);
	}
}

void get_messages() {
    char response[MAX_RESPONSE];
	sendCommand(host, port, "GET-MESSAGES", user, password, args, response);

	if (strcmp(response, "OK\r\n") == 0) {
		printf("User %s added\n", user);
	}
}

void send_message(char * host, int port, char * user,
	char * password, char * args) {
    char response[MAX_RESPONSE];
	sendCommand(host, port, "SEND-MESSAGE", user, password, args, response);

	if (strcmp(response, "OK\r\n") == 0) {
		printf("User %s added\n", user);
	}
}

void print_users_in_room() {
    char response[MAX_RESPONSE];
	sendCommand(host, port, "GET-USERS-IN-ROOM", user, password, args, response);

	if (strcmp(response, "OK\r\n") == 0) {
		printf("User %s added\n", user);
	}
}

void print_users() {
	// Try first to add user in case it does not exist.
    char response[MAX_RESPONSE];
	sendCommand(host, port, "GET-ALL-USERS", user, password, "", response);

	if (strcmp(response, "OK\r\n") == 0) {
		printf("User %s added\n", user);
	}
}

void printPrompt() {
	printf("talk> ");
	fflush(stdout);
}

void printHelp() {
	printf("Commands:\n");
	printf(" -who   - Gets users in room\n");
	printf(" -users - Prints all registered users\n");
	printf(" -help  - Prints this help\n");
	printf(" -quit  - Leaves the room\n");
	printf("Anything that does not start with \"-\" will be a message to the chat room\n");
}

void * getMessagesThread(void * arg) {
	// This code will be executed simultaneously with main()
	// Get messages to get last message number. Discard the initial Messages

	while (1) {
		// Get messages after last message number received.

		// Print messages

		// Sleep for ten seconds
		usleep(2 * 1000 * 1000);
	}
}

void startGetMessageThread()
{
	//pthread_t threads;
	pthread_create(NULL, NULL, getMessagesThread, NULL);
}

int main(int argc, char **argv) {

	char line[MAX_MESSAGE_LEN + 1];

	if (argc < 6) {
		printUsage();
	}

	host = argv[1];
	sport = argv[2];
	user = argv[3];
	password = argv[4];
	args = argv[5];

	printf("\nStarting talk-client %s %s %s %s\n", host, sport, user, password);

	// Convert port to number
	sscanf(sport, "%d", &port);

	add_user(host, port, user, password);

    enter_room(host, port, user, password, args);

	// Enter room
	enter_room(host, port, user, password, args);

	// Start message thread
	//startGetMessageThread();

	while (1) {
		printPrompt();

		char * s = fgets(line, MAX_MESSAGE_LEN, stdin);
		if (s == NULL) {
			leave_room(host, port, user, password, args);
			printf("talk-client exiting...\n");
			exit(1);
		}

		if (!isatty(0)) {
			// If it is not a terminal, echo command as well
			printf("%s\n", line);
		}

		if (line[0] == '-') {
			// This is a command process it
			if (!strcmp(line, "-help")) {
				printHelp();
			}
			else if (!strcmp(line, "-quit")) {
				printf("talk-client exiting...\n");
				exit(1);
			}
			// Put other commands here
		}
		else if (line[0] == 0) {
			// Empty line. Print help
			printf("Type -help to print the available commands\n");
		}
		else {
			// Send message
		}
	}

	printf("TEST ENDS\n");
	return 0;
}

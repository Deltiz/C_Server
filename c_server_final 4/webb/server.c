#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <netdb.h>
#define PORT 8080
int main(int argc, char const* argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	//char buffer[1024] = { 0 };
	//Create a buffer for requests and the response
	char buffer[8192];
	char resp[8192];
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	//Bind
	if (bind(server_fd, (struct sockaddr*)&address,
			sizeof(address))
		< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	//Listen for connections
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	//Infinite loop that waits for requests
	while(1) {
		int bytes = 0;
		//Accept connection
		if ((new_socket
			= accept(server_fd, (struct sockaddr*)&address,
					(socklen_t*)&addrlen))
			< 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		//Read request from client and save in buffer
		valread = read(new_socket, buffer, 1024);
		printf("%s\n", buffer);
		//Extract the filename from the GET request
		//Gets the first line
		char *file = strtok(buffer, " \n\r");
		//Gets the filename
		file = strtok(NULL, " \n\r");
		//Removes the '/' left with just the filename
		file+=1;
		printf("%s\n", file);
		if(strcmp(file, "") == 0) {
			file = "index.html";
		}
		printf("Request FILE: %s\n", file);
		//Try to open the file	
		int f  = open(file, O_RDONLY);

		//If the file cannot be opened
		if(f == -1) {
			//Send 404 response
			sprintf(resp, "HTTP/1.0 404 Not Found\r\nServer: Demo Web Server\r\nContent-Length: %ld\r\nContent-Type: %s\r\n\r\n",1, "unknown");
			send(new_socket, resp, strlen(resp), 0);
			//Close the socket
			close(new_socket);
		}

		else {
			//Want to get the file extentsion from the filename
			//We want whatever is after the '.' in filename
			const char separator = '.';
			//strchr gets whatever is after the separator
			char *ext = strchr(file, separator);
			printf("%s\n", ext);
			char *type = "unknown";
			//Supports only .html and .jpg at the moment
			if(strcmp(ext, ".html") == 0)
				type = "text/html";
			if(strcmp(ext, ".jpg") == 0)
				type = "image/jpg";

			//File size	
			struct stat st;
			stat(file, &st);
			long fileLength = st.st_size;
			//Allocate memory to save file content
			char * html_content= (char *)malloc(fileLength);
			//Save filecontents in html_content
			//fread(html_content, 1, fileLength, f);

			//200 response with length and type of file
			sprintf(resp, "HTTP/1.0 200 OK\r\nServer: Demo Web Server\r\nContent-Length: %ld\r\nContent-Type: %s\r\n\r\n",fileLength, type);
			//Send response
			send(new_socket, resp, strlen(resp), 0);
			//Send file body
			//OP code
			//html_content = file
			while((bytes = read(f, html_content, fileLength) > 0)) {
				//Write to html_content
				write(f, html_content, fileLength);
			}
			//Send html_content
			send(new_socket, html_content, fileLength, 0);	
			// closing socket and file, free mem
			free(html_content);
			close(f);
			close(new_socket);
		}
	}
	//Shutdown server
	shutdown(server_fd, SHUT_RDWR);
	return 0;
}


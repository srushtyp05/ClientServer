//Team members:

//Shaily Mukeshbhai Patel - 110123322 - Section 2
//Srushty Mukeshbhai Patel - 110123335 - Section 2  


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <libgen.h> 

#define BUFFER_SIZE 1024
#define COUNT_FILE "count_ss.txt"

//function to send request to socket
void forsendingcommand_ss(int socket, const char *forrcommand_ss);

//function to receive response from socket
void forreceivingresponse_ss(int socket, char **argv);


//function to read and write counts of request
int forreadingcount_ss();
void forwritingcount_ss(int forcount_ss);

//client request is send to the socket
void forsendingcommand_ss(int socket, const char *forrcommand_ss) {
    send(socket, forrcommand_ss, strlen(forrcommand_ss), 0);
}


//client response will be received from socket
void forreceivingresponse_ss(int socket, char **argv) {

//buffer to store response
    char response_ss[BUFFER_SIZE];
    
    //variable for storing the size of response
    ssize_t receivingsize_ss;

    //request is received either from server or from mirror
    receivingsize_ss = recv(socket, response_ss, sizeof(response_ss), 0);
    
    
    //checking for errors using if..else...
    if (receivingsize_ss < 0) {
        perror("Something went wrong. There is an error in receiving a response.");
        exit(EXIT_FAILURE);
    } else if (receivingsize_ss == 0) {
    
        //client has closed the connection
        printf("The connection is closed.\n");
        exit(EXIT_SUCCESS);
    }

    
    //printf("Received %zd bytes: %s\n", receivingsize_ss, response_ss);
    printf("The response from server or mirror:  %s\n", response_ss);


    //buffer to store path of client command
    char pathofclient_ss[BUFFER_SIZE];
    
    //if there is an error then exit 
    if (realpath(argv[0], pathofclient_ss) == NULL) {
       // perror("Something went wrong. There is an error.\n");
        exit(EXIT_FAILURE);
    }

    //buffer for storing directory path of client command
    char directoryofclient_ss[BUFFER_SIZE];
    
    //copy the path
    strcpy(directoryofclient_ss, dirname(pathofclient_ss));

    //buffer to store the path of the directory f23 project
    char pathoff23project_ss[BUFFER_SIZE];
    
    //creating path for project folder
    snprintf(pathoff23project_ss, sizeof(pathoff23project_ss), "%s/f23project", directoryofclient_ss);

    //create project folder in same as client file
    char commandofmkdir_ss[BUFFER_SIZE];
    
    //create the path 
    snprintf(commandofmkdir_ss, sizeof(commandofmkdir_ss), "mkdir -p %s", pathoff23project_ss);
    system(commandofmkdir_ss);

    //copy the tar file into created project folder
    char commandofcopy_ss[BUFFER_SIZE];
    
    //create path to copy tar file in project folder
    snprintf(commandofcopy_ss, sizeof(commandofcopy_ss), "cp temp.tar.gz %s/", pathoff23project_ss);
    system(commandofcopy_ss);
}

//function to read counts
int forreadingcount_ss() {

//open file for reading counts
    FILE *count_file = fopen(COUNT_FILE, "r");
    
    //if error in opening then it is an error
    if (count_file == NULL) {
        return 0; 
    }

    int forcount_ss;
    
    //reading counts from file
    fscanf(count_file, "%d", &forcount_ss);
    
    //close the file
    fclose(count_file);

    return forcount_ss;
}


//write counts to the file
void forwritingcount_ss(int forcount_ss) {

//open file for counting
    FILE *count_file = fopen(COUNT_FILE, "w");
    
    
    //if there is an error opening file then
    if (count_file == NULL) {
    
    //print an error message
        perror("Something went wrong. There is an error.\n");
        
        //exit
        exit(EXIT_FAILURE);
    }

//write the count value
    fprintf(count_file, "%d", forcount_ss);
    
    //close the file
    fclose(count_file);
}


//main function
int main(int argc, char **argv) {

//to store address of server or mirror
    struct sockaddr_in forserveraddress_ss;
    
    //buffer to store command that will be sent to mirror or server
    char forrcommand_ss[BUFFER_SIZE];

    //loop for client to stay in infinite loop to send commands to server or mirror
    while (1) {
        int forrequestingcount_ss = forreadingcount_ss();

        //check if client connects with mirror or server according to requirements
        int checkingconnection_ss = (forrequestingcount_ss % 8 < 4);

        //setting address of mirror and server
        memset(&forserveraddress_ss, 0, sizeof(forserveraddress_ss));
        forserveraddress_ss.sin_family = AF_INET;
        
        //IP address
        forserveraddress_ss.sin_addr.s_addr = inet_addr("127.0.0.1"); 
        
        //using server and mirror port for switching client's requests
        forserveraddress_ss.sin_port = htons(checkingconnection_ss ? 54321 : 54322); 

        int socketofclient_ss;

        //create socket to communicate with server and mirror
        socketofclient_ss = socket(AF_INET, SOCK_STREAM, 0);
        
        //if there is an error creating socket then print an error message
        if (socketofclient_ss == -1) {
            perror("Something went wrong. There is an error in creating a socket.\n");
            
            //exit
            exit(EXIT_FAILURE);
        }

        //connection with server and mirror
        //if there is an error, print error message
        if (connect(socketofclient_ss, (struct sockaddr *)&forserveraddress_ss, sizeof(forserveraddress_ss)) == -1) {
            perror("Something went wrong. Couldn't able to connect to server or mirror.\n");
            
            //close client socket
            close(socketofclient_ss);
            
            //exit from client terminal
            exit(EXIT_FAILURE);	
        }
        
        
        //print to send command
        printf("Enter a command: \n");
        
        //sending commands to mirror/server
        fgets(forrcommand_ss, sizeof(forrcommand_ss), stdin);
        
        //receive response from mirror or server
        forsendingcommand_ss(socketofclient_ss, forrcommand_ss);

        // Receive and print the response_ss from the server/mirror
        forreceivingresponse_ss(socketofclient_ss, argv);

        //close the socket
        close(socketofclient_ss);
        
        
        
        //logic to switch connections from server to mirror and mirror to server
        //if the count is less than or equal to eight
    if (forrequestingcount_ss <=8)
    
    //then check for count is less than four
    {	if (forrequestingcount_ss > 4) {
    
    //if count is less than four then switch to server to mirror or mirror to server
            checkingconnection_ss = !checkingconnection_ss;
            
            //otherwise remain in the same terminal
    	} else {
    	checkingconnection_ss = checkingconnection_ss;
    	}
    	
    	
    	//if counter is greater than eight then
    	//if the count is divisible by two 
    } else if (forrequestingcount_ss % 2 == 0) {
    
    //then switch to other terminal
    	checkingconnection_ss = !checkingconnection_ss;
    	
    	//otherwise remain in the same terminal
    	} else {
    	checkingconnection_ss = checkingconnection_ss;
    	}


        // Update the request count
        forrequestingcount_ss++;
        forwritingcount_ss(forrequestingcount_ss);
    }

    return 0;
}

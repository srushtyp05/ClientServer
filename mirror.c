//Team members:

//Shaily Mukeshbhai Patel - 110123322 - Section 2
//Srushty Mukeshbhai Patel - 110123335 - Section 2  


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

//it is a function that will send a response to the client through socket
void forsendingresponse_ss(int socketofclient_ss, const char *response_ss) {

    //it will send the response string to the client
    send(socketofclient_ss, response_ss, strlen(response_ss), 0);
}

//it is a function that will gather the information of a particular file and after that it will store this information in a string
void forfileinformation_ss(const char *forfilename_ss, char *forinformation_ss) {

    //it is a structure that will hold the information of a particular file
    struct stat file_stat;
    
    //it will check whether the file information is obtained successfully or not
    if (stat(forfilename_ss, &file_stat) == 0) {
    
        //it will only extract the files
        char *name = strrchr(forfilename_ss, '/');
        
        //it will only extract the name of the files
        name = (name != NULL) ? name + 1 : (char *)forfilename_ss;  
        
        //it will give all information of files (name, size, creation date and permission)
        sprintf(forinformation_ss, "Name: %s\nSize: %ld bytes\nDate Created: %sPermissions: %o\n",
                name, file_stat.st_size, ctime(&file_stat.st_ctime), file_stat.st_mode & 0777);
    } else {
    
        //if the file information is not obtained successfully, then it will print an error message
        strcpy(forinformation_ss, "Something went wrong. No file found.\n");
    }
}

//it is a function that will send the information of a particular file to the client through socket
void forsendingfile_ss(int socketofclient_ss, const char *forfilename_ss) {

    //it will open a particular file for reading in binary mode
    FILE *file = fopen(forfilename_ss, "rb");
    
    //it will check if the file is opened successfully or not
    if (file == NULL) {
    
        //if file is not opened successfully, then it will print an error message
        forsendingresponse_ss(socketofclient_ss, "Something went wrong. No file found.\n");
        return;
    }

    char buffer[BUFFER_SIZE];
    
    //it will give the number of bytes that are read from the file
    size_t forreadingbytes_ss;

    //it will first read the file and after that, it will send the file to the client
    while ((forreadingbytes_ss = fread(buffer, 1, sizeof(buffer), file)) > 0) {
    
        //it will send the file information to the client
        send(socketofclient_ss, buffer, forreadingbytes_ss, 0);
    }

    //it will close the file
    fclose(file);
}

//it is a function that will create a tar file 
void forcreatingtarfile_ss(const char *forextension_ss) {

    // it will find the command to locate the files with a specified extension
    char forrcommand_ss[BUFFER_SIZE];
    
    //it will create a tar file
    snprintf(forrcommand_ss, sizeof(forrcommand_ss), "find ~ -type f -name '*.%s' -print | tar -cvf temp.tar.gz -T -", forextension_ss);
    
    //it will execute the command
    system(forrcommand_ss);
}

//it is a function that will perfom getfn command 
//it will return the filename, size(in bytes), date created and file permissions
void forgetfncommand_ss(int socketofclient_ss, const char *forrcommand_ss) {

    //this buffer is used to store the extracted file name
    char forfilename_ss[256];
    
    //using getfn command, it will extract the file name
    sscanf(forrcommand_ss, "getfn %s", forfilename_ss);

    //it will find the specific file in the home directory
    char forfindingcommand_ss[BUFFER_SIZE];
    
    //it will constructe a find command to search for files in the user's home directory and after this, it will store the results 
    snprintf(forfindingcommand_ss, sizeof(forfindingcommand_ss), "find $HOME -name '%s' -print", forfilename_ss);
    
    //using a pipe, it will execute the find command
    FILE *forfindingpipe_ss = popen(forfindingcommand_ss, "r");
    
    //it will check if the execution of command is successful or not
    if (forfindingpipe_ss == NULL) {
    
        //if the execution is not successful, then it will print an error message
        forsendingresponse_ss(socketofclient_ss, "Something went wrong. There is an error in executing the command.\n");
        return;
    }

    //this buffer is used to store the path of the founded file
    char forpathoffile_ss[PATH_MAX];

    //it will first read the information of file and after that, it will store that information
    while (fgets(forpathoffile_ss, sizeof(forpathoffile_ss), forfindingpipe_ss) != NULL) {
    
        //it will remove the newline character from the path
        size_t forlength_ss = strlen(forpathoffile_ss);
        
        //it will check for the last character in the path
        //if the last character in the file path is a newline character or not
        if (forlength_ss > 0 && forpathoffile_ss[forlength_ss - 1] == '\n') {
        
            //if it is true, then it will replace the newline character with a null terminator \0
            forpathoffile_ss[forlength_ss - 1] = '\0';
        }

        //this buffer is used to store the information of a file
        char forinformationoffile_ss[BUFFER_SIZE];
        
        //it will get the information of file and then store it
        forfileinformation_ss(forpathoffile_ss, forinformationoffile_ss);

        //it will send information of file to the client
        forsendingresponse_ss(socketofclient_ss, forinformationoffile_ss);

        //it will break the loop to send only the first file information
        break;
    }

    //close the pipe
    pclose(forfindingpipe_ss);
}

//it is a function that will perfom getft command 
//it will give all files with a specific extension and will store those file in temp.tar.gz file
void forgetftcommand_ss(int socketofclient_ss, const char *forrcommand_ss) {

    //the buffer is used to store the extracted file extensions
    char forextension_ss[32];
    
    //it will extract the file extensions using getft command
    int forfinaloutcome_ss = sscanf(forrcommand_ss, "getft %s", forextension_ss);

    //it will check if the extraction was successful
    if (forfinaloutcome_ss != 1) {
    
        //if it was unsuccessful, then it will print an error message
        forsendingresponse_ss(socketofclient_ss, "Something went wrong. Invalid command.\n");
        return;
    }

    //it will check the number of specified file extensions
    char *extension_ss = strtok(forextension_ss, ",");
    int forcountofextension_ss = 0;

    //it will count the number of extensions
    while (extension_ss != NULL) {
    
        //it will increament the extension count
        forcountofextension_ss++;
        
        //it will move to the next extension
        extension_ss = strtok(NULL, ",");
    }
    
    //it will check if the number of extensions is within the allowed range i.e. 1 to 3
    if (forcountofextension_ss < 1 || forcountofextension_ss > 3) {
        forsendingresponse_ss(socketofclient_ss, "The number of extesions for this command are allowed upto 3.\n");
        return;
    }

    //it will find the files with specified extensions and create a tar archive
    char forfindingcommand_ss[BUFFER_SIZE];
    
    //it will find the files with specified extensions in the home directory and after this, it will create a tar file
    snprintf(forfindingcommand_ss, sizeof(forfindingcommand_ss), "find ~ -type f -name '*.%s' -print | tar -cvf temp.tar.gz -T -", forextension_ss);
    
    //using system function, it will execute the find and tar command
    system(forfindingcommand_ss);

    //it will check if the tar file is created successfully or not
    struct stat archive_stat;
    if (stat("temp.tar.gz", &archive_stat) == 0 && archive_stat.st_size > 0) {
    
        //if it is successfully created, then it will print a message
        forsendingresponse_ss(socketofclient_ss, "temp.tar.gz file is created in the home directory of client.\n");
    } else {
        forsendingresponse_ss(socketofclient_ss, "Something went wrong. No file found.\n");
    }
}

//it is a function that will perfom getfz command 
//it give give all files within a specified range and will store those files in temp.tar.gz file
void forgetfzcommand_ss(int socketofclient_ss, const char *forrcommand_ss) {
    long forsize1_ss, forsize2_ss;
    
    //it will extract the size range from getfz command
    int forfinaloutcome_ss = sscanf(forrcommand_ss, "getfz %ld %ld", &forsize1_ss, &forsize2_ss);

    //it will check if the extraction was successful and the size range is valid
    if (forfinaloutcome_ss != 2 || forsize1_ss < 0 || forsize2_ss < 0 || forsize1_ss > forsize2_ss) {
    
        //if not, it will print an error message
        forsendingresponse_ss(socketofclient_ss, "Something went wrong. Invalid command.\n");
        return;
    }

    //it will find the files within the specified size range and will create a tar file
    char forfindingcommand_ss[BUFFER_SIZE];
    
    //it will find the files within the specified size range in the home directory and will create a tar file.
    snprintf(forfindingcommand_ss, sizeof(forfindingcommand_ss), "find ~ -type f -size +%ldc -size -%ldc -print | tar -czvf temp.tar.gz -T -", forsize1_ss, forsize2_ss);
    
    //using system function, it will execute the find and tar command
    system(forfindingcommand_ss);

    //it will check if the tar file is created successfully or not
    struct stat archive_stat;
    if (stat("temp.tar.gz", &archive_stat) == 0 && archive_stat.st_size > 0) {
    
        //if it is successfully created, then it will print a message
        forsendingresponse_ss(socketofclient_ss, "temp.tar.gz file is created in the home directory of client.\n");
    } else {
        forsendingresponse_ss(socketofclient_ss, "Something went wrong. No files found.\n");
    }
}

//it is a function that will perfom getfda command 
//it will give all the files that are created after the specified date and will store those files in temp.tar.gz file
void forgetfdacommand_ss(int socketofclient_ss, const char *forrcommand_ss) {

    //it will store the extracted date string
    char fordate_ss[20];
    
    //it will extract the date from getfda command
    int forfinaloutcome_ss = sscanf(forrcommand_ss, "getfda %s", fordate_ss);

    //it will check if the extraction is successful
    if (forfinaloutcome_ss != 1) {
    
        //if not, then it will print an error message
        forsendingresponse_ss(socketofclient_ss, "Something went wrong. Invalid command.\n");
        return;
    }

    //it will convert the date string to a struct tm
    struct tm date_tm;
    
    
    memset(&date_tm, 0, sizeof(struct tm));
    
    //it will check if the date string has the correct format YYYY-MM-DD or not
    if (strptime(fordate_ss, "%Y-%m-%d", &date_tm) == NULL) {
    
    //if not, then it will print an error message
        forsendingresponse_ss(socketofclient_ss, "The format of date is wrong. Use YYYY-MM-DD format to get the result.\n");
        return;
    }

    //it will convert struct tm to time_t
    time_t date_time = mktime(&date_tm);

    //it will get the current working directory of the client
    char forclientpwd_ss[BUFFER_SIZE];
    
    //it will check if getting the current working directory of the client is successful
    if (getcwd(forclientpwd_ss, sizeof(forclientpwd_ss)) == NULL) {
    
        //if not, then it will print an error message
        forsendingresponse_ss(socketofclient_ss, "Something went wrong. Invalid command.\n");
        return;
    }

    //it will locate the files that are created on or before the specified date
    char forfindingcommand_ss[BUFFER_SIZE];
    
    //it will find the files created on or after the specified date and will create a tar file in the client's home directory
    snprintf(forfindingcommand_ss, sizeof(forfindingcommand_ss), "find ~ -type f -newermt \"%s\" -print | tar -cvf %s/temp.tar.gz -T -", fordate_ss, forclientpwd_ss);
    
    //using system function, it will execute the find and tar command
    system(forfindingcommand_ss);

    //it will check if the tar file is created successfully or not
    struct stat archive_stat;
    if (stat(forclientpwd_ss, &archive_stat) == 0 && archive_stat.st_size > 0) {
    
        //if it is successfully created, then it will print a message
        forsendingresponse_ss(socketofclient_ss, "temp.tar.gz file is created in the home directory of client.\n");
    } else {
        forsendingresponse_ss(socketofclient_ss, "Something went wrong, No files found\n");
    }

    char forcopying_ss[BUFFER_SIZE];
    
    //it will copy the files in th f23project directory in the client's home directory
    snprintf(forcopying_ss, sizeof(forcopying_ss), "cp %s/temp.tar.gz f23project/", forclientpwd_ss);
    system(forcopying_ss);
}



void forgetfdbcommand_ss(int socketofclient_ss, const char *forrcommand_ss) {

    //it will store the extracted date string
    char fordate_ss[20];
    
    //it will extract the date from getfdb command
    int forfinaloutcome_ss = sscanf(forrcommand_ss, "getfdb %s", fordate_ss);

    //it will check if the extraction is successful
    if (forfinaloutcome_ss != 1) {
    
        //if not, then it will print an error message
        forsendingresponse_ss(socketofclient_ss, "Something went wrong. Invalid command.\n");
        return;
    }

    //it will convert the date string to a struct tm
    struct tm date_tm;
    
    
    memset(&date_tm, 0, sizeof(struct tm));
    
    //it will check if the date string has the correct format YYYY-MM-DD or not
    if (strptime(fordate_ss, "%Y-%m-%d", &date_tm) == NULL) {
    
        //if not, then it will print an error message
        forsendingresponse_ss(socketofclient_ss, "The format of date is wrong. Use YYYY-MM-DD format to get the result.\n");
        return;
    }

    //it will convert struct tm to time_t
    time_t date_time = mktime(&date_tm);

    //it will locate the files that are created on or before the specified date
    char forfindingcommand_ss[BUFFER_SIZE];
    
    //it will find the files created on or before the specified date and will create a tar file in the client's home directory
    snprintf(forfindingcommand_ss, sizeof(forfindingcommand_ss), "find ~ -type f ! -newermt \"%s 23:59:59\" -print | tar -cvf temp.tar.gz -T -", fordate_ss);
    
    //using system function, it will execute the find and tar command
    system(forfindingcommand_ss);

    //it will check if the tar file is created successfully or not
    struct stat archive_stat;
    if (stat("temp.tar.gz", &archive_stat) == 0 && archive_stat.st_size > 0) {
    
        //if it is successfully created, then it will print a message
        forsendingresponse_ss(socketofclient_ss, "temp.tar.gz file is created in the home directory of client.\n");
    } else {
        forsendingresponse_ss(socketofclient_ss, "Something went wrong. No files found\n");
    }
}


//it is a function that will process client requests based on the command
void pclientrequest(int socketofclient_ss, const char *forrcommand_ss) {

    //buffer is used to store the response that will be sent to the client
    char response_ss[BUFFER_SIZE];

    //it will check the type of the command and will call the function according to that command
    //if the command is getfn, then it will call forgetfncommand_ss function
    if (strncmp(forrcommand_ss, "getfn ", 6) == 0) {
        forgetfncommand_ss(socketofclient_ss, forrcommand_ss);
    } 
    
    //if the command is getfz, then it will call forgetfzcommand_ss function
    else if (strncmp(forrcommand_ss, "getfz ", 6) == 0) {
        forgetfzcommand_ss(socketofclient_ss, forrcommand_ss);
    } 
    
    //if the command is getft, then it will call forgetftcommand_ss function
    else if (strncmp(forrcommand_ss, "getft ", 6) == 0) {
        forgetftcommand_ss(socketofclient_ss, forrcommand_ss);	
    } 
    
    //if the command is getfda, then it will call forgetfdacommand_ss function
    else if (strncmp(forrcommand_ss, "getfda ", 7) == 0) {
        forgetfdacommand_ss(socketofclient_ss, forrcommand_ss);
    } 
    
    //if the command is getfdb, then it will call forgetfdbcommand_ss function
    else if (strncmp(forrcommand_ss, "getfdb ", 7) == 0) {
        forgetfdbcommand_ss(socketofclient_ss, forrcommand_ss);
    } 
    
    //if the command is quitc, then it will exit the terminal
    else if (strncmp(forrcommand_ss, "quitc", 5) == 0) {
     exit(EXIT_FAILURE);
        forsendingresponse_ss(socketofclient_ss, "You are now exiting.\n");
       
    } else {
        forsendingresponse_ss(socketofclient_ss, "Something went wrong. Invalid command.\n");
    }
}

//it is a function that will handle the communication with a client in a separate thread
void *forhandlingclient_ss(void *arg) {

    //it will extract the socket of the client
    int socketofclient_ss = *((int *)arg);
    
    //the buffer is used to store the received command from the client
    char forrcommand_ss[BUFFER_SIZE];

    //run infinitely
    while (1) {
    
        //it will receive the command from the client
        ssize_t receivingsize_ss = recv(socketofclient_ss, forrcommand_ss, sizeof(forrcommand_ss), 0);

        //it will check for any errors or it will check if the connection is closed or not
        if (receivingsize_ss <= 0) {
            perror("Connection from server is successful  ");
            
            //it will close th client socket
            close(socketofclient_ss);
            pthread_exit(NULL);
        }

        //it will handle the request of a client
        pclientrequest(socketofclient_ss, forrcommand_ss);
    }

    //it will close the client socket
    close(socketofclient_ss);
    pthread_exit(NULL);
}

int main() {
    int socketofserver_ss, socketofclient_ss;
    
    //it will store the server and client address information
    struct sockaddr_in forserveraddress_ss, client_addr;
    
    //length of the client address structure that is needed for certain socket functions
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t foridofthread_ss;

    //it will create a socket of server
    socketofserver_ss = socket(AF_INET, SOCK_STREAM, 0);
    
    //check if the socket is created successfully or not
    if (socketofserver_ss == -1) {
    
        //if not, then it will print an error message
        perror("Something went wrong. There is an error while creating a socket.\n");
        exit(EXIT_FAILURE);
    }

    //it will clear any existing data in the server address structure
    memset(&forserveraddress_ss, 0, sizeof(forserveraddress_ss));
    
    //it will set the address family to IPv4
    forserveraddress_ss.sin_family = AF_INET;
    
    //it will allow the server to accept connections from any IP address on the machine
    forserveraddress_ss.sin_addr.s_addr = INADDR_ANY;
    
    //it will convert the given port number to the network byte order
    forserveraddress_ss.sin_port = htons(54322); 
    
    
    
    int ss = 1;
setsockopt(socketofserver_ss, SOL_SOCKET, SO_REUSEADDR, &ss, sizeof(int));

    
    

    //it will bind the server socket to the specified address and port
    if (bind(socketofserver_ss, (struct sockaddr *)&forserveraddress_ss, sizeof(forserveraddress_ss)) == -1) {
    
        //if there is an issue in binding, then it will print an error message
        perror("Something went wrong. There is an error while binding a socket.\n");
        exit(EXIT_FAILURE);
    }

    //it will listen for incoming connections with a maximum queue length of 10
    if (listen(socketofserver_ss, 10) == -1) {
    
        //if there is an issue, then it will print an error message
        perror("Something went wrong. There is an error.\n");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for client to connect\n");

    //run infinitely
    while (1) {
        //it will accept the connection of a client 
        socketofclient_ss = accept(socketofserver_ss, (struct sockaddr *)&client_addr, &client_addr_len);
        if (socketofclient_ss == -1) {
        
            //if there is an issue, then it will print an error message
            perror("Something went wrong. There is an error.\n");
            continue;
        }

        printf("Client connected.\n");

        //it will create a new thread to handle the client
        if (pthread_create(&foridofthread_ss, NULL, forhandlingclient_ss, (void *)&socketofclient_ss) != 0) {
        
            //if there is an issue, then it will print an error message
            perror("Something went wrong. There is an error while creating a thread.\n");
            close(socketofclient_ss);
            continue;
        }

        //it will detach the thread so that it can clean up itself when it finishes
        pthread_detach(foridofthread_ss);
    }

    //it will close the socket of server
    close(socketofserver_ss);
    return 0;
}




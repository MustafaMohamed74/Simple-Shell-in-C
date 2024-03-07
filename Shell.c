#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define cd 1
#define echo 2
#define export 3

void Mycd(void);
void MyEcho(void);
void MyExport(void);
void Execute_based_on_InputType(char *input[20]);

void execute_command(void);
void execute_shell_bultin(int shell_cmnd);

void Setup_Environment(void);
void shell(void);

void reap_child_zombie();
void write_to_log_file(const char *message);
void register_child_signal();
void on_child_exit(int signum) ;

char *cmnd[20];

char *str_var[50];
int num_var[100];
char char_str_var[100]="";
char char_num_var[100]="";

int count_num =0;
int count_str =0 ;

char dir_arr[100];

int main() {
	register_child_signal();
	Setup_Environment();
	shell();

	return 0;
}

void shell(void){
	int i ;
	do {
		char input[1000];

		printf("%s:",getcwd(dir_arr , 100 ));
		scanf(" %[^\n]", input);

		// Use strtok to tokenize the input string
		char *token = strtok(input, " ");
		i=0 ;

		// Iterate through the tokens and store them in cmnd
		while (token != NULL && i < 20) {

			cmnd[i] = token;

			// printf("%s\n",token);
			if(!strcmp(cmnd[0],"export")){
				token = strtok(NULL, "\"");

			}else {
				token = strtok(NULL, " ");
			}
			
			i++;
			
		}
		cmnd[i]=NULL;
		if(!strcmp(cmnd[0],"exit")){
			break;
		}
		//execute based on INPUT Type 
		Execute_based_on_InputType(cmnd);

	} while (strcmp(cmnd[0],"\"exit\""));
}

void Mycd(void){
	if (cmnd[1] != NULL) {
		if (chdir(cmnd[1]) == 0) {
			printf("Changed directory to %s\n", cmnd[1]);
		}
		else if(((strcmp (cmnd[1],"~")==0))){
			chdir(getenv("HOME"));
		}

		else {
			printf("%s\n",cmnd[1]);
			perror("chdir");
		}
	} else {
		fprintf(stderr, "cd: missing argument\n");
	}
}
void MyEcho(void){
	for(int i = 0; i< strlen(cmnd[1]); ++i){

		if(cmnd[1][i]=='$'){
			for(int j =0 ; j< 100; j++){
				if(cmnd[1][i+1]==char_str_var[j] ){
					printf("%s",str_var[j]);
					break;
				}
				if(cmnd[1][i+1]==char_num_var[j]){
					printf("%d",num_var[j]);
					break;
				}
			}
			i++ ;
			continue;
		}
		printf("%c",cmnd[1][i]);
	}
	printf("\n");
}
void MyExport(void){
	char_str_var[count_str] = cmnd[1][0];
	if(cmnd[2]!=0){
		str_var[count_str] = malloc(strlen(cmnd[2]) - 2 + 1); 
		int i;  
		for (i = 3; cmnd[1][i] != '\0'; ++i) {
			str_var[count_str][i - 3] = cmnd[1][i];
		}
		// Add null terminator to str_var[count_str]
		str_var[count_str][i - 3] = '\0';
	}else{
		str_var[count_str] = malloc(strlen(cmnd[1]) - 2 + 1); 
		int i;  
		for (i = 3; cmnd[1][i] != '\0'; ++i) {
			str_var[count_str][i - 3] = cmnd[1][i-1];
		}
		// Add null terminator to str_var[count_str]
		str_var[count_str][i - 3] = '\0';
	}
	if (str_var[count_str] == NULL) {
		fprintf(stderr, "Memory allocation error\n");
		exit(EXIT_FAILURE);
	}

	count_str++;
}
void Execute_based_on_InputType(char *input[20]){
	int cmnd; 
	if((!strcmp(input[0],"cd")) || (!strcmp(input[0],"echo")) || (!strcmp(input[0],"export"))){
		if(!strcmp(input[0],"cd")){
			cmnd = cd ;

		}
		else if (!strcmp(input[0],"echo") ){
			cmnd = echo ;
		}
		else if (!strcmp(input[0],"export")){
			cmnd = export ;
		}
		execute_shell_bultin(cmnd);
	}
	else {
		execute_command();
	}

}

void execute_shell_bultin(int shell_cmnd){
	switch(shell_cmnd){

	case cd:
		Mycd();
		break;
	case echo :
		MyEcho();
		break ;
	case export:
		MyExport();
		break;

	default:
		break; 
	}
}

void execute_command(void){
	int pid =fork();
	if (pid == 0) {
		// This is the child process
		// Code for child
		if(cmnd[1]== NULL){
			execvp(cmnd[0],cmnd);
			perror("execvp");
			exit(EXIT_SUCCESS);
		}

		else{
			//  This is the child process
			// Code for child

			if(cmnd[1][0]=='$'){
				for(int j =0 ; j< 100; j++){
					if(cmnd[1][1]==char_str_var[j] ){
						// printf("%s",str_var[j]);
						cmnd[1]=str_var[j];
						break;
					}
					// if(cmnd[1][0]==char_num_var[j]){
					//     cmnd[1]=num_var[j];
					//     break;
					// }
				}
				// Split cmnd[1] on spaces
				char *token2 = strtok(cmnd[1], " ");

				// Iterate through the tokens and store them in cmnd[1], cmnd[2], ...
				int e = 1; // Start from index 1
				while (token2 != NULL && e < 100) {
					// Store the token in the appropriate cmnd array element
					cmnd[e]=token2 ;
					// Move to the next token
					token2 = strtok(NULL, " ");
					e++;
				}

			}
			int i = 0;
			while (cmnd[i] != NULL) {
				// Calculate the length of the original string
				size_t originalLength = strlen(cmnd[i]);

				// Create a new string with double quotes
				char* stringWithQuotes = (char*)malloc(originalLength + 3);  // +3 for the double quotes and null terminator
				if (stringWithQuotes == NULL) {
					fprintf(stderr, "Memory allocation failed\n");

				}

				// Add double quotes
				strcpy(stringWithQuotes, "\"");
				strcat(stringWithQuotes, cmnd[i]);
				strcat(stringWithQuotes, "\"");


				// Free the allocated memory
				free(stringWithQuotes);
				i++;
				
			}
			
			execvp(cmnd[0], cmnd);
			perror("execvp");
			exit(EXIT_SUCCESS);

		}
	} else if (pid > 0) {
		// This is the parent process
		// Code for parent
		int status;
		pid_t child_pid = waitpid(pid, &status, 0);
	

	} else {
		// Fork failed
		printf("Fork failed");
	}


}

void Setup_Environment(void){
	char proj_dir[100];
	chdir(getcwd(proj_dir , 100));
}

void reap_child_zombie() {
	int status;
	pid_t child_pid = waitpid(-1, &status, WNOHANG);

	// Check if a child process has terminated
	if (child_pid > 0) {
		// Perform cleanup or additional processing as needed
		printf("Child with PID %d terminated\n", child_pid);

		// Check the exit status of the child process
		if (WIFEXITED(status)) {
			printf("Exit status: %d\n", WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			printf("Terminated by signal: %d\n", WTERMSIG(status));
		}
	}
}

void write_to_log_file(const char *message) {
	FILE *log_file = fopen("logfile.txt", "a");
	if (log_file != NULL) {
		fprintf(log_file, "%s\n", message);
		fclose(log_file);
	} else {
		perror("Error opening log file");
	}
}

void register_child_signal() {
	// Register the signal handler for SIGCHLD (child process termination)
	if (signal(SIGCHLD, on_child_exit) == SIG_ERR) {
		perror("Error registering signal handler");
		exit(EXIT_FAILURE);
	}
}

void on_child_exit(int signum) {
	// Handle child process termination
	reap_child_zombie();
	write_to_log_file("Child terminated");
}

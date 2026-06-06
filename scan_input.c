#include"header.h"

//to store list of external commands(extract from file and execute in the function)
char *ext_command[154];

int ret;
int status;
struct job *head = NULL;

// Main shell loop:
// - Displays prompt
// - Reads user input
// - validates input
// - Identifies command type (builtin/external)
// - Executes accordingly
// - Handles process creation and job control

void scan_input(){

	extract_external_commands(ext_command);
	signal(SIGINT,signal_handler);
	signal(SIGTSTP,signal_handler);
	
	while(1){

		//resetting the return
		ret=0;

		printf(ANSI_COLOR_GREEN"%s"ANSI_COLOR_RESET,prompt);

		//emptying the string
		input_string[0]='\0';

		//collecting the input string
		scanf("%[^\n]",input_string);
		getchar();

		if(strlen(input_string)==0){
			continue;
		}
		
		//if prompt change command is given, we are changing it
		if(strncmp(input_string,"PS1=",4)==0){

			int invalid= 0;

			//if space is present immediately after PS1= its invalid
			if(input_string[4]==' ' || input_string[4]=='\0'){
			       printf("Invalid\n");
			       continue;
			}
			//counting the non space characters
			int i=5;
			while(input_string[i]!=' ' && input_string[i]!='\0'){
				i++;
			}
			//length of the new prompt
			int len=i-4;

			//if other characters are present after space, its invalid
			while(input_string[i]!='\0'){
				if(input_string[i]!=' '){
					invalid=1;
					printf("Invalid\n");
					break;
				}
				i++;
			}
			if(invalid) continue;
			strncpy(prompt,input_string+4,len);
			prompt[len]='\0';
			
		}		
	
		
		//to get the command except arguments
		char* command= get_command();

		//the command is passed to know the type of the command(builtin or external)
		int type = check_command_type(command);

		//builtin functions
		if(type==BUILTIN){
			execute_internal_commands();
		}
		//external commands(executing by creating child)
		else if(type==EXTERNAL){
			ret=fork();

			if(ret>0){

				waitpid(ret,&status,WUNTRACED);
				
				//if process stopped, added to the job list
				if(WIFSTOPPED(status)){
					int id = count_jobs()+1;
					printf("\n[%d]      stopped        %s\n",id,input_string);
					insert_first(ret,input_string);
				}
				if (WIFSIGNALED(status)) printf("\n");
			}
			//child with default signal functions
			else if(ret==0){
				signal(SIGINT,SIG_DFL);
				signal(SIGTSTP,SIG_DFL);
				execute_external_commands(input_string);
			}
			else{
				perror("fork");
			}
		}
		//neither builtin or external
		else{
			printf("%s: command not found\n",input_string);
		}
	}
}
// Handles signals:
// SIGINT  (Ctrl+C)  → prints prompt if shell is idle
// SIGTSTP (Ctrl+Z)  → prints prompt if shell is idle
// SIGCHLD           → cleans up terminated background processes
void signal_handler(int signum){
	if(signum==SIGINT){
		//only print prompt if no child was running
		if(ret==0) printf(ANSI_COLOR_GREEN"\n%s"ANSI_COLOR_RESET,prompt);
	}
	if(signum==SIGTSTP){
		if(ret==0) printf(ANSI_COLOR_GREEN"\n%s"ANSI_COLOR_RESET,prompt);
	}
	if(signum==SIGCHLD){
		waitpid(-1,&status,WNOHANG);
	}
	fflush(stdout);
}

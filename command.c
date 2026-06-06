#include"header.h"
#include<unistd.h>

//has command without arguments
char cmd[10];

//2D array of internal commands
char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
		    "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
		    "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help","fg","bg","jobs", NULL};


// Extracts command (first word) from input_string
// Example: "ls -l" → "ls"
char* get_command(){
	int j=0;
	for(int i=0;input_string[i]!='\0';i++){
		if(input_string[i]!=' '){
			cmd[j++]=input_string[i];
		}else{
			break;
		}
	}
	cmd[j]='\0';
	return cmd;
}

// Checks whether command is:
// - BUILTIN  → internal shell command
// - EXTERNAL → system command
// - NO_COMMAND → invalid command

int check_command_type(char* command){
	//checking if the command is built in (from the 2D list)
	for(int i=0;builtins[i]!=NULL;i++){
		if(strcmp(command,builtins[i])==0){
			return BUILTIN;
		}
	}
	for(int i=0;i<154;i++){
		if(strcmp(command,ext_command[i])==0){
			return EXTERNAL;
		}
	}
	return NO_COMMAND;
}

// Reads list of external commands from file
// Stores them in ext_command array for command validation
void extract_external_commands(char** ext_command){
	int fd= open("external_command.txt",O_RDONLY);
	for(int i=0;i<154;i++){
		char ch;
		int size=0; char temp[20];
		while(read(fd,&ch,1)>0){
			if(ch=='\n'){
				break;
			}
			temp[size++]=ch;
		}
		temp[size]='\0';
		ext_command[i]=malloc(size+1);
		strcpy(ext_command[i],temp);
	}
	close(fd);
}

// Executes built-in shell commands:
// - exit, pwd, cd
// - echo $$, echo $?, echo $SHELL
// - jobs, fg, bg
// - Handles job control operations for fg/bg
void execute_internal_commands(){
	//exit
	if(strcmp(input_string,"exit")==0){
		exit(0);
	}
	//pwd
	else if(strcmp(input_string,"pwd")==0){
		char buffer[50];
		getcwd(buffer,50);
		printf("%s\n",buffer);
	}
	//cd
	else if(strncmp(input_string,"cd",2)==0){
		int ret=chdir(input_string+3);
		if(ret==-1){
			printf("cd: %s: No such file or directory\n",input_string+3);
			return;
		}
		char buffer[50];
		getcwd(buffer,50);
		printf("%s\n",buffer);
	}
	//echo $$
	else if(strcmp(input_string,"echo $$")==0){
		printf("\n%d\n",getpid());
	}
	//echo $?
	else if(strcmp(input_string,"echo $?")==0){
		int exit_code;
		if (WIFEXITED(status))
			{
				exit_code = WEXITSTATUS(status);
			}
			else if (WIFSIGNALED(status))
			{
				exit_code = 128 + WTERMSIG(status);
			}
		printf("%d\n",exit_code);
	}
	//echo $SHELL
	else if(strcmp(input_string,"echo $SHELL")==0){
		char buffer[15]="SHELL";
		printf("%s\n",getenv(buffer));
	}
	//jobs
	else if(strcmp(input_string,"jobs")==0){
		int i=0;
		print_jobs(head,&i);
	}
	//fg
	else if(strcmp(input_string,"fg")==0){
		if(head==NULL){
			printf("fg: current: no such job\n");
			return;
		}
		ret = head->pid;
		kill(ret,SIGCONT);
		printf("%s\n",head->command);
		waitpid(ret,&status,WUNTRACED);
		if(WIFSIGNALED(status) || WIFEXITED(status)){
			delete_first();
		}else{
			printf("\n");
		}
	}
	//bg
	else if(strcmp(input_string,"bg")==0){
		if(head==NULL){
			printf("bg: current: no such job\n");
			return;
		}
		ret=head->pid;
		kill(ret,SIGCONT);
		printf("%s\n",head->command);
		delete_first();
	}
}

void execute_external_commands(){
	char temp[25];
	strcpy(temp,input_string);
	int i=0;
	char *argv[10];
	int argc=0;

	//1D array to 2D
	while(temp[i] != '\0'){

		while(temp[i]==' '){
			i++;
		}
		if(temp[i]=='\0'){
			break;
		}
		argv[argc++]= temp+i;
		while(temp[i] != ' ' && temp[i] != '\0'){
			i++;
		}
		if(temp[i] != '\0'){
			temp[i]='\0';
			i++;
		}
	} 
	argv[argc]=NULL;

	//counting pipes
	int pipes=0;
	for(int i=0;i<argc;i++){
		if(strcmp(argv[i],"|")==0){
			pipes++;
		}
	}
	//No pipe
	if(pipes==0){
		execvp(argv[0],argv);
		perror("execvp");
		exit(1);
	}
	//n pipe , n+1 command execution
	else{
		int command[pipes+1];
		int index=0;
		command[index++]=0;

		for(int i=0;i<argc;i++){
			if(strcmp(argv[i],"|")==0){
				command[index++]=i+1;
				argv[i]=NULL;
			}
		}
		int fd[2];
		for(int i=0;i<index;i++){
			if(i!=index-1){
				pipe(fd);
			}
			int pid=fork();
			if(pid>0){
				if(i!=index-1){
					dup2(fd[0],0);
					close(fd[0]);
					close(fd[1]);
				}
				wait(NULL);
			}
			else if(pid==0){
				if(i!=index-1){
					close(fd[0]);
					dup2(fd[1],1);
					close(fd[1]);
				}
				execvp(argv[command[i]],argv+command[i]);
				perror("execvp");
				exit(1);
			}
		}
	}
	exit(0); 
}

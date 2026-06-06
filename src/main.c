#include"header.h"
#include<stdlib.h>

//global varibales to store commands and prompts
char input_string[25];
char prompt[25]="minishell$ ";

int main(){
	system("clear");

	//starts the shell loop
	scan_input(prompt,input_string);
}


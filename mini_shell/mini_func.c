/*
Author:      Chandan Gowda S
Description: Mini_Shell
Date:        30/08/2024
*/

#include "main.h"
int pid,wstatus,stop=0;
char* builtin_cmd[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval", "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source", "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", "fg", "bg", "jobs"};
extern char prompt[100],input_string[256];
sigstore* head;

void signal_handler(int signum){
    if(signum==SIGINT){
        if(input_string[0]=='\0'){
            printf("\n%s ",prompt);
            fflush(stdout);
        }
        return;
    }
    else if(signum==SIGTSTP){
        if(input_string[0]!='\0'){
	        sigstore* node=(sigstore*)malloc(sizeof(sigstore));
	        if(node==NULL){
		    printf("Node unable to create\n");
      		    return;		
            }
	        strcpy(node->cmd,input_string);
	        node->cmd_pid=pid;
	        node->link=NULL;
	        if(head==NULL){
		        head=node;
		        return;
	        }
	        sigstore* temp=head;
	        node->link=temp;
	        head=node;
	        return;
	    }
    }
}

void _delete(){
    sigstore* temp=head;
    if(head->link==NULL) head=NULL;
    else{
        head=head->link;
    }
    free(temp);
}

void copy_change(){
    strcpy(prompt,input_string+4);
}

char *get_command(char* _input_string){
    char* command=(char*)calloc(30,sizeof(char));
    if(command!=NULL){
        int i=0;
        while(_input_string[i]){
            if(_input_string[i]==' '||_input_string[i]=='\n'){
                break;
            }
            command[i]=_input_string[i];
            ++i;
        }
        command[i]='\0';
    }
    return command;
}

int check_command_type(char* command){
    //creation of array of strings to store external commands from file
    char* ext_cmd[152];
    for(int i=0;i<152;i++){
        ext_cmd[i]=calloc(30,sizeof(char));
        if(ext_cmd[i]==NULL){
            printf("Heap is full\n");
            return 0;
        }
    }
    int fd=open("external_commads.txt",O_RDONLY);
    char ch;
    int i=0,j=0;
    while(1){
        int rd=read(fd,&ch,1);
        if(rd==0){
            ext_cmd[i][j]='\0';
            break;
        }
        else if(ch=='\n'){
            ext_cmd[i][j]='\0';
            ++i;
            j=0;
        }
        else{
            ext_cmd[i][j]=ch;
            j++;
        }
    }
    close(fd);
    for(int i=0;i<sizeof(builtin_cmd)/sizeof(builtin_cmd[0]);i++){
        if(strcmp(builtin_cmd[i],command)==0){
            return BUILTIN;
        }
    }
    for(int i=0;i<sizeof(ext_cmd)/sizeof(ext_cmd[0]);i++){
        if(strcmp(ext_cmd[i],command)==0){
            return EXTERNAL;
        }
    }
    return NO_COMMAND;
}

void execute_internal_commands(char* _input_string){
    //system(input_string);
    char buf[200];
    if(strcmp(_input_string,"pwd")==0){
	    getcwd(buf,200);
        printf("%s\n",buf);
    }
    else if(strcmp(_input_string,"exit")==0){
        exit(0);
    }
    else if(strstr(_input_string,"cd")==input_string){
        int ret=chdir(_input_string+3);
        if(ret==0){
            printf("Successfully Changed\n");
        }
        else if(ret==-1){
            printf("No such file or directory\n");
        }
    }
    else if(strstr(_input_string,"echo ")==_input_string){
        int not_special=1;
        if(strstr(_input_string+5,"$$")==_input_string+5){
            //minishell process id
            not_special=0;
            if(strlen(_input_string)>strlen("echo $$")){
                printf("%d %s\n",getpid(),_input_string+7);
            }
            else{
                printf("%d\n",getpid());
            }
        }
        else if(strcmp(_input_string+5,"$?")==0){
            //child status exit code
            not_special=0;
            if(WIFEXITED(wstatus)){
                printf("Child pid %d exited normally with exit code %d\n",pid,WEXITSTATUS(wstatus));
            }
            else{
                printf("Child pid %d exited abnormally with exit code %d\n",pid,WEXITSTATUS(wstatus));
            }
        }
        else if(strcmp(_input_string+5,"$SHELL")==0){
            //env variable
            not_special=0;
            char* env=getenv(_input_string+6);
            printf("%s\n",env);    
        }
        else if(not_special){
            printf("%s\n",_input_string+5);
        }
    }
    else if(strcmp(_input_string,"echo")==0){
        printf("\n");
    }
    else if(strcmp(_input_string,"jobs")==0){
        sigstore* temp=head;
        while(temp!=NULL){
            printf("%s\n",temp->cmd);
            temp=temp->link;
        }
    }
    else if(strcmp(_input_string,"fg")==0){
        printf("%s\n",head->cmd);
        kill(head->cmd_pid,SIGCONT);
        waitpid(head->cmd_pid,&wstatus,WUNTRACED);
        _delete();
    }
    else if(strcmp(_input_string,"bg")==0){
        printf("%s\n",head->cmd);
        kill(head->cmd_pid,SIGCONT);
        //deleting it from linkedlist of sigstore
        _delete();
    }
}

int ispipe(char* _input_string){
    int pipes=0;
    for(int i=0;i<strlen(_input_string);i++){
        if(_input_string[i]=='|') ++pipes;
    }
    return pipes;
}

void split_args(char *command,char *args[]){
    int count=0;
    char *token=strtok(command," \t\n");
    while(token!=NULL&&count<30){
        args[count++]=token;
        token=strtok(NULL," \t\n");
    }
    args[count]=NULL;
}

void execute_n_pipes(int cmdlen,char *commands[]){
    int fd[2];          
    int in_fd=0;
    for(int i=0;i<cmdlen;i++){
        // Creating a pipe
        pipe(fd);  
        pid=fork();
        if(pid==0){  
            // Child process
            if(in_fd!=0){  // If not the first command read from the previous pipe
                dup2(in_fd,0);
                close(in_fd);
            }
            if(i!=cmdlen-1){  // If not the last command, write to the current pipe
                dup2(fd[1],1);
            }
            close(fd[0]);  
            char *args[30];
            split_args(commands[i],args);  // Splitting command into arguments
            execvp(args[0],args);  
            perror("execvp failed");  
            exit(EXIT_FAILURE);
        } 
        else if(pid>0){
            //parent proccess
            close(fd[1]);
            in_fd = fd[0];  
            waitpid(pid,&wstatus,WUNTRACED); //wait for all child
        }
        else{
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }
}

void execute_external_commands(char* _input_string){
    int npipes=ispipe(_input_string);
    if(npipes>0){

        char* cmd_arg[npipes+1];
        for(int i=0;i<npipes+1;i++){
            cmd_arg[i]=(char*)calloc(30,sizeof(char));
            if(cmd_arg[i]==NULL) return;
        }
        int cmd_pos[npipes];
        cmd_pos[0]=0;
        int k=0,l=0,z=1;
        for(int i=0;i<strlen(_input_string);i++){
            if(_input_string[i]=='|'){
                cmd_pos[z++]=i+1; //updating command position
                cmd_arg[k][l]='\0';
                i++; //skipping space
                l=0; 
                k++;
            }
            else{
                cmd_arg[k][l++]=_input_string[i];
            }
        }
        cmd_arg[k][l]='\0';

        execute_n_pipes(npipes+1, cmd_arg);

    }
    else {
        pid=fork();
        if(pid>0){
            //parent proccess
            waitpid(pid,&wstatus,WUNTRACED);
        }
        else if(pid==0){
            //child proccess
            signal(SIGINT,SIG_DFL);
            signal(SIGTSTP,SIG_DFL);
            char* external_commands[10];
            for(int i=0;i<10;i++){
                external_commands[i]=calloc(30,sizeof(char));
                if(external_commands[i]==NULL){
                    printf("Heap is full\n");
                    return;
                }
            }
            int cl_count=0,k=0;
            for(int i=0;i<=strlen(_input_string);i++){
                if(_input_string[i]==' '){
                    cl_count++;
                    k=0;
                }
                else if(_input_string[i]=='\0'){
                    external_commands[cl_count][i]='\0';
                    cl_count++;
                    external_commands[cl_count]=NULL;
                }
                else{
                    external_commands[cl_count][k]=_input_string[i];
                    k++;
                }
            }
            execvp(external_commands[0],external_commands);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
        else{
            perror("fork");
            return;
        }
    }
}

void scan_input(){
    while(1){
        signal(SIGINT,signal_handler);
        signal(SIGTSTP,signal_handler);
        printf("%s ",prompt);
        scanf("%[^\n]s",input_string);
        getchar();
        stop=0;
        if(strstr(input_string,"PS1") == input_string){
            if(strstr(input_string,"PS1 ")||strstr(input_string,"PS1= ")||strlen(input_string)==3){
                printf("PS1: command not found\n");
                continue;
            }
            else if(strstr(input_string,"PS1= ") == NULL){
                copy_change(); //if matches changing prompt name
                continue;
            }
        }
        else if(strlen(input_string)==0) continue;
        char* cmd=get_command(input_string);
        int ret=check_command_type(cmd);
        switch(ret){
            case 1:
                execute_internal_commands(input_string);
            break;
            case 2:
                //external commands
                execute_external_commands(input_string);
            break;
            case 3:printf("%s: command not found\n",input_string);
            break;
        }
        input_string[0]='\0';
    }
}

/* npipes
for(int i=0;i<sizeof(cmd_arg)/sizeof(cmd_arg[0]);i++){
            printf("%s\n",cmd_arg[i]);
        }
        for(int i=0;i<npipes+1;i++){
            printf("%d\n",cmd_pos[i]);
        }
        for(int i=0;i<npipes+1;i++){
            //create a pipe
            int fd[2];
            pipe(fd);
            int cid=fork();
            if(cid>0){
                close(fd[1]);
                dup2(fd[0],0);
                close(fd[0]);
                
            }
            else if(cid==0){
                if(i<=npipes){
                    close(fd[0]);
                    dup2(fd[1],1); 
                }
                printf("Hi\n");
                execvp(cmd_arg[i],cmd_arg+i);
            }
            else {
                perror("fork");
                return;
            }
            waitpid(pid,&wstatus,WUNTRACED);
        }
        int fd[2], input_fd = 0;
        for (int i=0;i<=npipes;i++) {
            pipe(fd);
            pid=fork();
            if (pid == 0) {
                // Child process

                if (input_fd != 0) {
                    dup2(input_fd, 0); // Input from previous pipe
                    close(input_fd);
                }
                if (i != npipes){
                    dup2(fd[1], 1); // Output to next command
                    close(fd[0]);
                }
                execvp(cmd_arg[cmd_pos[i]], cmd_arg+cmd_pos[i]);
                //execlp(cmd_arg[i], cmd_arg[i], (char *)NULL);
                //execvp(cmd_arg[i]+cmd_pos[i],cmd_arg+cmd_pos[i]); // Execute command
                perror("exec");
                exit(1);
            }
            else {
                // Parent process
                if (i < npipes) {
                    close(fd[1]);
                }
                input_fd = fd[0]; // Save input for next command
                waitpid(pid,&wstatus,WUNTRACED); // Wait for child to finish
            }
        }
}*/
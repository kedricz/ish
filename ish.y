%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include <pwd.h>
#include <errno.h>
#include <ctype.h>
  //#include "ish.h"
#include "job.c"

#define numBuiltin 8


extern int yylex();
extern int yyparse();
extern int errno;
typedef void(*sighandler_t)(int);
static char *ishrc_file = "~/.ishrc";


////////////////////////////////////////
//
//    Input variables
//
////////////////////////////////////////
 
 static char *my_argv[20], *temp_argv[20], *my_envp[50];
 static char *search_path[10];
 char *temp_path;
 char* command, *final_cmd;;
 char* arg;
 char* parameter=NULL;
 void build_argv();
 int av=0;  // number of arguments to pass
 int num_env=0;
 pid_t shell_pgid;
 int shell_terminal;
 int shell_is_interactive;
 

////////////////////////////////////////
//
//   Built-in functions
//
////////////////////////////////////////

 char *builtin[] = {"bg", "cd", "exit", "fg", "jobs", "kill", "setenv", "unsetenv"};


 int ish_bg();
 int ish_cd(char *path);
 int ish_exit();
 int ish_fg();
 int ish_jobs();
 int ish_kill();
 int ish_setenv(char *var, const char *word);
 int ish_unsetenv(char *var);

 int is_builtin(char* command);
 int builtin_cmd();
 int mystrcmp(char* const *s1, char const *s2);


//////////////////////////////////////// 
//                                                                                                     
//   Pipe                 
//                                                                                              
////////////////////////////////////////

 int num_pipe=0;
 int fd[2]; // pipe
 int fdr; // >, <
 int redirect_in = 0;
 int redirect_out = 0;
 int append = 0;
 char *filename;
 int pipe_s = 0;
 int semicolon = 0;

//////////////////////////////////////////
//
//    Job
//
//////////////////////////////////////////

 int job_num = 1;
 int nextjid = 1;
 job *jobs = NULL;     // job list
 void put_job_in_background (job *j, int cont);
 void put_job_in_foreground (job *j, int cont);
 void wait_for_job (job *j);
 void format_job_info(job *j, const char *status);
 void launch_process(process *p, pid_t  pgid, int infile, int outfile, int errfile, int foreground);
 void launch_job(job *j, int foreground);
 int mark_process_status (pid_t pid, int status);
 void update_status (void);
 void wait_for_job (job *j);
 void format_job_info (job *j, const char *status);
 void do_job_notification (void);

//////////////////////////////////////////                                       
//                                                                                               
//    Signal                                                                
//                                                                                                
//////////////////////////////////////////

 //sigset_t blockset;
 //sigemptyset(&blockset);
 //sigaddset(&blockset, SIGINT); // add ^C to blockset
 //sigaddset(&blockset, SIGQUIT); // add ^\ to blockset
 //sigprocmask(SIG_BLOCK, &blockset, NULL);
 void handler(int sig);



int print_prompt();




void handle_signal(int signo);
void fill_argv(char *tmp_argv);
void copy_envp(char **envp);
void get_path_string(char **tmp_envp, char *bin_path);
void insert_path_str_to_search(char *path_str);
int attach_path(char *command);

void init_shell ();
void command_exec();
//void call_execve();
void reset_argv();



%}


%union
{
    char	*string;
    int		integer;
}

%token 	<string>	WORD
%token 	<string>	COMMAND
%token 	<string>	FILENAME
%token	<int>		BACKGROUND
%token	<int>		PIPE
%token	<int>		PIPE_ERROR
%token	<int>		SEMICOLON
%token	<int>		REDIRECT_IN
%token	<int>		REDIRECT_OUT
%token	<int>		REDIRECT_ERROR
%token	<int>		APPEND
%token	<int>		APPEND_ERROR
%token	<string>	OPTION
%token	<string>	STRING
%token	<int>		LOGICAL_AND
%token	<int>		LOGICAL_OR

%%

cmd_line 	: cmd_line separator COMMAND parameters  
                | COMMAND parameters       {command=$1; build_argv(); attach_path(command); command_exec();}
		| cmd_line BACKGROUND
		| cmd_line SEMICOLON
		|  
		| error 
		;

separator 	: BACKGROUND        { reset_argv(); }
                | PIPE              { reset_argv();command=NULL; final_cmd=NULL; pipe_s=1;}         
                | PIPE_ERROR        {perror("pipe error");}
                | SEMICOLON         { reset_argv();command=NULL; final_cmd=NULL;semicolon=1; }
                ;

parameters	: parameters OPTION    {temp_argv[av++]=$2; parameter="option";}
                | parameters STRING    {temp_argv[av++]=$2; parameter="string";}
                | parameters WORD      {temp_argv[av++]=$2; parameter="word";}
                | parameters REDIRECT_IN FILENAME      {redirect_in = 1; filename=$3;}
                | parameters REDIRECT_OUT FILENAME     {redirect_in = 1;filename=$3;}
                | parameters REDIRECT_ERROR FILENAME   {perror("redirect error");}
		| parameters APPEND FILENAME           {append = 1;filename=$3;}
                | parameters APPEND_ERROR FILENAME     {perror("append error");}
		|
		;

%%

int yyerror(char *s)
{
    fprintf(stderr, "syntax error\n");
    return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void handler(int sig)
{
  if(getpid != 0) {
    switch(sig) {
    case SIGINT:
      break;
    case SIGTSTP:
      kill(0, SIGCHLD);
      break;
    case SIGCONT:
      break;
    }
  }
}



void fill_argv(char *tmp_argv)
{
  char *tagv = tmp_argv;
  int index = 0;
  char ret[100];
  memset(ret, '\0', 100);
  while(*tagv != '\0') {
    
    if(index==10)
      break;

    if(*tagv==' ') {
      if(my_argv[index] == NULL) {
	my_argv[index] = (char*)malloc(sizeof(char)*strlen(ret)+1);
      }
      else {
	memset(my_argv[index], '\0', strlen(my_argv[index]));
      }

      strncpy(my_argv[index], ret, strlen(ret));
      strncat(my_argv[index], "\0", 1);
      memset(ret, '\0', 100);
      index++;
    }

    else {
      strncat(ret, tagv, 1);
    }
    tagv++;

  } // end while

  my_argv[index] = (char*)malloc(sizeof(char)*strlen(ret)+1);
  strncpy(my_argv[index], ret, strlen(ret));
  strncat(my_argv[index], "\0", 1);

}



/* copy envp to my_envp(global variable) */
void copy_envp(char **envp)
{
  int i = 0;
  for(;envp[i] != NULL; i++) {
    my_envp[i] = (char *)malloc(sizeof(char) * (strlen(envp[i]) + 1));
    memcpy(my_envp[i], envp[i], strlen(envp[i]));
  }                                                                     
}



/* copy path from envp */
void get_path_string(char **tmp_envp, char *bin_path)
{
  int count = 0;
  char *tmp;
  while(1) {
    tmp = strstr(tmp_envp[count], "PATH");
    
    if(tmp == NULL) {
      count++;
    } 
    
    else {
      break;
    }
  }
  strncpy(bin_path, tmp, strlen(tmp));
}


/* copy PATH to search path (global variable) */
void insert_path_str_to_search(char *path_str) 
{
  int i=0;
  char *tmp = path_str;
  char ret[100];

  while(*tmp != '=')
    tmp++;
  tmp++;

  while(*tmp != '\0') {
    if(*tmp == ':') {
      strncat(ret, "/", 1);
      search_path[i] = (char *) malloc(sizeof(char) * (strlen(ret) + 1));
      strncat(search_path[i], ret, strlen(ret));
      strncat(search_path[i], "\0", 1);
      i++;
      memset(ret, '\0', 100);
    } else {
      strncat(ret, tmp, 1);
    }
    tmp++;
  }
}


/* Add command to search path for execve */
int attach_path(char *cmd)
{
  char ret[100];
  int i;
  int fdd;
  memset(ret, '\0', 100);
 

  for(i=0; search_path[i]!=NULL; i++) {
    strcpy(ret, search_path[i]);
    strncat(ret, cmd, strlen(cmd));

    if((fdd = open(ret, O_RDONLY)) > 0) {
      memset(final_cmd, '\0', 100);
      strncpy(final_cmd, ret, strlen(ret));
      close(fdd);
      return 0;
    }
  }
  return 0;
}
    




/* Build the argument array to pass to exeve */
void build_argv()
{
  my_argv[0] = command;
  int i;
  for(i=0; i<av; i++)
    my_argv[i+1] = temp_argv[i];
}



///////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////// 
/* Reset the argument array */
void reset_argv()
{
  int i;
  for(i=0;i<20;i++) {
    //printf("my_argv[] = %s\n", my_argv[i]);
    my_argv[i] = NULL;
  }
  av = 0;
}



/********************************************************************************************/


int main(int argc, char* argv[], char* envp[]) {
  
  char c;
  int i,j, fd;

  
  
  //signal(SIGINT, SIG_IGN);
  signal(SIGINT, handler);
  signal(SIGTSTP, handler);

  /* Install the signal handlers */

  /* These are the ones you will need to implement */
  //Signal(SIGINT,  sigint_handler);   /* ctrl-c */
  //Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
  //Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

 


  while(1) {

    init_shell();
    /* Initializing environment variables & PATH */
    copy_envp(envp);
    temp_path = (char *)malloc(sizeof(char) * 256);
    get_path_string(my_envp, temp_path);
    final_cmd = (char *)malloc(sizeof(char) * 100);
    insert_path_str_to_search(temp_path);

    reset_argv();
    print_prompt();
    yyparse();

    //command_exec(envp);
    
    
 


  }
  return 0;
}








/* print the prompt with current directory + hostname%  */
int print_prompt() {
   char *buffer = (char*)malloc(100);

   
   strcpy(buffer, "[");
   
   // current directory
   char *path = (char*)malloc(100);
   char *value2 = (char*)getcwd(path, 100);

   strncat(buffer, path, strlen(path));
   
   //strncpy(buffer, token2, strlen(token2));
   strcat(buffer, "] ");


   // host name                                                                                   
   char *temp = (char*)malloc(100);
   int value = gethostname(temp, 100);
   //printf("host name = %s\n", temp);

   char *s = ".";
   char *token;
   token = strtok(temp, s);

    //strcpy(buffer, "[");                         
    strncat(buffer, token, strlen(token));
    //strcat(buffer, " ");


    if(value==0) {
      fprintf(stdout, "%s%% ", buffer);    
    }

    free(temp);
    free(buffer);
    return 0;
}





/////////////////////////////////////////////////////////////////////////////////        
//                                                                                       
//   Command 
//                                                                                                
/////////////////////////////////////////////////////////////////////////////////  


/* Excute command using execve  */
void command_exec() {

  // if the command is built_in
  if(is_builtin(command)) {
    if(built_in(command)!=0)
      perror("ish");
  }


  // if the command is not built_in
  else {
    pid_t wpid, pid;
    int status;
    int pp = 0;
    
    process *p = (process*)malloc(sizeof(process));
    p->next = NULL;
    p->argv = my_argv;
    p->pid = getpid();


    process *p_next;
    if(redirect_in==1 || redirect_out==1 || append==1 || pipe_s==1 || semicolon==1) {
      p_next = (process*)malloc(sizeof(process));
      p_next->next = NULL;
      pp=1;
    }

    job *next_job =(job*)malloc(sizeof(job));
    next_job->first_process = p;
    if(pp==1)
      next_job->first_process->next = p_next;

    if(job_num <= 16)
      next_job->jobID = job_num++;
    else {
      job_num = 1;
      next_job->jobID;
    }
      
    job *tjob;
    
    for(tjob=jobs; tjob; tjob->next) {
      if(tjob->next == NULL)
	tjob->next = next_job;
    }

 
    launch_job(next_job, 1);


    /*
    pid = fork();

    // child process                                                                                     
    if(pid==0) {

      if(pipe(fd)==-1)
	perror("pipe fail");

      else {      
	printf("entering launch_job\n");
	job *next_job = (job*)malloc(sizeof(job));
	jobs->next = next_job;
	launch_job(jobs, 1);

		   //execve(final_cmd, my_argv, my_envp);
      }      
      exit(EXIT_FAILURE);
    }


    // fork error
    else if(pid < 0) {
      perror("ish");
    }


    // parent process                                                                                    
    else {
      do 
	{
	wpid = waitpid(pid, &status, WUNTRACED);
      }
      while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    */



    // check for / & ./

    
  }

  //return 0;
}






/////////////////////////////////////////////////////////////////////////////////
//
//   Built-In commands
//
/////////////////////////////////////////////////////////////////////////////////


/* Check if the command is built-in */
int is_builtin(char* command) {
  int i;
  for(i=0; i<numBuiltin; i++) {
    if(strcmp(command, builtin[i]) == 0)
      return 1;
  }
  return 0;
}


                                                                                     
/* Execute built_in command */
int built_in(char* command)
{
  // bg
  if(strcmp(command, "bg")==0) {
    ish_bg(); 
  }

  // cd
  else if(strcmp(command, "cd")==0) {
    //printf("my_argv[1] = %s\n", my_argv[1]);
    ish_cd(my_argv[1]);
  }


  // exit
  else if(strcmp(command, "exit")==0) {
    ish_exit();
  }

  
  // fg
  else if(strcmp(command, "fg")==0) {
    ish_fg();
  }

  
  // jobs                                                                                                              
  else if(strcmp(command, "jobs")==0) {
    ish_jobs();
  }
  

  // kill                                                                                                              
  else if(strcmp(command, "kill")==0) {
    ish_kill();
  }


  // setenv
  else if(strcmp(command, "setenv")==0) {
    char* var = my_argv[1];
    char* word = my_argv[2];
    //printf("var = %s\nword = %s\n", var, word);
    ish_setenv(var, word);
  }


  // unsetenv                                                        
  else if(strcmp(command, "unsetenv")==0) {
    char* var = my_argv[1];
    ish_unsetenv(var);
  }

  else
    return -1;

  return 0;
}





/* Put a job in the background  */
int ish_bg() {
  pid_t pid = getpid();
  pid_t pgid = getpgid(pid);
  job* cj = find_job(pgid, jobs);
  put_job_in_background(cj,0);
  return 0;
}


/////////////////////////////////////////////////////////////
/* Change directory function  */
int ish_cd(char *path) {
  char* cwd;
  char *userdir;
  struct passwd *pwd;

  //printf("path = %s\n", path);

  // if path is not provdied,                                                                                            
  // change to home directory
  if(path==NULL) {
    uid_t uid = getuid();
    pwd = getpwuid(uid);
    userdir = pwd->pw_dir;
    chdir(userdir);
  }

  // if path is provided
  // change to the path
  else {
    if(chdir(path) != 0)
      perror(path);
  }
  
  return 0;
}


///////////////////////////////////////////////////////////// 
/* Exit function */
int ish_exit() {
  exit(0);
  return 0;
}




/* Foreground function  */
int ish_fg() {
  char *id;
  job *jj;

  if(my_argv[1]==NULL && jobs==NULL)
    return 0;


  if(my_argv[1]!=NULL) {
    strcpy(id, my_argv[1]);
    if(id[0] == '%')
      id++;
  
    //printf("id = %s\n", id);
    int jn = atoi(id);  
    // printf("jn = %d\n", jn);
  
  
    for(jj=jobs; jj; jj=jj->next) {
      if(jj->jobID == jn)
	break;
    }
  }
  else if(my_argv[1] == NULL) {
    printf("else if\n");
    jj = jobs;
  }
  
  put_job_in_foreground(jj,0);
  return 0;
}



/////////////////////////////////////////////////////////////
/* Print the job list  */
int ish_jobs() {
  listjobs(jobs);
  return 0;
 }
  



///////////////////////////////////////////////////////////// 
/* Kill the job with the job number  */ 
int ish_kill() 
{
  int jid = atoi(my_argv[1]);
  pid_t cpid = getpid();
  pid_t cpgid = getpgid(cpid);
  
  job* cjob;
  for(cjob = jobs; cjob; cjob=cjob->next) {
    if(cjob->jobID == jid) {
      cpgid = cjob->pgid;
      kill(cpgid, SIGTERM);
      kill(cpgid, SIGCONT);

      return 0;
    }
   }

  //killjob(jobs, jid);
  return -1;
}




  ///////////////////////////////////////////////////////////// 
/* Set environment variables  */   
int ish_setenv(char *var, const char *word) {
  // char *temp = (char*)malloc(sizeof(char)*100);
  int var_len;
  char *upVar;
  int j=0;

  
  if(var!=NULL) {
    var_len = strlen(var);
    upVar = (char*)malloc(sizeof(char)*(var_len+1));
    // Convert to uppercase                                                                                              
    while(j<var_len) {
      upVar[j] = toupper(var[j]);
      j++;
    }
  }


  // if no argument, display all environment variables
  if(var==NULL && word==NULL) {
    int i=0;                                                                        
    for(;my_envp[i] != NULL; i++) {    
      printf("%s\n", my_envp[i]); 
    } 
  }


  // if only VAR argument presents
  else if(var!=NULL && word==NULL) {
    if(var[0]=='\0' || strchr(var, '=') != NULL) {
      errno = EINVAL;
      perror("VAR cannot set to NULL");
      return -1;
    }
    setenv(upVar, NULL, 1);
  }

  // if with both arguments
  else if(var!=NULL && word != NULL) {
    if(var[0]=='\0' || strchr(var, '=') != NULL) {
      errno = EINVAL;
      perror("VAR cannot set to word");
      return -1;
    }
    setenv(upVar, word, 1);
  }

  
  else {
    errno = EINVAL;
    return -1;
  }
  
  return 0;
}


 
/* Remove var from the environment  */    
int ish_unsetenv(char *var) {
  char *upVar;
  int j=0;

  if(var != NULL) {
    // Convert to uppercase                                                                                        
    while(var[j]) {
      upVar[j] = toupper(var[j]);
      j++;
    }
  }

  if(var==NULL || var[0]=='\0' || strchr(var, '=') != NULL) {
    errno = EINVAL;
    //perror("Error");
    return -1;
  }

  else
    unsetenv(upVar);

  return 0;
}






/////////////////////////////////////////////////////////////////////////////////                      
//                                                                                                  
//   jobs
//                                                                                                         
/////////////////////////////////////////////////////////////////////////////////

/* Make sure the shell is running interactively as the foreground job
   before proceeding. */

void init_shell ()
{

  /* See if we are running interactively.  */
  shell_terminal = STDIN_FILENO;
  shell_is_interactive = isatty (shell_terminal);

  if (shell_is_interactive)
    {
      /* Loop until we are in the foreground.  */
      while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ()))
        kill (- shell_pgid, SIGTTIN);

      /* Ignore interactive and job-control signals.  */
      //signal (SIGINT, SIG_IGN);
      signal (SIGQUIT, SIG_IGN);
      //signal (SIGTSTP, SIG_IGN);
      //signal (SIGTTIN, SIG_IGN);
      signal (SIGTTOU, SIG_IGN);
      //signal (SIGCHLD, SIG_IGN);

      /* Put ourselves in our own process group.  */
      shell_pgid = getpid ();
      if (setpgid (shell_pgid, shell_pgid) < 0)
        {
          perror ("Couldn't put the shell in its own process group");
          exit (1);
        }

      /* Grab control of the terminal.  */
      tcsetpgrp (shell_terminal, shell_pgid);

      /* Save default terminal attributes for shell.  */
      tcgetattr (shell_terminal, &shell_tmodes);
    }
}





void launch_process (process *p, pid_t pgid, int infile, int outfile, int errfile,
                int foreground)
{
  pid_t pid;

  if (shell_is_interactive)
    {
      #if defined (HISTORY)
          /* Initialize the interactive history stuff. */
          if (!shell_initialized)
	     load_history ();
      #endif /* HISTORY */

      /* Initialize terminal state for interactive shells after the
	 .ish_profile and .ishrc are interpreted. */
      //get_tty_state ();
    

      /* Put the process into the process group and give the process group
         the terminal, if appropriate.
         This has to be done both by the shell and in the individual
         child processes because of potential race conditions.  */
      pid = getpid ();
      if (pgid == 0) pgid = pid;
      setpgid (pid, pgid);
      if (foreground)
        tcsetpgrp (shell_terminal, pgid);

      /* Set the handling for job control signals back to the default.  */
      //signal (SIGINT, SIG_DFL);
      signal (SIGQUIT, SIG_DFL);
      //signal (SIGTSTP, SIG_DFL);
      //signal (SIGTTIN, SIG_DFL);
      signal (SIGTTOU, SIG_DFL);
      //signal (SIGCHLD, SIG_DFL);
    }

  /* Set the standard input/output channels of the new process.  */
  if (infile != STDIN_FILENO)
    {
      dup2 (infile, STDIN_FILENO);
      close (infile);
    }
  if (outfile != STDOUT_FILENO)
    {
      dup2 (outfile, STDOUT_FILENO);
      close (outfile);
    }
  if (errfile != STDERR_FILENO)
    {
      dup2 (errfile, STDERR_FILENO);
      close (errfile);
    }

  /* Exec the new process.  Make sure we exit.  */
  execve (final_cmd, my_argv, my_envp);
  perror ("execve");
  exit (1);
}




/* launch new job */
void launch_job (job *j, int foreground)
{
  process *p;
  pid_t pid;
  int mypipe[2], infile, outfile;

  infile = j->stdin;
  //printf("infile = %d\n", infile);
  for (p = j->first_process; p; p = p->next)
    {
      /* Set up pipes, if necessary.  */
      if (p->next)
        {
          if (pipe (mypipe) < 0)
            {
              perror ("pipe");
              exit (1);
            }
          outfile = mypipe[1];
        }
      else
        outfile = j->stdout;

      /* Fork the child processes.  */
      pid = fork ();
      if (pid == 0)
        /* This is the child process.  */
        launch_process (p, j->pgid, infile,
                        outfile, j->stderr, foreground);
      else if (pid < 0)
        {
          /* The fork failed.  */
          perror ("fork");
          exit (1);
        }
      else
        {
          /* This is the parent process.  */
          p->pid = pid;
          if (shell_is_interactive)
            {
              if (!j->pgid)
                j->pgid = pid;
              setpgid (pid, j->pgid);
            }
        }

      /* Clean up after pipes.  */
      if (infile != j->stdin)
        close (infile);
      if (outfile != j->stdout)
        close (outfile);
      infile = mypipe[0];
    } // end for

  //format_job_info (j, "launched");

  if (!shell_is_interactive)
    wait_for_job (j);
  else if (foreground)
    put_job_in_foreground (j, 0);
  else
    put_job_in_background (j, 0);
}






/* Return true if all processes in the job have completed.  */
int job_is_completed (job *j)
{
  process *p;

  for (p = j->first_process; p; p = p->next)
    if (!p->completed)
      return 0;
  return 1;
}



/* Put job j in the foreground.  If cont is nonzero,
   restore the saved terminal modes and send the process group a
   SIGCONT signal to wake it up before we block.  */

void put_job_in_foreground (job *j, int cont)
{
  /* Put the job into the foreground.  */
  tcsetpgrp (shell_terminal, j->pgid);
  /* Send the job a continue signal, if necessary.  */
  if (cont)
    {
      tcsetattr (shell_terminal, TCSADRAIN, &j->tmodes);
      if (kill (- j->pgid, SIGCONT) < 0)
        perror ("kill (SIGCONT)");
    }
  /* Wait for it to report.  */
  wait_for_job (j);

  /* Put the shell back in the foreground.  */
  tcsetpgrp (shell_terminal, shell_pgid);

  /* Restore the shell’s terminal modes.  */
  tcgetattr (shell_terminal, &j->tmodes);
  tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes);
}




/* Put a job in the background.  If the cont argument is true, send
   the process group a SIGCONT signal to wake it up.  */

void put_job_in_background (job *j, int cont)
{
  /* Send the job a continue signal, if necessary.  */
  if (cont)
    if (kill (-j->pgid, SIGCONT) < 0)
      perror ("kill (SIGCONT)");
}



/* Store the status of the process pid that was returned by waitpid.
   Return 0 if all went well, nonzero otherwise.  */

int mark_process_status (pid_t pid, int status)
{
  job *j;
  process *p;
  if (pid > 0)
    {
      /* Update the record for the process.  */
      for (j = jobs; j; j = j->next)
        for (p = j->first_process; p; p = p->next)
          if (p->pid == pid)
            {
              p->status = status;
              if (WIFSTOPPED (status))
                p->stopped = 1;
              else
                {
                  p->completed = 1;
                  if (WIFSIGNALED (status))
                    fprintf (stderr, "%d: Terminated by signal %d.\n",
                             (int) pid, WTERMSIG (p->status));
                }
              return 0;
	    }
      //fprintf (stderr, "No child process %d.\n", pid);
      return -1;
    }
  else if (pid == 0 || errno == ECHILD)
    /* No processes ready to report.  */
    return -1;
  else {
    /* Other weird errors.  */
    perror ("waitpid");
    return -1;
  }
}



/* Check for processes that have status information available,
   without blocking.  */
void update_status (void)
{
  int status;
  pid_t pid;

  do
    pid = waitpid (WAIT_ANY, &status, WUNTRACED|WNOHANG);
  while (!mark_process_status (pid, status));
}




/* Check for processes that have status information available,
   blocking until all processes in the given job have reported.  */
void wait_for_job (job *j)
{
  int status;
  pid_t pid;

  do
    pid = waitpid (WAIT_ANY, &status, WUNTRACED);
  while (!mark_process_status (pid, status)
         && !job_is_stopped (j)
         && !job_is_completed (j));
}



/* Format information about job status for the user to look at.  */

void format_job_info (job *j, const char *status)
{
  fprintf (stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
}








/* Notify the user about stopped or terminated jobs.
   Delete terminated jobs from the active job list.  */

void do_job_notification (void)
{
  job *j, *jlast, *jnext;
  process *p;

  /* Update status information for child processes.  */
  update_status ();

  jlast = NULL;
  for (j = jobs; j; j = jnext)
    {
      jnext = j->next;

      /* If all processes have completed, tell the user the job has
         completed and delete it from the list of active jobs.  */
      if (job_is_completed (j)) {
        format_job_info (j, "completed");
        if (jlast)
          jlast->next = jnext;
        else
          jobs = jnext;
        //free_job (j);
      }

      /* Notify the user about stopped jobs,
         marking them so that we won’t do this more than once.  */
      else if (job_is_stopped (j) && !j->notified) {
        format_job_info (j, "stopped");
        j->notified = 1;
        jlast = j;
      }

      /* Don’t say anything about jobs that are still running.  */
      else
        jlast = j;
    }
}



/* Mark a stopped job J as being running again.  */

void mark_job_as_running (job *j)
{
  process *p;

  for (p = j->first_process; p; p = p->next)
    p->stopped = 0;
  j->notified = 0;
}



/* Continue the job J.  */
void continue_job (job *j, int foreground)
{
  mark_job_as_running (j);
  if (foreground)
    put_job_in_foreground (j, 1);
  else
    put_job_in_background (j, 1);
}

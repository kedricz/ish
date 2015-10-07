#ifndef lint
static const char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20130304

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)

#define YYPREFIX "yy"

#define YYPURE 0

#line 2 "ish.y"
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
  /*#include "ish.h"*/
#include "job.c"

#define numBuiltin 8


extern int yylex();
extern int yyparse();
extern int errno;
typedef void(*sighandler_t)(int);
static char *ishrc_file = "~/.ishrc";


/*//////////////////////////////////////*/
/**/
/*    Input variables*/
/**/
/*//////////////////////////////////////*/
 
 static char *my_argv[20], *temp_argv[20], *my_envp[50];
 static char *search_path[10];
 char *temp_path;
 char* command, *final_cmd;;
 char* arg;
 char* parameter=NULL;
 void build_argv();
 int av=0;  /* number of arguments to pass*/
 int num_env=0;
 pid_t shell_pgid;
 int shell_terminal;
 int shell_is_interactive;
 

/*//////////////////////////////////////*/
/**/
/*   Built-in functions*/
/**/
/*//////////////////////////////////////*/

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


/*////////////////////////////////////// */
/*                                                                                                     */
/*   Pipe                 */
/*                                                                                              */
/*//////////////////////////////////////*/

 int num_pipe=0;
 int fd[2]; /* pipe*/
 int fdr; /* >, <*/
 int redirect_in = 0;
 int redirect_out = 0;
 int append = 0;
 char *filename;
 int pipe_s = 0;
 int semicolon = 0;

/*////////////////////////////////////////*/
/**/
/*    Job*/
/**/
/*////////////////////////////////////////*/

 int job_num = 1;
 int nextjid = 1;
 job *jobs = NULL;     /* job list*/
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

/*////////////////////////////////////////                                       */
/*                                                                                               */
/*    Signal                                                                */
/*                                                                                                */
/*////////////////////////////////////////*/

 /*sigset_t blockset;*/
 /*sigemptyset(&blockset);*/
 /*sigaddset(&blockset, SIGINT); // add ^C to blockset*/
 /*sigaddset(&blockset, SIGQUIT); // add ^\ to blockset*/
 /*sigprocmask(SIG_BLOCK, &blockset, NULL);*/
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
/*void call_execve();*/
void reset_argv();



#line 145 "ish.y"
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union
{
    char	*string;
    int		integer;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 173 "y.tab.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define WORD 257
#define COMMAND 258
#define FILENAME 259
#define BACKGROUND 260
#define PIPE 261
#define PIPE_ERROR 262
#define SEMICOLON 263
#define REDIRECT_IN 264
#define REDIRECT_OUT 265
#define REDIRECT_ERROR 266
#define APPEND 267
#define APPEND_ERROR 268
#define OPTION 269
#define STRING 270
#define LOGICAL_AND 271
#define LOGICAL_OR 272
#define YYERRCODE 256
static const short yylhs[] = {                           -1,
    0,    0,    0,    0,    0,    0,    1,    1,    1,    1,
    2,    2,    2,    2,    2,    2,    2,    2,    2,
};
static const short yylen[] = {                            2,
    4,    2,    2,    2,    0,    1,    1,    1,    1,    1,
    2,    2,    2,    3,    3,    3,    3,    3,    0,
};
static const short yydefred[] = {                         0,
    6,   19,    0,    0,    0,    8,    9,    0,    0,   13,
    0,    0,    0,    0,    0,   11,   12,   19,   14,   15,
   16,   17,   18,    0,
};
static const short yydgoto[] = {                          3,
    9,    4,
};
static const short yysindex[] = {                      -250,
    0,    0, -258, -244,    0,    0,    0,    0, -249,    0,
 -259, -247, -245, -243, -242,    0,    0,    0,    0,    0,
    0,    0,    0, -244,
};
static const short yyrindex[] = {                        11,
    0,    0,    0,   15,    1,    0,    0,    7,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   19,
};
static const short yygindex[] = {                         0,
    0,   -8,
};
#define YYTABLESIZE 282
static const short yytable[] = {                         19,
    3,    5,    6,    7,    8,    1,    4,    2,   18,   24,
    5,   20,   10,   21,    2,   22,   23,    0,    1,   11,
   12,   13,   14,   15,   16,   17,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    7,    0,
    3,    3,    3,    3,   10,    0,    4,    4,    4,    4,
    5,    5,    5,    5,    2,    2,    2,    2,    1,    1,
    1,    1,
};
static const short yycheck[] = {                        259,
    0,  260,  261,  262,  263,  256,    0,  258,  258,   18,
    0,  259,  257,  259,    0,  259,  259,   -1,    0,  264,
  265,  266,  267,  268,  269,  270,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  258,   -1,
  260,  261,  262,  263,  258,   -1,  260,  261,  262,  263,
  260,  261,  262,  263,  260,  261,  262,  263,  260,  261,
  262,  263,
};
#define YYFINAL 3
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 272
#if YYDEBUG
static const char *yyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"WORD","COMMAND","FILENAME",
"BACKGROUND","PIPE","PIPE_ERROR","SEMICOLON","REDIRECT_IN","REDIRECT_OUT",
"REDIRECT_ERROR","APPEND","APPEND_ERROR","OPTION","STRING","LOGICAL_AND",
"LOGICAL_OR",
};
static const char *yyrule[] = {
"$accept : cmd_line",
"cmd_line : cmd_line separator COMMAND parameters",
"cmd_line : COMMAND parameters",
"cmd_line : cmd_line BACKGROUND",
"cmd_line : cmd_line SEMICOLON",
"cmd_line :",
"cmd_line : error",
"separator : BACKGROUND",
"separator : PIPE",
"separator : PIPE_ERROR",
"separator : SEMICOLON",
"parameters : parameters OPTION",
"parameters : parameters STRING",
"parameters : parameters WORD",
"parameters : parameters REDIRECT_IN FILENAME",
"parameters : parameters REDIRECT_OUT FILENAME",
"parameters : parameters REDIRECT_ERROR FILENAME",
"parameters : parameters APPEND FILENAME",
"parameters : parameters APPEND_ERROR FILENAME",
"parameters :",

};
#endif

int      yydebug;
int      yynerrs;

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  500
#endif
#endif

#define YYINITSTACKSIZE 500

typedef struct {
    unsigned stacksize;
    short    *s_base;
    short    *s_mark;
    short    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
/* variables for the parser stack */
static YYSTACKDATA yystack;
#line 196 "ish.y"

int yyerror(char *s)
{
    fprintf(stderr, "syntax error\n");
    return 0;
}



///////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////

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
#line 1479 "y.tab.c"

#if YYDEBUG
#include <stdio.h>		/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (short *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return -1;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return -1;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack)) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = YYLEX) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack))
        {
            goto yyoverflow;
        }
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    yyerror("syntax error");

    goto yyerrlab;

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yystack.s_mark]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack))
                {
                    goto yyoverflow;
                }
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 2:
#line 171 "ish.y"
	{command=yystack.l_mark[-1].string; build_argv(); attach_path(command); command_exec();}
break;
case 7:
#line 178 "ish.y"
	{ reset_argv(); }
break;
case 8:
#line 179 "ish.y"
	{ reset_argv();command=NULL; final_cmd=NULL; pipe_s=1;}
break;
case 9:
#line 180 "ish.y"
	{perror("pipe error");}
break;
case 10:
#line 181 "ish.y"
	{ reset_argv();command=NULL; final_cmd=NULL;semicolon=1; }
break;
case 11:
#line 184 "ish.y"
	{temp_argv[av++]=yystack.l_mark[0].string; parameter="option";}
break;
case 12:
#line 185 "ish.y"
	{temp_argv[av++]=yystack.l_mark[0].string; parameter="string";}
break;
case 13:
#line 186 "ish.y"
	{temp_argv[av++]=yystack.l_mark[0].string; parameter="word";}
break;
case 14:
#line 187 "ish.y"
	{redirect_in = 1; filename=yystack.l_mark[0].string;}
break;
case 15:
#line 188 "ish.y"
	{redirect_in = 1;filename=yystack.l_mark[0].string;}
break;
case 16:
#line 189 "ish.y"
	{perror("redirect error");}
break;
case 17:
#line 190 "ish.y"
	{append = 1;filename=yystack.l_mark[0].string;}
break;
case 18:
#line 191 "ish.y"
	{perror("append error");}
break;
#line 1737 "y.tab.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            if ((yychar = YYLEX) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack))
    {
        goto yyoverflow;
    }
    *++yystack.s_mark = (short) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    yyerror("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}

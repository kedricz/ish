#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <termios.h>
#include "ish.h"


#define MAXJOBS 16   // max jobs at any point in time
#define MAXJID 1<<16 // max job ID



pid_t shell_pgid;                                                                                                        
struct termios shell_tmodes;                                                                                            
int shell_terminal;                                                                                                      
int shell_is_interactive;


/*************************************
void clearjob(job *job);
void initjobs(job *jobs);
int maxjobid(job *jobs); 
int addjob(job *jobs, pid_t pid, int state, char *cmdline);
int deletejob(job *jobs, pid_t pid); 
pid_t fgpid(job *jobs);
job *getjobpid(job *jobs, pid_t pid);
job *getjobjid(job *jobs, int jobid); 
int pid2jid(pid_t pid); 



int killjob(job *jobs, int jid);
void listjobs(job *j, pid_t pgid);




job *find_job(pid_t pgid, job* fjob);
int job_is_completed(job *j);
int mark_process_status (pid_t pid, int status);
*********************************************/











/* clearjob - Clear the entries in a job struct */
void clearjob(job *jobs) {
  jobs->pgid = 0;
  jobs->jobID = 0;
  jobs->state = UNDEF;
  jobs->command = NULL;
}

/* initjobs - Initialize the job list */
void initjobs(job *jobs) {
  int i;

  for (i = 0; i < MAXJOBS; i++)
    clearjob(&jobs[i]);
}


/* maxjid - Returns largest allocated job ID */
int maxjid(job *jobs) 
{
  int i, max=0;

  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].jobID > max)
      max = jobs[i].jobID;
  return max;
}

/* addjob - Add a job to the job list */
/*
int addjob(job *jobs, pid_t pid, int state, char *cmdline) 
{
  int i;
    
  if (pid < 1)
    return 0;

  for (i = 0; i < MAXJOBS; i++) {
    if (jobs[i]->pgid == 0) {
      jobs[i]->pgid = pid;
      jobs[i]->state = state;
      jobs[i]->jobID = nextjid++;
      if (nextjid > MAXJOBS)
	nextjid = 1;
      strcpy(jobs[i]->command, cmdline);
      if(verbose){
	printf("Added job [%d] %d %s\n", jobs[i]->jobID, jobs[i]->pgid, jobs[i]->cmd);
      }
      return 1;
    }
  }
  printf("Tried to create too many jobs\n");
  return 0;
}
*/


/* deletejob - Delete a job whose PID=pid from the job list */
/*
int deletejob(job *jobs, pid_t pid) 
{
  int i;

  if (pid < 1)
    return 0;

  for (i = 0; i < MAXJOBS; i++) {
    if (jobs[i]->pgid == pid) {
      clearjob(&jobs[i]);
      nextjid = maxjid(jobs)+1;
      return 1;
    }
  }
  return 0;
}
*/

/* fgpid - Return PID of current foreground job, 0 if no such job */
/*
pid_t fgpid(job *jobs) {
  int i;

  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i]->state == FG)
      return jobs[i]->pid;
  return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
/*
job *getjobpid(job *jobs, pid_t pid) {
  int i;

  if (pid < 1)
    return NULL;
  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i]->pid == pid)
      return &jobs[i];
  return NULL;
}


/* getjobjid  - Find a job (by JID) on the job list */
/*
job *getjobjid(job *jobs, int jid) 
{
  int i;

  if (jid < 1)
    return NULL;
  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].jobID == jid)
      return &jobs[i];
  return NULL;
}

/* Map process ID to job ID */
/*
int pid2jid(pid_t pid) 
{
  int i;

  if (pid < 1)
    return 0;
  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i]->pgid == pid) {
      return jobs[i]->jobID;
    }
  return 0;
}



/* Kill the job whose jobID=jid from the job list */
int killjob(job *jobs, int jid)
{
  job *j;
  for(j=jobs; j; j=j->next) {
    if(j->jobID == jid) {
      // send the TERM(terminate) signal
      // job is also sent a CONT(continue) signal

      return 1;
    }
  }
  return 0;
}



/* Print the job list */
void listjobs(job *j) 
{
  job *i;   
  for (i = j; i; i=i->next) {
    // if (i->pgid != 0) {
      printf("[%d] %d", i->jobID, i->pgid);
      // }
  }
}
/******************************
 * end job list helper routines
 ******************************/







/* Find the active job with the indicated pgid.  */
job * find_job (pid_t pgid, job* fjob)
{
  job *j;

  for (j = fjob; j; j = j->next)
    if (j->pgid == pgid)
      return j;
  return NULL;
}


/* Return true if all processes in the job have stopped or completed.  */
int job_is_stopped (job *j)
{
  process *p;

  for (p = j->first_process; p; p = p->next)
    if (!p->completed && !p->stopped)
      return 0;
  return 1;
}




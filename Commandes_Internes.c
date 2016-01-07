#include "Commandes_Internes.h"

#include "Shell.h"
#include "Affichage.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

// Trouver méthode pour imprimer "Command not found"
void expression_ou(Expression *e) {
	  if ((execvp (e->gauche->arguments[0], e->gauche->arguments)) != -1) 
            expression_simple(e->gauche);
          else {
            perror(e->gauche->arguments[0]);
	    if ((execvp (e->droite->arguments[0], e->droite->arguments)) != -1)
              expression_simple(e->droite);
            else 
              perror(e->droite->arguments[0]);      
      }
}


int expression_commande_interne(Expression *e) {
         
       if (strcmp(e->arguments[0],"echo") == 0) {
         execvp("/bin/echo", e->arguments); 
         return 1;
       }
       else if (strcmp(e->arguments[0],"pwd") == 0) {
         execvp("/bin/pwd", e->arguments); 
         return 1;
       }
       else if (strcmp(e->arguments[0],"date") == 0) {
         execvp("/bin/date", e->arguments); 
         return 1;
       }
       else if (strcmp(e->arguments[0],"hostname") == 0) {
         execvp("/bin/hostname", e->arguments); 
         return 1;
       }
      else if (strcmp(e->arguments[0],"kill") == 0) {
         execvp("/bin/kill", e->arguments); 
         return 1;
       }
      else if (strcmp(e->arguments[0],"history") == 0) {
         execvp("/bin/history", e->arguments); 
         return 1;
       }

      return 0;
  }


void expression_simple(Expression *e) {
   int commandeInterne = expression_commande_interne(e);
   if (commandeInterne == 0) {
          int status;
	    pid_t pid = fork();
	    if (pid == 0) {
	      execvp (e->arguments[0], e->arguments);
	      perror(e->arguments[0]);
	      exit(1);
	    }
	    else {
	      waitpid(pid, &status, 0);
	      return;
	    }
	    printf("%s\n", execv(e->arguments[0], e->arguments));
 }
}


void expression_pipe(Expression *e) {
  int tube[2], enregistrer, status;
  pipe(tube);  
  pid_t pid = fork();
  enregistrer = dup(1);
  
  if (pid == 0) {
    close(tube[0]);
    enregistrer = dup(1);
    dup2(tube[1], 1); 
    close(tube[1]);
    execvp (e->gauche->arguments[0], e->gauche->arguments);
    dup2(1, enregistrer);
  }
  else {
   wait(&status);
   close(tube[1]);
   dup2(tube[0], 0);
   dup2(1, enregistrer);

   execvp (e->droite->arguments[0], e->droite->arguments);
   close(tube[0]);
  }
}

 void expression_redirection_fichier(Expression *e) {
  int tube[2], enregistrer, status, fd, nmLus, N = 4;
  char aLire[N]; 
  
  pipe(tube);  
  pid_t pid = fork();
  enregistrer = dup(1);
  
  if (pid == 0) {
    close(tube[0]);
    enregistrer = dup(1);
    dup2(tube[1], 1); 
    close(tube[1]);
    execvp (e->gauche->arguments[0], e->gauche->arguments);
    dup2(1, enregistrer);
  }
  else {
   wait(&status);
   close(tube[1]);
   fd = open(e->arguments[0], O_CREAT | O_TRUNC | O_WRONLY, 0666); 
   while ( (nmLus = read(tube[0], aLire, N)) > 0)
     write(fd, aLire, nmLus);
   close(tube[0]); 
  } 

}

 void expression_redirection_fichier_append(Expression *e) {
  int tube[2], enregistrer, status, fd, nmLus, N = 4;
  char aLire[N]; 
  
  pipe(tube);  
  pid_t pid = fork();
  enregistrer = dup(1);
  
  if (pid == 0) {
    close(tube[0]);
    enregistrer = dup(1);
    dup2(tube[1], 1); 
    close(tube[1]);
    execvp (e->gauche->arguments[0], e->gauche->arguments);
    dup2(1, enregistrer);
  }
  else {
   wait(&status);
   close(tube[1]);
   fd = open(e->arguments[0], O_CREAT | O_APPEND | O_WRONLY, 0666); 
   while ( (nmLus = read(tube[0], aLire, N)) > 0)
     write(fd, aLire, nmLus);
   close(tube[0]); 
  } 

}



void expression_redirection_commande(Expression *e) {
  int tube[2], enregistrer, enregistrerSTDOUT, status, fd, nmLus, N = 6;
  char aLire[N];
  pipe(tube);  
  pid_t pid = fork();
  enregistrer = dup(0);
  enregistrerSTDOUT = dup(1);
 
 
  if (pid == 0) {
    close(tube[0]);
    fd = open(e->arguments[0], O_RDONLY, 0666); 
    while ( (nmLus = read(fd, aLire, N)) > 0) {
      write(tube[1], aLire, N);
     }
    close(tube[1]);
    close(fd);
    exit(1);
  }
  else { 
   wait(&status);
    close(tube[1]);
    enregistrer = dup(0);    
    dup2(tube[0], 0);
    execvp (e->gauche->arguments[0], e->gauche->arguments);
    close(tube[0]);
    dup2(0, enregistrer); 
  } 
}


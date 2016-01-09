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
#include <readline/history.h>


void expression_ou(Expression *e) {
	  if (evaluer_expr(e->gauche) == -1) {
            return evaluer_expr(e->droite);
            }
          else {
           return 1;
          }
}


int expression_et(Expression *e) {
     if (evaluer_expr(e->gauche) != -1)
       return evaluer_expr(e->droite);
     else
       return -1;
}


int expression_sequence(Expression *e) {
  evaluer_expr(e->gauche);
  return evaluer_expr(e->droite);
}

int expression_sous_shell(Expression *e) {
  return evaluer_expr(e->gauche);
}

int expression_simple(Expression *e) {
   if ((strcmp(e->arguments[0],"echo") == 0) || (strcmp(e->arguments[0],"kill") == 0) || (strcmp(e->arguments[0],"pwd") == 0) 
        || (strcmp(e->arguments[0],"date") == 0 ) || (strcmp(e->arguments[0],"hostname") == 0)) {
	   char str[100];
	   strcpy(str, e->arguments[0]);
	   for (int i = 1; e->arguments[i] != NULL; i++) {
	     strcat(str, " ");
	     strcat(str, e->arguments[i]);
           }
	   system(str);
       }
       else if (strcmp(e->arguments[0],"history") == 0){
	 commande_history();
       }
       else if (strcmp(e->arguments[0],"exit") == 0){
		exit(EXIT_SUCCESS);
	}

	else if (strcmp(e->arguments[0],"cd") == 0){
           char str[100];
	   strcpy(str, e->arguments[1]);	
           chdir(str);
        }
       else {
          int status;
	    pid_t pid = fork();
	    if (pid == 0) {
	      if (execvp (e->arguments[0], e->arguments) == -1) {
	        perror(e->arguments[0]);
                return -1;
              }
	    }
	    else {
	      waitpid(pid, &status, 0);
	      return 1;
	    }
	    printf("%s\n", execv(e->arguments[0], e->arguments));
            return 1;
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
   close(fd);
   close(tube[0]); 
  } 
}



 void expression_redirection_fichier_stderr(Expression *e) {
  int tube[2], enregistrer, status, fd, nmLus, N = 4;
  char aLire[N]; 
  
  pipe(tube);  
  pid_t pid = fork();
  enregistrer = dup(2);
  
  if (pid == 0) {
    close(tube[0]);
    enregistrer = dup(2);
    dup2(tube[1], 2); 
    close(tube[1]);
    execvp (e->gauche->arguments[0], e->gauche->arguments);
    dup2(2, enregistrer);
  }
  else {
   wait(&status);
   close(tube[1]);
   fd = open(e->arguments[0], O_CREAT | O_TRUNC | O_WRONLY, 0666); 
   while ( (nmLus = read(tube[0], aLire, N)) > 0)
     write(fd, aLire, nmLus);
   close(tube[0]); 
   close(fd);

  } 
}



 void expression_redirection_fichier_stderr_stdout(Expression *e) {
  int tube[2], enregistrer, enregistrer2, status, fd, nmLus, N = 4;
  char aLire[N]; 
  
  pipe(tube);  
  pid_t pid = fork();
  enregistrer = dup(2);
  enregistrer2 = dup(1);
  if (pid == 0) {
    close(tube[0]);
    enregistrer = dup(2);
    enregistrer2 = dup(1);

    dup2(tube[1], 2); 
    dup2(tube[1], 1); 

    close(tube[1]);
    execvp (e->gauche->arguments[0], e->gauche->arguments);
    dup2(2, enregistrer);
    dup2(1, enregistrer2);

  }
  else {
   wait(&status);
   close(tube[1]);
   fd = open(e->arguments[0], O_CREAT | O_TRUNC | O_WRONLY, 0666); 
   while ( (nmLus = read(tube[0], aLire, N)) > 0)
     write(fd, aLire, nmLus);
   close(tube[0]); 
    dup2(2, enregistrer);

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


void expression_arriere_plan(Expression *e) {
  int status;
  pid_t pid = fork();
  if (pid == 0) {
    expression_simple(e->gauche);
    setpgid(0, 0);
  }
}

void commande_history() {
  HIST_ENTRY ** historique = history_list();
  for (int i = 0; historique[i] != NULL; i++) {
    printf("%d  %s\n", i,historique[i]->line);
  }
}

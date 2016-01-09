#include "Shell.h"
#include "Evaluation.h"
#include "Commandes_Internes.h"




int
evaluer_expr(Expression *e)
{
  int arret = 0;
  if (e == NULL) 
    return;
 
  switch(e->type){
	case VIDE :
	  break;
	
	case SIMPLE : 
	  return expression_simple(e);     
		break;
        case SEQUENCE :
          return expression_sequence(e);
          break;
	case SEQUENCE_OU : 
          return expression_ou(e);
          break; 
        case SEQUENCE_ET : 
          return expression_et(e);
          break;
        case PIPE: 
          expression_pipe(e);
          break;
	case REDIRECTION_I: 
          expression_redirection_commande(e);
          break;  	
	case REDIRECTION_O: 		
          expression_redirection_fichier(e);		
          break;
	case REDIRECTION_A:
          expression_redirection_fichier_append(e);
          break;		
	case REDIRECTION_E:
          expression_redirection_fichier_stderr(e);
          break; 
	case REDIRECTION_EO :
          expression_redirection_fichier_stderr_stdout(e);

	  break;
	case BG:	
          expression_arriere_plan(e);
          break;
	case SOUS_SHELL:
          	return expression_sous_shell(e);
	  break;
	default : 
         fprintf(stderr,"fonctionnalité non implémentée\n");
    } 
  return 1;
}

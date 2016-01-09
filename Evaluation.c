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
          break; // redirection de l'entr�e (<)	 	
	case REDIRECTION_O: // redirection de la sortie (>)		
          expression_redirection_fichier(e);		
          break;
	case REDIRECTION_A: // redirection de la sortie en mode APPEND (>>)
          expression_redirection_fichier_append(e);
          break;		
	case REDIRECTION_E: // redirection de la sortie erreur
	case REDIRECTION_EO : // redirection des sorties erreur et standard.
	  break;
	case BG: // tache en arriere plan (&)	
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

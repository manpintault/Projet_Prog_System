#include "Shell.h"
#include "Evaluation.h"
#include "Commandes_Internes.h"




int
evaluer_expr(Expression *e)
{
  if (e == NULL) 
    return;
 
  switch(e->type){
	case VIDE :
	  break;
	
	case SIMPLE : 
		expression_simple(e); 
		break;
        case SEQUENCE : 
          	evaluer_expr(e->gauche);
		evaluer_expr(e->droite);
                break;
	case SEQUENCE_OU : 
                expression_ou(e);
		break; 
        case SEQUENCE_ET : 
		evaluer_expr(e->gauche);
		evaluer_expr(e->droite);
                break;
        case PIPE: 
          expression_pipe(e);
          break;
	case REDIRECTION_I: 
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
	case SOUS_SHELL:
	  break;
	default : 
         fprintf(stderr,"fonctionnalité non implémentée\n");
    } 
  return 1;
}

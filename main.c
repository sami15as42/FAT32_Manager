#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "afficher_fdel.h"
#include "liste_disques.h"
#include "afficher_secteur.h"

extern void initialiser(File *);
extern void enfiler(File *, int, char *);
extern int defiler(File *, char *);
extern unsigned char * lire_secteur_partition(char *, int, int);
extern long int lire_octets(unsigned char *, int, int);
extern long int adresse_lba_cluster_i(unsigned char *, int);
extern void informations(unsigned char *, int, File *, char *);
extern long int num_cluster_suivant(char *, int, long int);
extern int informations_entree(unsigned char *, int);
extern void afficher_infos_rep(char *, int, unsigned char *, int, char *, int, File *);
extern void Afficher_Fdel(char *, int);
extern int taille_secteur_physique(char *);
extern unsigned char* lire_secteur(char *, int);
extern void afficher_secteur(char *, int);
extern void liste_disques();
    
int main()
{
	liste_disques(); //Affichage des disques physiques connectés au début du programme principal.
	int stop = 0; //Lorsque stop est à 1, le programme principal s'arrete.
	int choix; //Une Variable contenant le choix de l'utilisateur. 
	char entree_utilisateur[100] = ""; //Chaine de caractères contenant l'entrée de l'utilisateur.
	char disque[3] = "";
	int num_partition;
	int num_secteur;
	
	while(!stop) /* tant que stop est différent de 1 on continue l'exécution. */ 
	{
		/* Affichage menu */
		printf("\n*** MENU ***\n");
		printf("1.Afficher la liste des disques physiques connectés.\n");
		printf("2.Afficher un secteur d'un disque physique.\n");
		printf("3.Afficher les fichiers et répertoires d'une partition FAT32.\n");
		printf("4.Quitter\n");
		printf("Entrer votre choix : ");
		scanf("%s", entree_utilisateur); //Choix de l'utilisateur.
		choix = atoi(entree_utilisateur); //Convertir l'entrée de l'utilisateur à un entier.
		printf("\n");
		/* Exécution de la procédure adéquate selon le choix de l'utilisateur. */ 
		switch(choix)
		{
			case 1:
				liste_disques();
				break;

		    case 2:
		    	printf("Entrer le nom du disque physique : ");
		    	scanf("%s", disque);
		    	printf("Entrer le numéro du secteur : ");
		    	scanf("%d", &num_secteur);
		    	printf("\n");
		    	afficher_secteur(disque, num_secteur);
		        break;
	 
		    case 3:
		    	printf("Entrer le nom du disque physique : ");
		    	scanf("%s", disque);
		    	printf("Entrer le numéro de la partition FAT32 : ");
		    	scanf("%d", &num_partition);
		    	printf("\n"); 
		    	Afficher_Fdel(disque, num_partition);
		        break;
 
         	case 4:
            	stop = 1;
            	break;
 
         	default:
         		strcpy(entree_utilisateur,"");
            	printf("Choix incorrecte !\n");
      	}
   	}
	return 0;
}

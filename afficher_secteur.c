#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "afficher_secteur.h"

/* Cette fonction retourne la taille des secteurs physiques du disque physique en entrée (à savoir 512 ou 4096 octets). */
int taille_secteur_physique(char disque_physique[])
{
    FILE *f = NULL;
    char taille_sect_chaine[4] = ""; //Variable pour sauvegarder la taille sous format chaine de caractères.
    char chemin[100] = "/sys/class/block/";
    strcat(chemin, disque_physique);
    strcat(chemin, "/queue/physical_block_size");
    f = fopen(chemin, "r"); //Ouverture du fichier "/sys/class/block/disque_physique/queue/physical_block_size" contenant la taille d'un secteur dans les systèmes Linux.
    if (f!=NULL)
    {
		fgets(taille_sect_chaine,4,f); //Lecture de la taille depuis le fichier ouvert précédemment.
		if(strlen(taille_sect_chaine)==0)
		{
		    printf("Erreur lors de la recherche de la taille des secteurs\n");
		    return -1;
		}
		fclose(f);
		return atoi(taille_sect_chaine); //Transformer la chaine de caractères contenant la taille en entier.
	}
	else 
	{
		printf("Erreur lors de la recherche de la taille des secteurs\n");
		return -1;
	}
}
/* Cette fonction lit le secteur "num_sect" du disque "disque_physique" en retournant le pointeur du debut de ce secteur (le numéro de secteur est en adresse LBA .i.e le premier secteur c'est le numéro 0). */
unsigned char* lire_secteur(char disque_physique[], int num_sect) 
{
    int taille_secteur = taille_secteur_physique(disque_physique); //Récupération de la taille des secteurs du disque "disque_physique".
    if (taille_secteur!=-1)
    {
		static unsigned char buffer[4096];
		FILE *disque = NULL;
		int n;
		char chemin[100] = "/dev/";
		strcat(chemin, disque_physique); 
		disque = fopen(chemin, "rb"); //Ouverture du disque physique.
		if(disque == NULL) {printf("Le disque n'est pas ouvert ou il n'existe pas.\n");return NULL;}
		else
		{
		    /* Positionnement au début du secteur souhaité (le secteur "num_secteur"). */
		    /* Le plus grand déplacement supporté par fseek est 2 147 483 647. */
		    long long deplacement = (long long) taille_secteur * num_sect;
			long quotient = deplacement/2147483647;
			long reste = deplacement - quotient * 2147483647; 
			int n = fseek(disque,reste,SEEK_SET);
			if(n<0)
		    {
		        printf("Erreur de fseek = %d \n",n);
		        fclose(disque);
		        return NULL;
		    }
			for (int i=0; i<quotient; i++)
			{
				n = fseek(disque,2147483647,SEEK_CUR);
				if(n<0)
				{
				    printf("Erreur de fseek = %d \n",n);
				    fclose(disque);
				    return NULL;
				}
			}
		    //Lecture du secteur dans le Buffer. 
		    n = fread(buffer, taille_secteur, 1, disque);
		    if(n<=0)
		    {
		        printf("Erreur de fread = %d \n",n);
		        fclose(disque);
		        return NULL;
		    }
		}
		fclose(disque);
		//Renvoyer le buffer contenant le contenu du secteur. 
		return buffer;
	}
	else return NULL;
}

/* Cette fonction se charge d'afficher le secteur "num_sect" du disque "disque_physique" dans la console (le numéro de secteur est en adresse LBA .i.e le premier secteur c'est le numéro 0). */
void afficher_secteur(char disque_physique[], int num_sect)
{
    //Recuperation du contenu du secteur dans le pointeur "secteur" par le biais de la fonction "lire_secteur".
    unsigned char* secteur = lire_secteur(disque_physique,num_sect); 
    if (secteur!=NULL)
    {
        //Récupération de la taille des secteurs du disque "disque_physique".
	int taille_secteur = taille_secteur_physique(disque_physique);
	printf("Affichage du secteur %d de disque %s\n", num_sect, disque_physique); 
        //Affichage du contenu du secteur avec le format demandé.
	for (int j = 0; j < taille_secteur/16; j++)
	{
            printf("%04d ", j*16);
            for (int i = 0; i < 16; i++) printf("%02x ", secteur[j*16+i]);
            printf("\n");
	}
	printf("\n");
    }
    else printf("Erreur lors de la lecture d'un secteur\n");
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "liste_disques.h"


/* Cette fonction permet d'afficher les differents disques physiques connectés à la machine. */
void liste_disques()
{
    char type_disques_physiques[2][4] = {"sd","hd"}; //Les types des disques physiques sdN et hdN.
    char disque_physique[4] = "";
    char chemin[8] = "";
    printf("Les disques physiques connectés à la machine sont : \n");
    int longeur_type_disques_physiques = sizeof(type_disques_physiques)/sizeof(type_disques_physiques[0]);
    for (int j=0; j<longeur_type_disques_physiques; j++)
    {
        /* Pour chaque fichier de type sd* ou hd* on concatène à chaque fois une lettre de l'alphabet (code ascii de 97 à 122). */
        for (int i=97; i<123; i++)
        {
            errno = 0; /* Cette variable globale de la bibliothèque standard "errno.h" contient le code de la dernière erreur ayant était déclenchée dans le système (dans le cas d'absence d'erreurs errno est à la valeur 0). */
            strcpy(disque_physique,type_disques_physiques[j]);
            disque_physique[2] = (char) i;
            disque_physique[3] = '\0';
            strcpy(chemin,"/dev/");
            strcat(chemin,disque_physique);
            /* On essaie d'ouvrir le fichier "sdi" si le fichier s'ouvre correctement c'est qui'il existe et donc l'existence du disque physique (on affiche le nom du disque par la suite). */
            FILE * fichier = fopen(chemin, "r");
            /* On pourrait tomber dans le cas où la non-ouverture du fichier est du à un autre problème autre que la non existence de ce dernier. Pour remedier à cela on vérifie le contenu de la variable globale "errno" si celle ci est égale à 2 alors l'erreur de l'ouverture du fichier est du à son non-existence sinon le fichier existe mais peut-etre "fopen" n'a pas pu l'ouvrir à cause de problèmes de permission ou d'insuffisance d'espace mémoire. */
            if (errno!=2)
            {
            	printf("%s\n", chemin);
            	if (fichier!=NULL) fclose(fichier);
            }
        }
    }
    printf("\n");
}

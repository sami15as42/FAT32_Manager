#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "afficher_fdel.h"

/***************************************************************************************************************/
/***************************************************************************************************************/
/* Element est une structure contenant un entier qui sauvegarde le numéro du cluster et une chaine de          */
/* caractères pour sauvegarder le nom du répertoire père.                                                      */ 
/***************************************************************************************************************/
/***************************************************************************************************************/

typedef struct Element Element;
struct Element
{
    int num_cluster;
    char repertoire_pere[256];
    Element *suivant;
};

/*****************************************************************************************************************/
/*****************************************************************************************************************/
/* File est une file d'attente, elle est représenté à travers deux pointeurs "tete" et "queue" vers la structure */ 
/* Element. "tete" est utilisé pour savegarder la tete de la file (le premier élément inséré) et "queue" pour    */
/* sauvegarder le dernier élément inséré.                                                                        */
/*****************************************************************************************************************/
/*****************************************************************************************************************/

typedef struct File File;
struct File
{
    Element * tete; //Pointeur vers premier "Element" inséré dans la file.
    Element * queue; //Pointeur vers dernier "Element" inséré dans la file.
};

/***************************************************************************************************************/
/***************************************************************************************************************/
/* Cette procédure permet d'initialiser une nouvelle file en affectant NULL à "tete" et "queue".               */
/***************************************************************************************************************/
/***************************************************************************************************************/

void initialiser(File *file)
{
    file->tete = NULL;
    file->queue = NULL;
}

/***************************************************************************************************************/
/***************************************************************************************************************/
/* Cette procédure permet d'enfiler un nouveau élément contenant les valeurs "nv_num_cluster" et               */
/* "nv_repertoire_pere" dans la file "file".                                                                   */
/***************************************************************************************************************/
/***************************************************************************************************************/

void enfiler(File *file, int nv_num_cluster, char nv_repertoire_pere[])
{
    Element *nouveau = malloc(sizeof(*nouveau)); //Création d'un nouveau élément "nouveau".
    if (file == NULL || nouveau == NULL) //File inexistante ou erreur lors de la création d'un nouveau élément. 
    {
        exit(EXIT_FAILURE);
    }
	
	//Initialisation de l'élément "nouveau" avec "nv_num_cluster" et "nv_repertoire_pere".	
    nouveau->num_cluster = nv_num_cluster;
    strcpy(nouveau->repertoire_pere, nv_repertoire_pere);
    nouveau->repertoire_pere[strlen(nv_repertoire_pere)] = '\0';
    nouveau->suivant = NULL;

    if (file->queue != NULL) /* La file n'est pas vide */
    {
        (file->queue)->suivant = nouveau; 
        file->queue = nouveau;
    }
    else /* La file est vide, notre élément est le premier */
    {
        file->tete = nouveau;
        file->queue = nouveau;
    }
}

/***************************************************************************************************************/
/***************************************************************************************************************/
/* Cette fonction permet de defiler un élément à partir de la tete de la file "file" et retourner le numéro de */
/* cluster et le nom du répertoire père de cet élément.                                                        */ 
/***************************************************************************************************************/
/***************************************************************************************************************/

int defiler(File *file, char * repertoire_pere_defile)
{
    if (file == NULL) //File inexistante.
    {
        exit(EXIT_FAILURE);
    }

    int num_cluster_defile = 0;

    /* On vérifie s'il y a quelque chose à défiler */
    if (file->tete != NULL)
    {
        Element *elementDefile = file->tete;

        num_cluster_defile = elementDefile->num_cluster;
        strcpy(repertoire_pere_defile, elementDefile->repertoire_pere);
        file->tete = elementDefile->suivant; //Modifier la tete de notre file.
        free(elementDefile); //Libérer l'élément de la file.
    }
    else return -1; //Dans le cas où la file est vide on retourne -1.

    return num_cluster_defile;
}

/***************************************************************************************************************/
/***************************************************************************************************************/
/* Cette fonction permet de retourner un tableau de 512 "unsigned char", ce tableau est le secteur numéro      */
/* "num_secteur" (adresse LBA .i.e premier secteur à le numéro 0) de la partition "num_partition" du disque    */
/* "disque_physique".                                                                                          */
/***************************************************************************************************************/
/***************************************************************************************************************/

unsigned char * lire_secteur_partition(char disque_physique[], int num_partition, int num_secteur)
{
    static unsigned char secteur[512]; //Tableau qui contiendra le secteur.
    FILE * partition = NULL;
    char chemin[100] = "/dev/"; //chemin est une chaine de caractères qui contiendra le chemin absolu vers notre partition.
    strcat(chemin, disque_physique); //Ajout de nom du disque physique à chemin.
    char partition_chaine[2] = "";
    sprintf(partition_chaine, "%d", num_partition); 
    strcat(chemin, partition_chaine); //Ajout du numéro de la partition au chemin.
    partition = fopen(chemin, "rb"); //Ouverture de la partition dont le chemin absolu est dans la variable "chemin".
    if(partition == NULL) {printf("La partition n'est pas ouverte ou elle n'existe pas\n"); return NULL;}
    else
    {
        /* Positionnement au début de notre secteur. */
    	/* Le plus grand déplacement supporté par fseek est 2 147 483 647. */
    	long long deplacement = (long long) 512 * num_secteur;
    	long quotient = deplacement/2147483647;
    	long reste = deplacement - quotient * 2147483647; 
    	int n = fseek(partition,reste,SEEK_SET);
    	if(n<0)
        {
            printf("Erreur de fseek = %d \n",n);
            fclose(partition);
            return NULL;
        }
    	for (int i=0; i<quotient; i++)
    	{
    		n = fseek(partition,2147483647,SEEK_CUR);
    		if(n<0)
		    {
		        printf("Erreur de fseek = %d \n",n);
		        fclose(partition);
		        return NULL;
		    }
    	}
        n = fread(secteur, 512, 1, partition); //La lecture du secteur.
        if(n<=0)
        {
            printf("Erreur de fread = %d \n",n); //Erreur lors de la lecture.
            fclose(partition);
            return NULL;
        }
        fclose(partition);
        return secteur; //Retourner le secteur.    
    }
}

/***************************************************************************************************************/
/***************************************************************************************************************/
/* Cette fonction permet de lire à partir d'un buffer "n" octets dont les adresses début et fin dans le buffer */
/* sont "octet_debut" et "octet_fin", et retourner la valeur de ces "n" octets en entier format long.          */
/***************************************************************************************************************/
/***************************************************************************************************************/

long int lire_octets(unsigned char * buffer, int octet_debut, int octet_fin)
{
    char resultat[100] = "";
    char octet[4] = "";
    for(int k = octet_fin; k >= octet_debut; k--) //On commence la lecture à partir de la fin (little indian).
    {
        sprintf(octet, "%02x", buffer[k]); //Lecture d'un octet et le transformer en hexa.
        strcat(resultat, octet); //Ajout de l'octet lu dans résulat.
    }
    return strtol(resultat, NULL, 16); //Conversion du résultat du hexa vers entier long.
}

/****************************************************************************************************************/
/****************************************************************************************************************/
/* Cette fonction permet de retourner l'adresse LBA du cluster "i" en donnant à cette fonction le secteur boot. */
/****************************************************************************************************************/
/****************************************************************************************************************/

long int adresse_lba_cluster_i(unsigned char * secteur_boot, int i)
{
    //Adresse LBA du premier cluster (généralement cluster 2) = nombre de secteurs réservés (octets 14-15) + nombre FAT (octet 16) * nombre secteurs par FAT (octets 36-39)
    long int adresse_lba_premier_cluster = lire_octets(secteur_boot,14,15) + lire_octets(secteur_boot,16,16) * lire_octets(secteur_boot,36,39);
    //Adresse LBA d'un cluster i = adresse LBA premier cluster + (i - 2) * nombre de secteurs par cluster (octet 13) 
    return adresse_lba_premier_cluster + (i - lire_octets(secteur_boot,44,47)) * lire_octets(secteur_boot,13,13);
}      

/****************************************************************************************************************/
/****************************************************************************************************************/
/* Cette procédure permet d'afficher les différentes informations d'un fichier, et ça en la donnant un ensemble */
/* d'entrées de de ce fichier. Dans le cas où le fichier est un répertoire, on affiche ses informations et on   */
/* empile son numéro de premier cluster et le nom du répertoire père dans la file "maFile" pour afficher par la */
/* suite son contenu.                                                                                           */
/* - "nb_entree" c'est le nombre d'entrées (format long et court) dans "buffer"                                 */
/****************************************************************************************************************/
/****************************************************************************************************************/
  
void informations(unsigned char * buffer, int nb_entree, File * maFile, char * repertoire_pere)
{
    int tab[13] = {1, 3, 5, 7, 9, 14, 16, 18, 20, 22, 24, 28, 30}; //Ce tableau contient les positions des différents code UTF-16 du nom format long d'une entrée.
    char nom_format_court[12] = ""; //Nom format court sur 11 caractères max.
    char nom_format_long[256] = ""; //Nom format long sur 255 caractères max.
    char poids_fort_hex[3] = "0";
    char poids_faible_hex[3] = "0";
    char caractere;
    char type_fichier[16] = "";
    char octet[3] = "";
    char octet1[3] = "";
    char octet2[3] = "";
    char resultat[5] = "";
    int poids_fort;
    int poids_faible;
    char premier_cluster[10] = "";
    //Ci-dessous le bloc de code qui permet de trouver le type du fichier.
    //Le type est indiqué dans le 12 ème octet de l'entrée format court, si bit3 = 1 alors c'est un label de volume sinon il y a deux cas, si bit4 = 1 alors c'est répertoire sinon c'est un fichier ordinaire.
    sprintf(octet, "%02x", buffer[11]);
    sprintf(poids_fort_hex,"%c",octet[0]);
    sprintf(poids_faible_hex,"%c",octet[1]);
    poids_fort = (int) strtol(poids_fort_hex,NULL,16);
    poids_faible = (int) strtol(poids_faible_hex,NULL,16);
    if (poids_fort%2==1) strcpy(type_fichier,"Repertoire");
    else
    {
        if (poids_faible>=8) strcpy(type_fichier,"Label de volume");
        else strcpy(type_fichier,"Fichier");
    }
    //Ci-dessous le bloc de code qui permet de trouver le nom du fichier format court.
    for (int i = 0; i<12; i++)
    {
        sprintf(octet, "%02x", buffer[i]);
        caractere = (char) (int) strtol(octet,NULL,16);
        nom_format_court[i] = caractere;
    }
    nom_format_court[11] = '\0';
    if ((strcmp(nom_format_court,".          ")!=0) && (strcmp(nom_format_court,"..         ")!=0)) /* Dans le cas où le nom du fichier est différent de "." et ".." alors on cherche le nom format long, la taille et le numéro du premier cluster de ce fichier sinon on affiche rien parce que les répertoires "." et ".." ont déjà été mentionné. */ 
    {
        //Ci-dessous le bloc de code qui permet de trouver le nom format long du fichier dans le cas où "buffer" possède plus d'une seule entrée.
        int cpt = 0;
        for (int i = 1; i<nb_entree; i++) //Boucle "for" de "nb_entree-1" itérations pour lire les informations des entrées format long.
        {
            for (int j = 0; j < 13; j++)
            {
                sprintf(octet2, "%02x", buffer[tab[j]+i*32]);
                if (strcmp(octet2,"ff")==1) break;
                sprintf(octet1, "%02x", buffer[tab[j]+1+i*32]);
                strcpy(resultat,octet1);
                strcat(resultat,octet2);
                caractere = (char) (int) strtol(resultat,NULL,16);
                nom_format_long[cpt] = caractere;
                cpt++;
            }
        }
        nom_format_long[cpt] = '\0';
        //Ci-dessous le bloc de code qui permet de trouver la taille du fichier en octets.
        long int taille_fichier = lire_octets(buffer,28,31); // La taille est représentée par les octets 28 jusqu'à 31 d'une entrée format court en little indian, mais la fonction "lire_octets" retourne la bonne valeur en octets.
        //Ci-dessous le bloc de code qui permet de trouver le numéro du premier cluster dans la FAT.
        sprintf(octet, "%02x", buffer[21]);
        strcat(premier_cluster, octet);
        sprintf(octet, "%02x", buffer[20]);
        strcat(premier_cluster, octet);
        sprintf(octet, "%02x", buffer[27]);
        strcat(premier_cluster, octet);
        sprintf(octet, "%02x", buffer[26]);
        strcat(premier_cluster, octet);
        long int num_premier_cluster = strtol(premier_cluster, NULL, 16);
        //Ci-dessous le bloc de code qui permet d'afficher les différentes informations du fichier.
        printf("Nom du répertoire père : %s\n", repertoire_pere);
        printf("Type du fichier : %s\n",type_fichier);
        printf("Nom du fichier (format court) : %s\n", nom_format_court);
        printf("Nom du fichier (format long) : %s\n", nom_format_long);
        printf("Taille du fichier : %ld octets\n", taille_fichier);
        printf("Numéro du premier cluster : %ld\n\n", num_premier_cluster);
        // Dans le cas où le fichier est un répertoire, on enfile le numéro du premier cluster de ce dernier avec son nom, qui deviendra par la suite, le nom du répertoire père des fichiers de ce répertoire.
        if (poids_fort%2==1)
        {
            if (strcmp(nom_format_long,"")==0) enfiler(maFile,(int) num_premier_cluster,nom_format_court);
            else enfiler(maFile,(int) num_premier_cluster,nom_format_long); 
        }
    } 
}

/***************************************************************************************************************/
/***************************************************************************************************************/
/* Cette fonction permet de retourner le numéro du cluster suivant du cluster "num_cluster". Pour le faire,    */
/* cette fonction calcul en premier temps l'adresse LBA de la FAT, après elle accède à la case "num_cluster"   */
/* de la FAT, en suite, elle récupère le numéro du prochain cluster dans le cas où le contenu de la case est   */
/* différent de "0x0ffffff0", "0x0ffffff6", "0x0ffffff7", "0x0ffffff8", "0x0ffffff0" et "0x00000000".          */
/***************************************************************************************************************/
/***************************************************************************************************************/
  
long int num_cluster_suivant(char disque_physique[], int num_partition, long int num_cluster)
{
    long int adresse_lba_fat = lire_octets(lire_secteur_partition(disque_physique,num_partition,0),14,15); //Adresse LBA de la FAT.
    unsigned char * secteur_fat = lire_secteur_partition(disque_physique,num_partition,adresse_lba_fat + num_cluster/128); //Lecture du secteur adéquat de la FAT (le secteur contenant la case "num_cluster").
    int pos = (num_cluster%128 * 4); //Position du premier octet de la case en question dans "secteur_fat".
    char resultat[10] = "";
    char octet[3] = "";
    for(int k = pos+3; k >= pos; k--)
    {
        sprintf(octet, "%02x", secteur_fat[k]);
        strcat(resultat, octet);
    }
    if (((strncmp(resultat,"0ffffff",7)==0) && (resultat[7]=='0' || resultat[7]=='6' || resultat[7]=='7' || resultat[7]=='8' || resultat[7]=='f')) || (strcmp(resultat,"00000000")==0)) return -1; 
    else return strtol(resultat, NULL, 16);
}

/***************************************************************************************************************/
/***************************************************************************************************************/
/* Cette fonction permet de retourner :                                                                        */
/*	1) 0 : si l'entrée est supprimé logiquement (premier octet de l'entrée égale à 0xE5).                      */
/*	2) -1 : si le contenu du répertoire est terminé (premier octet de l'entrée égale à 0x00).                  */
/*	3) 1 : si l'entrée est de format court.                                                                    */
/*	4) nb > 1 : si l'entrée est de format long (l'attribut LFN = 0x0F) de plus nb = le nombre d'entrées en     */
/* format long de ce fichier + 1 (entrée en format court).                                                     */
/***************************************************************************************************************/
/***************************************************************************************************************/
	
int informations_entree(unsigned char * buffer, int adresse_premier_octet_entree)
{
    char octet[3] = "";
    char demi_octet[3] = "0";
    demi_octet[2] = '\0';
    int resultat;
    sprintf(octet, "%02x", buffer[adresse_premier_octet_entree]);
    if (strcmp(octet,"e5")==0) return 0;
    else
    {
        if (strcmp(octet,"00")==0) return -1;
        else
        {
			sprintf(octet, "%02x", buffer[adresse_premier_octet_entree+11]);
			if (strcmp(octet,"0f")!=0) return 1;
			else
			{
		    	sprintf(octet, "%02x", buffer[adresse_premier_octet_entree]);
		        demi_octet[1] = octet[1];
		        resultat = strtol(demi_octet, NULL, 16);
		        demi_octet[1] = octet[0];
		        if (strtol(demi_octet, NULL, 16)%2==1) resultat = resultat + 16;
		        return resultat + 1;
			}
	    }
    }
}

/***************************************************************************************************************/
/***************************************************************************************************************/
/* Cette fonction permet d'afficher les informations sur les différents éléments (fichiers/répertoires) du     */
/* répertoire commançant au cluster "num_cluster" de la partition "num_partition" du disque "disque_physique". */
/* Le paramètre "repertoire_pere" c'est le nom du répertoire en question, "nb_secteurs" c'est le nombre de     */
/* secteurs par cluster.                                                                                       */ 
/***************************************************************************************************************/
/***************************************************************************************************************/

void afficher_infos_rep(char disque_physique[], int num_partition, unsigned char * secteur_boot, int num_cluster, char repertoire_pere[], int nb_secteurs, File * maFile)
{
    long int adresse = adresse_lba_cluster_i(secteur_boot,num_cluster);
    unsigned char * secteur = lire_secteur_partition(disque_physique,num_partition,adresse); //Premier secteur du cluster "num_cluster".
    int cpt = 0; //Numéro de l'entrée courante dans "secteur" (un secteur contient 16 entrées de 32 octets).
    int cpt_secteur = 0; //Numéro du secteur courant dans le cluster courant (un cluster contient "nb_secteurs" secteurs).
    int stop = 0; //Booléen d'arret, il est à 1 lorsqu'on arrive à une entrée où le premier octet est égale à 0x00.
    while(stop==0)
    {
        int info = informations_entree(secteur,cpt*32); //Appel de la fonction "informations_entree" pour nous informer sur le contenu de l'entrée "cpt". 
        if (info==-1) stop = 1; //Fin de notre répertoire.
        else
        {
            if (info==0) //Le cas d'une entrée supprimé.
            {
                cpt = (cpt+1)%16; //Incrémenter "cpt" pour pointer l'entrée suivante dans le secteur (modulo nombre d'entrées par secteur).
                if (cpt==0) //Dans le cas où "cpt" reviendra à 0. 
                {
                    cpt_secteur = (cpt_secteur+1)%nb_secteurs; //Incrémenter "cpt_secteur" pour pointer le prochain secteur à lire dans le cluster courant.
                    if (cpt_secteur!=0) //Lecture du prochain secteur du cluster courant. 
                    	secteur = lire_secteur_partition(disque_physique,num_partition,adresse+cpt_secteur);
                    else //Dans le cas où le cluster courant est terminé, on cherche le cluster prochain et on lit son premier secteur. 
                    {
                        num_cluster = (int) num_cluster_suivant(disque_physique,num_partition,num_cluster);
                        secteur_boot = lire_secteur_partition(disque_physique,num_partition,0);
                        adresse = adresse_lba_cluster_i(secteur_boot,num_cluster);
                        secteur = lire_secteur_partition(disque_physique,num_partition,adresse);
                    }
                }
            }
            else //Le cas d'une entrée valide. La variable "info" contient le nombre d'entrées d'un fichier.
            {
                unsigned char ent[672] = ""; //"ent" va contenir les "info" entrées du fichier, pour afficher les informations de ce dernier.  
                for (int i=info; i>0; i--)
                {
                    for (int j=0; j<32; j++) ent[(i-1)*32+j] = secteur[cpt*32+j]; //Lecture des 32 octets d'une entrée.   
                    //Pointer sur la prochaine entrée, dans le cas où le secteur est terminé on lit un autre secteur du cluster courant et dans le cas où le cluster est terminé on lit le cluster suivant en utilisant la fonction "num_cluster_suivant" (expliqué en détails ci-dessus).
                    cpt = (cpt+1)%16; 
                    if (cpt==0)
                    {
                        cpt_secteur = (cpt_secteur+1)%nb_secteurs;
                        if (cpt_secteur!=0) 
                            secteur = lire_secteur_partition(disque_physique,num_partition,adresse+cpt_secteur);
                        else
                        {
                            num_cluster = (int) num_cluster_suivant(disque_physique,num_partition,num_cluster);
                            secteur_boot = lire_secteur_partition(disque_physique,num_partition,0);
                            adresse = adresse_lba_cluster_i(secteur_boot,num_cluster);
                            secteur = lire_secteur_partition(disque_physique,num_partition,adresse);
                        }
		     		}
                }
                informations(ent,info,maFile,repertoire_pere); //Afficher les informations d'un fichier/répertoire, ces informations se trouvent dans le tableau "ent" contenant un ensemble d'entrées décrivant le fichier/répertoire. 
            }
        }
    }
}

/***************************************************************************************************************/
/***************************************************************************************************************/
/* Afficher les répertoires/fichiers de la partition "num_partition" du disque "disque_physique". Dans un      */
/* premier temps, on affiche les éléments du répertoire racine, puis on vérifie si la file n'est pas vide pour */
/* afficher les éléments du nouveau répertoire ainsi de suite...                                               */ 
/***************************************************************************************************************/
/***************************************************************************************************************/

void Afficher_Fdel(char disque_physique[], int num_partition)
{
    File * maFile = malloc(sizeof(*maFile)); // Création d'une variable de type "File".
    initialiser(maFile); //Initialisation de la file.
    unsigned char * secteur_boot = lire_secteur_partition(disque_physique,num_partition,0); //Lecture du secteur boot de notre partition.
    if (secteur_boot!=NULL)
    {
		int num_cluster = (int) lire_octets(secteur_boot,44,47); //Récupération du numéro du premier cluster.
		int nb_secteurs = (int) lire_octets(secteur_boot,13,13); //Récupération du numéro de secteurs par cluster. 
		char repertoire_pere[256] = "Racine"; //Nom du répertoire père courant.
		printf("Les répertoires et fichiers de la partition %s%d : \n",disque_physique,num_partition);
		while(num_cluster!=-1) /* Tant que le numéro du cluster est différent de -1 */ 
		{
		    secteur_boot = lire_secteur_partition(disque_physique,num_partition,0);
		    afficher_infos_rep(disque_physique,num_partition,secteur_boot,num_cluster,repertoire_pere,nb_secteurs,maFile); //Afficher les informations des éléments (fichiers/répertoires) du répertoire commençant au cluster "num_cluster".
		    num_cluster = defiler(maFile, repertoire_pere); //Defiler un élément de la file, si la file est vide, elle retourne -1 donc on a terminé de lire tous les répertoires sinon elle retourne le numéro du premier cluster d'un autre répertoire à lire.
		    printf("\n");
		}
	}
}

/***************************************************************************************************************/
/***************************************************************************************************************/

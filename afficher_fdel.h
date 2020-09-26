typedef struct Element Element;

typedef struct File File;

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

Voici ci-dessous les commandes utilisés pour la compilation, l'édition de liens et l'exécution :

gcc -c afficher_fdel.c

gcc -c liste_disques.c

gcc -c afficher_secteur.c

gcc -c main.c

gcc main.o afficher_fdel.o liste_disques.o afficher_secteur.o

sudo ./a.out

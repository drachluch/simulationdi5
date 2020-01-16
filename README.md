# simulationdi5

Une simulation orientée événement.

Il y a N membres du personnel qui répondent aux courriels et aux appels entre 8h et 12h.
Il y a Ntmax postes téléphoniques, donc au plus Ntmax employés peuvent répondre aux appels simultanément.
Nt est le nombre d'employés dédiés à la réponse aux appels à 8h.

SeedArrival est utilisé pour initialisé le générateur de nombres pseudo-aléatoires dédié aux tirages aléatoires concernant les arrivées des courriels et des appels.
SeedDeparture est utilisé pour initialisé le générateur de nombres pseudo-aléatoires dédié aux tirages des durées de traitements des courriels et des appels.

S est le nombre d'exécutions de la simulation avec le triplet (N, Ntmax, Nt).
Les SeedArrival' et SeedDeparture' utilisé sont changés pour chaque exécution.
Pour la nième exécution, SeedArrival' vaut SeedArrival + n et SeedDeparture' vaut SeedDeparture + n.

Le programme lit depuis l'entrée standard les paramètres N, Ntmax, Nt, S, SeedArrival et SeedDeparture dans cet ordre.
Les paramètres doivent être séparés par un espace, une tabulation ou un retour à la ligne.
Le programme exécute S fois la simulation avec ces paramètres, puis lit le jeu de paramètres suivant.
Quand SeedArrival vaut 0, le programme lui donne une nouvelle valeur aléatoire. Il en est de même pour SeedDeparture.

Le programme s'arrête quand il n'y a plus de paramètres à lire, quand la lecture des paramètres échoue, ou quand les conditions suivantes ne sont pas respectées :
- Ntmax <= N
- Nt <= Ntmax
- S > 0

Pour chaque jeu de paramètres, les informations écrites dans la sortie standard concernant les S simulations sont les suivantes :
- N ;
- Ntmax ;
- Nt ;
- S ;
- tM le nombre moyen de courriels qui sont arrivés ;
- aM le nombre moyen de courriels auxquels on a répondu ;
- wM le nombre moyen de courriels pour lesquels la réponse était en cours à 12h ;
- uM le nombre moyen de courriels auxquels on n'a pas répondu ;
- tC le nombre moyen d'appels qui sont arrivés ;
- aC le nombre moyen d'appels auxquels on a répondu ;
- wC le nombre moyen d'appels pour lesquels la réponse était en cours à 12h ;
- uC le nombre moyen d'appels auxquels on n'a pas répondu ;
- tmaM la moyenne du temps moyen entre l'arrivée d'un courriel et le début du processus de réponse à celui-ci ;
- tmaC la moyenne du temps moyen entre l'arrivée d'un appel et le début du processus de réponse à celui-ci ;
- oqP le taux moyen d'occupation du personnel ;
- oqT le taux moyen d'occupation des postes téléphoniques ;
- sA sert à initialiser le générateur de nombres pseudo-aléatoires dédié aux arrivées des courriels et des appels ;
- sD sert à initialiser le générateur de nombres pseudo-aléatoires dédié aux traitements des courriels et des appels ;
- etM l'écart-type du nombre de courriels qui sont arrivés ;
- eaM l'écart-type du nombre de courriels auxquels on a répondu ;
- ewM l'écart-type du nombre de courriels pour lesquels la réponse était en cours à 12h ;
- euM l'écart-type du nombre de courriels auxquels on n'a pas répondu ;
- etC l'écart-type du nombre d'appels qui sont arrivés ;
- eaC l'écart-type du nombre d'appels auxquels on a répondu ;
- ewC l'écart-type du nombre d'appels pour lesquels la réponse était en cours à 12h ;
- euC l'écart-type du nombre d'appels auxquels on n'a pas répondu ;
- etmaM l'écart-type du temps moyen entre l'arrivée d'un courriel et le début du processus de réponse à celui-ci ;
- etmaC l'écart-type du temps moyen entre l'arrivée d'un appel et le début du processus de réponse à celui-ci ;
- eoqP l'écart-type du taux d'occupation du personnel ;
- eoqT l'écart-type du taux d'occupation des postes téléphoniques ;

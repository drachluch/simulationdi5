# simulationdi5

A event oriented simulation.

There are N members of staff answering to mails and calls between 8h00 and 12h00.
There are Ntmax phone booths, therefore only Ntmax members can answer the phone simultaneously.
Nt is the number of members dedicated to answering the phone at 8h00.

SeedArrival is the seed used to generate the events of arrival of mails and calls.
SeedDeparture is the seed used to generate the events of end of answering of mails and calls.

S is how many time the simulation is played.
The nth time, the pair of seeds used is (SeedArrival + n; SeedDeparture + n).


The application listens to the standard input and read from it N, Ntmax, N, S, SeedArrival and SeedDeparture in this order.
It simulates S times with this parameters, then read again the parameters.
When SeedArrival is 0, it is given a new randomly generated value.
When SeedDeparture is 0, it is given a new randomly generated value.

The application stops when it can't read more parameters, when it fails to read a parameter, or when the parameters don't respect the following conditions :
- Ntmax <= N
- Nt <= Ntmax
- S > 0

For each set of simulations, it prints the following values :
- N ;
- Ntmax ;
- Nt ;
- S ;
- tM the mean number of arrived mails ;
- aM the mean number of answered mails ;
- wM the mean number of mails which had a member of the staff responding to it at 12h00 ;
- uM the mean number of unanswered mails ;
- tC the mean number of arrived calls ;
- aC the mean number of answered calls ;
- wC the mean number of calls which had a member of the staff responding to it at 12h00 ;
- uC the mean number of unanswered calls ;
- tmaM the mean "temps moyen entre l'arrivée d'un mail et le début du processus de réponse à ce mail" ;
- tmaC the mean "temps moyen entre l'arrivée d'un appel et le début du processus de réponse à cet appel" ;
- oqP the mean "taux d'occupation du personnel" ;
- oqT the mean "taux d'occupation des postes téléphoniques" ;
- sA the seed for the arrival events ;
- sD the seed for the departure events ;
- etM the standard deviation number of arrived mails ;
- eaM the standard deviation number of answered mails ;
- ewM the standard deviation number of mails which had a member of the staff responding to it at 12h00 ;
- euM the standard deviation number of unanswered mails ;
- etC the standard deviation number of arrived calls ;
- eaC the standard deviation number of answered calls ;
- ewC the standard deviation number of calls which had a member of the staff responding to it at 12h00 ;
- euC the standard deviation number of unanswered calls ;
- etmaM the standard deviation "temps moyen entre l'arrivée d'un mail et le début du processus de réponse à ce mail" ;
- etmaC the standard deviation "temps moyen entre l'arrivée d'un appel et le début du processus de réponse à cet appel" ;
- eoqP the standard deviation "taux d'occupation du personnel" ;
- eoqT the standard deviation "taux d'occupation des postes téléphoniques" ;

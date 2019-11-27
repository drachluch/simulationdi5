#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

enum class Event {
	begin,
	end,
	mailArrival,
	callArrival,
	mailAnswered,
	callAnswered
};

struct Resultat {
	unsigned int totalMails;
	unsigned int answeredMails;
	unsigned int unansweredMails;
	unsigned int workingMails;
	unsigned int totalCalls;
	unsigned int answeredCalls;
	unsigned int workingCalls;
	unsigned int unansweredCalls;
	float meanTimeToAnswerMail;
	float meanTimeToAnswerCall;
	float staffOccupationRate;
	float phoneBoothOccupationRate;
};

/**
 * N : effectif du personnel
 * Ntmax : nombre de postes téléphoniques
 * Nt : nombre d'humains affectés à un poste téléphonique
 */
Resultat simulate(unsigned int const N, unsigned int const Ntmax, unsigned int Nt) {
	std::random_device rd;
    std::mt19937 gen(rd());
	
	std::vector<std::pair<float, Event>> echeancier;
	echeancier.reserve(2048u);
	
	auto const compare = [](std::pair<float,Event> const & left, std::pair<float,Event> const & right) { return left.first > right.first; };
	auto const pop_echeancier = [&echeancier, &compare]() {
		std::pop_heap(echeancier.begin(), echeancier.end(), compare);
		echeancier.pop_back();
	};
	
	auto const push_echeancier = [&echeancier, &compare](float d, Event e) {
		echeancier.push_back(std::make_pair(d, e));
		std::push_heap(echeancier.begin(), echeancier.end(), compare);
	};
	
	auto const push_mailArrival = [&push_echeancier, &gen](float d) {
		//std::exponential_distribution<float> interArrivalMail(d < 60. ? .5 : 5);
		std::exponential_distribution<float> interArrivalMail(d < 60. ? 2 : .2);
		push_echeancier(d + interArrivalMail(gen), Event::mailArrival);
	};
	
	auto const push_mailAnswered = [&push_echeancier, &gen](float d) {
		std::uniform_real_distribution<float> answeringMail(3., 7.);
		push_echeancier(d + answeringMail(gen), Event::mailAnswered);
	};
	
	auto const push_callArrival = [&push_echeancier, &gen](float d) {
		std::exponential_distribution<float> interArrivalCall(d < 60. ? .2 : (d < 180. ? 1 : .1));
		push_echeancier(d + interArrivalCall(gen), Event::callArrival);
	};
	
	auto const push_callAnswered = [&push_echeancier, &gen](float d) {
		std::uniform_real_distribution<float> answeringCall(5., 15.);
		push_echeancier(d + answeringCall(gen), Event::callAnswered);
	};
	
	Resultat result{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0., 0., 0., 0.};
	
	float date = 0.;
	float totalTimeToAnswerMail = 0.;
	float totalTimeToAnswerCall = 0.;
	float totalTimeSpentOnMails = 0.;
	float totalTimeSpentOnCalls = 0.;
	unsigned int Bc = 0u;
	unsigned int Bm = 0u;
	unsigned int Nc = Nt;
	unsigned int Nm = N - Nc;
	
	auto const Begin = [&push_echeancier, &push_callArrival, &push_mailArrival, &push_mailAnswered, &result, &Bm, &gen, &Nm](float d) {
		push_callArrival(0.);
		push_mailArrival(0.);
		
		std::uniform_int_distribution<unsigned int> unansweredMailsDist(20, 80);
		result.unansweredMails = unansweredMailsDist(gen);
		result.totalMails = result.unansweredMails;
		
		Bm = std::min(result.unansweredMails, Nm);
		result.unansweredMails -= Bm;
		
		for (unsigned int i = 0; i < Bm; i++)
			push_mailAnswered(0.);
		
		push_echeancier(240., Event::end);
	};
	
	auto const MailArrival = [&push_mailArrival, &push_mailAnswered, &Bm, &Nm, &result](float d) {
		push_mailArrival(d);
		result.totalMails++;
		
		if (Bm < Nm) {
			Bm++;
			push_mailAnswered(d);
		} else
			result.unansweredMails++;
	};
	
	auto const MailAnswered = [&push_mailAnswered, &push_callAnswered, &Bm, &Bc, &Nm, &Nc, &result, Ntmax](float d) {
		Bm--;
		result.answeredMails++;
		
		if (result.unansweredCalls >= Nc && Nc < Ntmax) {
			Nc++;
			Nm--;
			if (result.unansweredCalls > 0) {
				Bc++;
				result.unansweredCalls--;
				push_callAnswered(d);
			}
		} else {
			if (result.unansweredMails > 0) {
				Bm++;
				result.unansweredMails--;
				push_mailAnswered(d);
			}
		}
	};
	
	auto const CallArrival = [&push_callArrival, &push_callAnswered, &Bc, &Nc, &result](float d) {
		push_callArrival(d);
		result.totalCalls++;
		
		if (Bc < Nc) {
			Bc++;
			push_callAnswered(d);
		} else
			result.unansweredCalls++;
	};
	
	auto const CallAnswered = [&push_callAnswered, &push_mailAnswered, &Bc, &Bm, &Nc, &Nm, &result](float d) {
		Bc--;
		result.answeredCalls++;
		
		if (result.unansweredCalls > 0) {
			Bc++;
			result.unansweredCalls--;
			push_callAnswered(d);
		} else {
			Nc--;
			Nm++;
			if (result.unansweredMails > 0) {
				Bm++;
				result.unansweredMails--;
				push_mailAnswered(d);
			}
		}
	};
	
	auto const End = [&echeancier](float d) {
		echeancier.clear();
	};
	
	push_echeancier(0., Event::begin);
	
	while (!echeancier.empty()) {
		//for (auto const q : echeancier)
		//	printf("%f, %u\t", q.first, (unsigned int) q.second);
		//puts("");
		
		auto const p = echeancier.front();
		pop_echeancier();
		
		//printf("%f, %u\n", p.first, (unsigned int) p.second);
		printf("%f, %lu\n", p.first, echeancier.size());
		
		// maj des aires
		float difference = p.first - date;
		totalTimeToAnswerMail += result.unansweredMails * difference;
		totalTimeToAnswerCall += result.unansweredCalls * difference;
		totalTimeSpentOnMails += Bm * difference;
		totalTimeSpentOnCalls += Bc * difference;
		
		date = p.first;
		
		switch(p.second) {
			case Event::begin:
				Begin(date);
				break;
			case Event::end:
				End(date);
				break;
			case Event::mailArrival:
				MailArrival(date);
				break;
			case Event::callArrival:
				CallArrival(date);
				break;
			case Event::mailAnswered:
				MailAnswered(date);
				break;
			case Event::callAnswered:
				CallAnswered(date);
				break;
			default:
				std::cerr << "Achtung !" << std::endl;
				exit(0);
		}
	}
	
	result.meanTimeToAnswerMail = totalTimeToAnswerMail / result.answeredMails;
	result.meanTimeToAnswerCall = totalTimeToAnswerCall / result.answeredCalls;
	result.staffOccupationRate = (totalTimeSpentOnMails + totalTimeSpentOnCalls) / (240. * N);
	result.phoneBoothOccupationRate = totalTimeSpentOnCalls / (240. * Ntmax);
	
	result.workingCalls = Bc;
	result.workingMails = Bm;
	
	return result;
}

int main(int argc, char ** argv) {
	
	std::cout << "Hello there\n";
	
	auto const result = simulate(45, 20, 5);
	
	printf("TotalMails : %u\nAnsweredMails : %u\nWorkingMails : %u\nUnansweredMails : %u\n",
		  result.totalMails, result.answeredMails, result.workingMails, result.unansweredMails);
	printf("TotalCalls : %u\nAnsweredCalls : %u\nWorkingCalls : %u\nUnansweredCalls : %u\n",
		  result.totalCalls, result.answeredCalls, result.workingCalls, result.unansweredCalls);
	printf("Temps moyen d'attente d'un mail : %f\nTemps moyen d'attente d'un appel : %f\nOccupation du personnel : %f\nOccupation des postes téléphoniques : %f\n",
		  result.meanTimeToAnswerMail, result.meanTimeToAnswerCall, result.staffOccupationRate, result.phoneBoothOccupationRate);
	
	return 0;
}


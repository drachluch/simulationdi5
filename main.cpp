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

struct ResultatAgrege {
	unsigned int simulations;
	float totalMails = 0.;
	float answeredMails = 0.;
	float unansweredMails = 0.;
	float workingMails = 0.;
	float totalCalls = 0.;
	float answeredCalls = 0.;
	float workingCalls = 0.;
	float unansweredCalls = 0.;
	float meanTimeToAnswerMail = 0.;
	float meanTimeToAnswerCall = 0.;
	float staffOccupationRate = 0.;
	float phoneBoothOccupationRate = 0.;

	ResultatAgrege(unsigned int simulations) : simulations(simulations) {};
};

ResultatAgrege& operator+=(ResultatAgrege & a, Resultat const & r) {
	float const s = a.simulations;
	a.totalMails               += r.totalMails / s;
	a.answeredMails            += r.answeredMails / s;
	a.unansweredMails          += r.unansweredMails / s;
	a.workingMails             += r.workingMails / s;
	a.totalCalls               += r.totalCalls / s;
	a.answeredCalls            += r.answeredCalls / s;
	a.workingCalls             += r.workingCalls / s;
	a.unansweredCalls          += r.unansweredCalls / s;
	a.meanTimeToAnswerMail     += r.meanTimeToAnswerMail / s;
	a.meanTimeToAnswerCall     += r.meanTimeToAnswerCall / s;
	a.staffOccupationRate      += r.staffOccupationRate / s;
	a.phoneBoothOccupationRate += r.phoneBoothOccupationRate / s;
	return a;
}

/**
 * N : effectif du personnel
 * Ntmax : nombre de postes téléphoniques
 * Nt : nombre d'humains affectés à un poste téléphonique
 */
Resultat simulate(unsigned int const N, unsigned int const Ntmax, unsigned int Nt, unsigned int seedArrival, unsigned int seedDeparture) {
	std::mt19937 genArrival(seedArrival);
	std::mt19937 genDeparture(seedDeparture);

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

	auto const push_mailArrival = [&push_echeancier, &genArrival](float d) {
		std::exponential_distribution<float> interArrivalMail(d < 60. ? 2 : .2);
		push_echeancier(d + interArrivalMail(genArrival), Event::mailArrival);
	};

	auto const push_mailAnswered = [&push_echeancier, &genDeparture](float d) {
		std::uniform_real_distribution<float> answeringMail(3., 7.);
		push_echeancier(d + answeringMail(genDeparture), Event::mailAnswered);
	};

	auto const push_callArrival = [&push_echeancier, &genArrival](float d) {
		std::exponential_distribution<float> interArrivalCall(d < 60. ? .2 : (d < 180. ? 1 : .1));
		push_echeancier(d + interArrivalCall(genArrival), Event::callArrival);
	};

	auto const push_callAnswered = [&push_echeancier, &genDeparture](float d) {
		std::uniform_real_distribution<float> answeringCall(5., 15.);
		push_echeancier(d + answeringCall(genDeparture), Event::callAnswered);
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

	auto const Begin = [&push_echeancier, &push_callArrival, &push_mailArrival, &push_mailAnswered, &result, &Bm, &genArrival, &Nm](float d) {
		push_callArrival(0.);
		push_mailArrival(0.);

		std::uniform_int_distribution<unsigned int> unansweredMailsDist(20, 80);
		result.unansweredMails = unansweredMailsDist(genArrival);
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
		auto const p = echeancier.front();
		pop_echeancier();

		// maj des aires
		float difference = p.first - date;
		totalTimeToAnswerMail += result.unansweredMails * difference;
		totalTimeToAnswerCall += result.unansweredCalls * difference;
		totalTimeSpentOnMails += Bm * difference;
		totalTimeSpentOnCalls += Bc * difference;

		//std::cerr << p.first << ", " << (unsigned int) p.second << std::endl;

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

	result.meanTimeToAnswerMail = totalTimeToAnswerMail / result.totalMails;
	result.meanTimeToAnswerCall = totalTimeToAnswerCall / result.totalCalls;
	result.staffOccupationRate = (totalTimeSpentOnMails + totalTimeSpentOnCalls) / (240. * N);
	result.phoneBoothOccupationRate = Ntmax == 0 ? 0 : totalTimeSpentOnCalls / (240. * Ntmax);

	result.workingCalls = Bc;
	result.workingMails = Bm;

	return result;
}

unsigned int convert_or_die(char const * str, char const * arg_name) {
	unsigned long N = strtoul(str, nullptr, 10);
	if (errno == ERANGE || N > std::numeric_limits<unsigned int>::max()) {
		std::cerr << arg_name << " is out of range." << std::endl;
		exit(1);
	}
	return static_cast<unsigned int>(N);
}

void assert_or_die(bool b, char const * str) {
	if (!b) {
		std::cerr << str << std::endl;
		exit(1);
	}
}

void printResultAgregeCSV(ResultatAgrege const & r) {
	printf("%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t",
		  r.totalMails, r.answeredMails, r.workingMails, r.unansweredMails,
		  r.totalCalls, r.answeredCalls, r.workingCalls, r.unansweredCalls,
		  r.meanTimeToAnswerMail, r.meanTimeToAnswerCall, r.staffOccupationRate, r.phoneBoothOccupationRate);
}

int readAndSimulate();

int main(int argc, char ** argv) {

	if (argc == 1) {
		return readAndSimulate();
	}

	if (argc < 4) {
		std::cerr << "Paramètres manquants (" << (argc-1) << " au lieu de 3)" << std::endl;
		std::cerr << "Les paramètres sont les suivants : N, Ntmax, Nt" << std::endl;
		std::cerr << "N : effectif total du personnel ;" << std::endl;
		std::cerr << "Ntmax : nombre de postes téléphoniques ;" << std::endl;
		std::cerr << "Nt : effectif attribué aux postes téléphoniques au début de la matinée." << std::endl;
		return 1;
	}

	unsigned int N = convert_or_die(argv[1], "N");
	unsigned int Ntmax = convert_or_die(argv[2], "Ntmax");
	unsigned int Nt = convert_or_die(argv[3], "Nt");

	assert_or_die(Nt <= Ntmax, "Nt doit être inférieur à Ntmax.");
	assert_or_die(Nt <= N, "Nt doit être inférieur à N.");

	unsigned int S = 1u;
	unsigned int seedArrival = 0u;
	unsigned int seedDeparture = 0u;

	if (argc > 4)
		S = convert_or_die(argv[4], "S");
	assert_or_die(S > 0u, "S doit être strictement positif.");

	if (argc > 5)
		seedArrival = convert_or_die(argv[5], "seedArrival");
	if (seedArrival == 0)
		seedArrival = std::random_device()();

	if (argc > 6)
		seedDeparture = convert_or_die(argv[6], "seedDeparture");
	if (seedDeparture == 0)
		seedDeparture = std::random_device()();

	printf("SeedArrival : %u\n", seedArrival);
	printf("SeedDeparture : %u\n", seedDeparture);

	ResultatAgrege meanResult(S);

	for (unsigned int i = 0; i < S; i++) {
		auto const result = simulate(N, Ntmax, Nt, seedArrival + i, seedDeparture + i);
		meanResult += result;
	}

	printf("Nombre de simulations : %u\n", S);
	puts("tM\taM\twM\tuM\ttC\taC\twC\tuC\ttmaM\ttmaC\toqP\toqT");
	printResultAgregeCSV(meanResult);
	puts("");

	return 0;
}

int readAndSimulate() {

	unsigned int N = 0u;
	unsigned int Ntmax = 0u;
	unsigned int Nt = 0u;
	unsigned int S = 0u;
	unsigned int seedArrival = 0u;
	unsigned int seedDeparture = 0u;

	puts("N\tNtmax\tNt\tS\ttM\taM\twM\tuM\ttC\taC\twC\tuC\ttmaM\ttmaC\toqP\toqT\tsA\tsD");

	for (;;) {
		std::cin >> N;
		if (std::cin.fail() || std::cin.eof()) return 0;
		std::cin >> Ntmax;
		if (std::cin.fail() || std::cin.eof()) return 0;
		std::cin >> Nt;
		if (std::cin.fail() || std::cin.eof()) return 0;
		std::cin >> S;
		if (std::cin.fail() || std::cin.eof()) return 0;
		std::cin >> seedArrival;
		if (std::cin.fail() || std::cin.eof()) return 0;
		std::cin >> seedDeparture;
		if (std::cin.fail() || std::cin.eof()) return 0;

		assert_or_die(Nt <= Ntmax, "Nt doit être inférieur à Ntmax.");
		assert_or_die(Nt <= N, "Nt doit être inférieur à N.");

		assert_or_die(S > 0u, "S doit être strictement positif.");

		if (seedArrival == 0)
			seedArrival = std::random_device()();

		if (seedDeparture == 0)
			seedDeparture = std::random_device()();

		ResultatAgrege meanResult(S);

		for (unsigned int i = 0; i < S; i++) {
			auto const result = simulate(N, Ntmax, Nt, seedArrival + i, seedDeparture + i);
			meanResult += result;
		}

		printf("%u\t%u\t%u\t%u\t", N, Ntmax, Nt, S);
		printResultAgregeCSV(meanResult);
		printf("%u\t%u\n", seedArrival, seedDeparture);
	}
}

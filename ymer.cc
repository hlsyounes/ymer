/*
 * Main program.
 *
 * Copyright (C) 2003, 2004 Carnegie Mellon University
 *
 * This file is part of Ymer.
 *
 * Ymer is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Ymer is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ymer; if not, write to the Free Software Foundation,
 * Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * $Id: ymer.cc,v 2.1 2004-01-25 12:45:47 lorens Exp $
 */
#include <config.h>
#include "comm.h"
#include "states.h"
#include "models.h"
#include "formulas.h"
#include <util.h>
#include <cudd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#if HAVE_GETOPT_LONG
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <getopt.h>
#else
#include "getopt.h"
#endif
#include <cerrno>
#include <iostream>
#include <stdexcept>
#include <string>


/* The parse function. */
extern int yyparse();
/* File to parse. */
extern FILE* yyin;
/* Current model. */
extern const Model* global_model;
/* Number of bits required by binary encoding of state space. */
extern int num_model_bits;
/* Parsed properties. */
extern FormulaList properties;
/* Clears all previously parsed declarations. */
extern void clear_declarations();

/* Name of current file. */
std::string current_file;
/* Verbosity level. */
int verbosity;
/* Total number of samples (for statistics). */
size_t total_samples;
/* Number of samples per trial (for statistics). */
std::vector<size_t> samples;
/* Total path lengths (for statistics). */
double total_path_lengths;
/* Sockets for communication. */
int server_socket = -1;
int client_socket = -1;
/* Base port. */
int port = -1;
/* Registered clients. */
ClientTable registered_clients;
/* Current property. */
size_t current_property;

/* Program options. */
static option long_options[] = {
  { "alpha", required_argument, NULL, 'A' },
  { "beta", required_argument, NULL, 'B' },
  { "delta", required_argument, NULL, 'D' },
  { "epsilon", required_argument, NULL, 'E' },
  { "engine", required_argument, NULL, 'e' },
  { "host", required_argument, NULL, 'h' },
  { "ip-address", required_argument, NULL, 'I' },
  { "client-id", required_argument, NULL, 'i' },
  { "matching-moments", required_argument, NULL, 'm' },
  { "port", required_argument, NULL, 'p' },
  { "seed", required_argument, NULL, 'S' },
  { "trials", required_argument, NULL, 'T' },
  { "verbose", optional_argument, NULL, 'v' },
  { "version", no_argument, NULL, 'V' },
  { "help", no_argument, NULL, '?' },
  { 0, 0, 0, 0 }
};
static const char OPTION_STRING[] = "A:B:D:E:e:h:I:i:m:p:S:T:v::V?";


/* Displays help. */
static void display_help() {
  std::cout << "usage: " << PACKAGE << " [options] [file ...]" << std::endl
	    << "options:" << std::endl
	    << "  -A a,  --alpha=a\t"
	    << "use bound a on false negatives with sampling engine"
	    << std::endl
	    << "\t\t\t  (default is 1e-2)" << std::endl
	    << "  -B b,  --beta=b\t"
	    << "use bound b on false positives with sampling engine"
	    << std::endl
	    << "\t\t\t  (default is 1e-2)" << std::endl
	    << "  -D d,  --delta=d\t"
	    << "use indifference region of width 2*d with sampling"
	    << std::endl
	    << "\t\t\t  engine (default is 1e-2)" << std::endl
	    << "  -E e,  --epsilon=e\t"
	    << "use precision e with hybrid engine (default is 1e-6)"
	    << std::endl
	    << "  -e e,  --engine=e\t"
	    << "use engine e; can be `sampling' (default), `hybrid',"
	    << std::endl
	    << "\t\t\t  or `mixed'" << std::endl
	    << "  -h h,  --host=h\t"
	    << "connect to server on host h" << std::endl
	    << "  -I i,  --ip-address=i\t"
	    << "IP address of machine running program" << std::endl
	    << "  -i i,  --client-id=i\t"
	    << "start as client with id i" << std::endl
	    << "  -m m,  --matching-moments=m" << std::endl
	    << "\t\t\tmatch the first m moments of general distributions"
	    << std::endl
	    << "  -p p,  --port=p\t"
	    << "communicate using port p" << std::endl
	    << "  -S s,  --seed=s\t"
	    << "use seed s with random number generator" << std::endl
	    << "\t\t\t  (sampling engine only)" << std::endl
	    << "  -T t,  --trials=t\t"
	    << "number of trials for sampling engine (default is 1)"
	    << std::endl
	    << "  -v[n], --verbose[=n]\t"
	    << "use verbosity level n;" << std::endl
	    << "\t\t\t  n is a number from 0 (verbose mode off) and up;"
	    << std::endl
	    << "\t\t\t  default level is 1 if optional argument is left out"
	    << std::endl
	    << "  -V,    --version\t"
	    << "display version information and exit" << std::endl
	    << "  -?,    --help\t\t"
	    << "display this help and exit" << std::endl
	    << "  file ...\t\t"
	    << "files containing models and properties;" << std::endl
	    << "\t\t\t  if none, descriptions are read from standard input"
	    << std::endl
	    << std::endl
	    << "Report bugs to <" PACKAGE_BUGREPORT ">." << std::endl;
}


/* Displays version information. */
static void display_version() {
  std::cout << PACKAGE_STRING << std::endl
	    << "Copyright (C) 2003, 2004 Carnegie Mellon University"
	    << std::endl
	    << PACKAGE_NAME
	    << " comes with NO WARRANTY, to the extent permitted by law."
	    << std::endl
	    << "For information about the terms of redistribution,"
	    << std::endl
	    << "see the file named COPYING in the " PACKAGE_NAME
	    << " distribution." << std::endl
	    << std::endl
	    << "Written by H\345kan L. S. Younes." << std::endl;
}


/* Parses the given file, and returns true on success. */
static bool read_file(const char* name) {
  yyin = fopen(name, "r");
  if (yyin == NULL) {
    std::cerr << PACKAGE << ':' << name << ": " << strerror(errno)
	      << std::endl;
    return false;
  } else {
    current_file = name;
    bool success = (yyparse() == 0);
    fclose(yyin);
    return success;
  }
}


/* Extracts a path formula from the given state formula. */
bool extract_path_formula(const PathFormula*& pf, double& theta,
			  const StateFormula& f) {
  const Conjunction* cf = dynamic_cast<const Conjunction*>(&f);
  if (cf != NULL) {
    for (FormulaList::const_iterator fi = cf->conjuncts().begin();
	 fi != cf->conjuncts().end(); fi++) {
      if (!extract_path_formula(pf, theta, **fi)) {
	return false;
      }
    }
    return true;
  }
  const Disjunction* df = dynamic_cast<const Disjunction*>(&f);
  if (df != NULL) {
    for (FormulaList::const_iterator fi = df->disjuncts().begin();
	 fi != df->disjuncts().end(); fi++) {
      if (!extract_path_formula(pf, theta, **fi)) {
	return false;
      }
    }
    return true;
  }
  const Negation* nf = dynamic_cast<const Negation*>(&f);
  if (nf != NULL) {
    return extract_path_formula(pf, theta, nf->negand());
  }
  const Implication* wf = dynamic_cast<const Implication*>(&f);
  if (wf != NULL) {
    return (extract_path_formula(pf, theta, wf->antecedent())
	    && extract_path_formula(pf, theta, wf->consequent()));
  }
  const Probabilistic* qf = dynamic_cast<const Probabilistic*>(&f);
  if (qf != NULL) {
    if (pf == NULL) {
      pf = &qf->formula();
      theta = qf->threshold().double_value();
      return true;
    } else {
      pf = NULL;
      return false;
    }
  }
  return true;
}


/* The main program. */
int main(int argc, char* argv[]) {
  /* Set default alpha. */
  double alpha = 1e-2;
  /* Set default beta. */
  double beta = 1e-2;
  /* Set default delta. */
  double delta = 1e-2;
  /* Set default epsilon. */
  double epsilon = 1e-6;
  /* Set default engine. */
  enum { SAMPLING_ENGINE, HYBRID_ENGINE, MIXED_ENGINE } engine =
							  SAMPLING_ENGINE;
  /* Number of moments to match. */
  size_t moments = 3;
  /* Set default seed. */
  size_t seed = time(NULL);
  /* Set default number of trials. */
  size_t trials = 1;
  /* Set default verbosity. */
  verbosity = 0;
  /* Sever hostname */
  std::string hostname = "localhost";
  /* Local ip-address */
  std::string ip_address = "127.0.0.1";
  /* Client id. */
  int client_id = -1;

  try {
    /*
     * Get command line options.
     */
    while (1) {
      int option_index = 0;
      int c = getopt_long(argc, argv, OPTION_STRING,
			  long_options, &option_index);
      if (c == -1) {
	break;
      }
      switch (c) {
      case 'A':
	alpha = atof(optarg);
	if (alpha < 1e-10) {
	  throw std::invalid_argument("alpha < 1e-10");
	} else if (alpha >= 0.5) {
	  throw std::invalid_argument("alpha >= 0.5");
	}
	break;
      case 'B':
	beta = atof(optarg);
	if (beta < 1e-10) {
	  throw std::invalid_argument("beta < 1e-10");
	} else if (beta >= 0.5) {
	  throw std::invalid_argument("beta >= 0.5");
	}
	break;
      case 'D':
	delta = atof(optarg);
	if (delta < 1e-10) {
	  throw std::invalid_argument("delta < 1e-10");
	} else if (delta > 0.5) {
	  throw std::invalid_argument("delta > 0.5");
	}
	break;
      case 'E':
	epsilon = atof(optarg);
	if (epsilon < 1e-10) {
	  throw std::invalid_argument("epsilon < 1e-10");
	} else if (epsilon > 1.0) {
	  throw std::invalid_argument("epsilon > 1.0");
	}
	break;
      case 'e':
	if (strcasecmp(optarg, "sampling") == 0) {
	  engine = SAMPLING_ENGINE;
	} else if (strcasecmp(optarg, "hybrid") == 0) {
	  engine = HYBRID_ENGINE;
	} else if (strcasecmp(optarg, "mixed") == 0) {
	  engine = MIXED_ENGINE;
	} else {
	  throw std::invalid_argument("unsupported engine `"
				      + std::string(optarg) + "'");
	}
	break;
      case 'h':
	hostname = optarg;
	break;
      case 'I':
	ip_address = optarg;
	break;
      case 'i':
	client_id = atoi(optarg);
	if (client_id < 0 || client_id > 0xffff) {
	  throw std::invalid_argument("invalid client id");
	}
	break;
      case 'm':
	moments = atoi(optarg);
	if (moments < 1) {
	  throw std::invalid_argument("must match at least one moment");
	} else if (moments > 3) {
	  throw std::invalid_argument("cannot match more than three moments");
	}
	break;
      case 'p':
	port = atoi(optarg);
	if (port < 0 || port > 0xffff) {
	  throw std::invalid_argument("invalid port number");
	}
	break;
      case 'S':
	seed = atoi(optarg);
	break;
      case 'T':
	trials = atoi(optarg);
	break;
      case 'v':
	verbosity = (optarg != NULL) ? atoi(optarg) : 1;
	break;
      case 'V':
	display_version();
	return 0;
      case '?':
	if (optopt == '?') {
	  display_help();
	  return 0;
	}
      case ':':
      default:
	std::cerr << "Try `" PACKAGE " --help' for more information."
		  << std::endl;
	return -1;
      }
    }

    /*
     * Read files.
     */
    if (optind < argc) {
      /*
       * Use remaining command line arguments as file names.
       */
      while (optind < argc) {
	if (!read_file(argv[optind++])) {
	  return -1;
	}
      }
    } else {
      /*
       * No remaining command line argument, so read from standard input.
       */
      yyin = stdin;
      if (yyparse() != 0) {
	return -1;
      }
    }
    clear_declarations();
    if (global_model == NULL) {
      std::cout << "no model" << std::endl;
      return 0;
    }
    if (verbosity > 1) {
      std::cout << *global_model << std::endl;
    }

    std::cout.setf(std::ios::unitbuf);
    if (port >= 0) {
      client_socket = socket(PF_INET, SOCK_DGRAM, 0);
      if (client_socket == -1) {
	perror(PACKAGE);
	return -1;
      }
      server_socket = socket(PF_INET, SOCK_DGRAM, 0);
      if (server_socket == -1) {
	perror(PACKAGE);
	return -1;
      }
      hostent* host = gethostbyname(hostname.c_str());
      if (host == NULL) {
	herror(PACKAGE);
	return -1;
      }
      sockaddr_in srvaddr;
      srvaddr.sin_family = AF_INET;
      srvaddr.sin_port = htons(port);
      srvaddr.sin_addr = *(in_addr*) host->h_addr;
      if (client_id >= 0) {
	/* Client mode. */
	if (-1 == connect(client_socket,
			  (sockaddr*) &srvaddr, sizeof srvaddr)) {
	  perror(PACKAGE);
	  return -1;
	}
	sockaddr_in locaddr;
	locaddr.sin_family = AF_INET;
	locaddr.sin_port = htons(port + client_id + 1);
	inet_aton(ip_address.c_str(), &locaddr.sin_addr);
	if (-1 == bind(server_socket, (sockaddr*) &locaddr, sizeof locaddr)) {
	  perror(PACKAGE);
	  return -1;
	}
	unsigned long caddr = ntohl(locaddr.sin_addr.s_addr);
	if (verbosity > 0) {
	  std::cout << "Client " << client_id << " at "
		    << ((caddr >> 24UL)&0xffUL) << '.'
		    << ((caddr >> 16UL)&0xffUL) << '.'
		    << ((caddr >> 8UL)&0xffUL) << '.'
		    << (caddr&0xffUL) << std::endl;
	  std::cout << "Initializing random number generator...";
	  Distribution::mts = get_mt_parameter_id(client_id);
	  std::cout << "done" << std::endl;
	}
 	ClientMsg msg = { ClientMsg::REGISTER, client_id, caddr };
	if (-1 == send(client_socket, &msg, sizeof msg, 0)) {
	  perror(PACKAGE);
	  return -1;
	}
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(server_socket, &rfds);
	const State init_state(*global_model);
	const PathFormula* pf = NULL;
	double alphap = alpha, betap = beta;
	timeval timeout = { 0, 0 };
	timeval* to = NULL;
	while (true) {
	  timeout.tv_sec = 0;
	  timeout.tv_usec = 0;
	  int result = select(server_socket + 1, &rfds, NULL, NULL, to);
	  if (result == -1) {
	    perror(PACKAGE);
	    exit(-1);
	  } else if (result == 0) {
	    if (pf != NULL) {
	      ClientMsg msg = { ClientMsg::SAMPLE, client_id };
	      if (pf->sample(*global_model, init_state,
			     delta, alphap, betap)) {
		msg.value = 1;
	      } else {
		msg.value = 0;
	      }
	      if (-1 == send(client_socket, &msg, sizeof msg, 0)) {
		if (verbosity > 0) {
		  std::cout << "Shutting down (server unavailable)"
			    << std::endl;
		}
		return 0;
	      }
	    }
	  } else {
	    ServerMsg smsg;
	    if (-1 == recv(server_socket, &smsg, sizeof smsg, 0)) {
	      perror(PACKAGE);
	      return -1;
	    }
	    if (smsg.id == ServerMsg::START) {
	      double theta;
	      if (!extract_path_formula(pf, theta, *properties[smsg.value])) {
		std::cerr << PACKAGE << ": multiple path formulae"
			  << std::endl;
		return -1;
	      }
	      if (pf != NULL) {
		alphap = alpha;
		betap = beta;
		if (pf->probabilistic()) {
		  double p0, p1;
		  while (true) {
		    p0 = std::min(1.0, (theta + delta)*(1.0 - betap));
		    p1 = std::max(0.0, 1.0 - ((1.0 - (theta - delta))
					      *(1.0 - alphap)));
		    if (p1 < p0) {
		      break;
		    } else {
		      alphap *= 0.5;
		      betap *= 0.5;
		    }
		  }
		}
	      }
	      to = &timeout;
	      if (verbosity > 0) {
		std::cout << "Sampling started for property "
			  << smsg.value << std::endl;
	      }
	    } else if (smsg.id == ServerMsg::STOP) {
	      to = NULL;
	      if (verbosity > 0) {
		std::cout << "Sampling stopped." << std::endl;
	      }
	    } else if (smsg.id == ServerMsg::QUIT) {
	      if (verbosity > 0) {
		std::cout << "Client terminated by sever." << std::endl;
	      }
	      return 0;
	    } else {
	      std::cerr << "Message with bad id (" << smsg.id << ") ignored."
			<< std::endl;
	    }
	  }
	}
      } else {
	/* Server mode. */
	if (verbosity > 0) {
	  std::cout << "Server at port " << port << std::endl;
	}
	if (-1 == bind(client_socket, (sockaddr*) &srvaddr, sizeof srvaddr)) {
	  perror(PACKAGE);
	  return -1;
	}
      }
    }

    if (engine == SAMPLING_ENGINE) {
      init_genrand_id(seed, Distribution::mts);
      std::cout << "Sampling engine: alpha=" << alpha << ", beta=" << beta
		<< ", delta=" << delta << ", seed=" << seed << std::endl;
      itimerval timer = { { 0, 0 }, { 40000000L, 0 } };
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, NULL);
#else
      setitimer(ITIMER_PROF, &timer, NULL);
#endif
      const State init_state(*global_model);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      long sec = 40000000L - timer.it_value.tv_sec;
      long usec = 1000000L - timer.it_value.tv_usec;
      if (usec < 1000000) {
	sec--;
      }
      double t = std::max(0.0, sec + usec*1e-6);
      std::cout << "Model built in " << t << " seconds." << std::endl;
      if (verbosity > 0) {
	std::cout << "Variables: " << init_state.values().size() << std::endl;
	std::cout << "Events:    " << global_model->commands().size()
		  << std::endl;
      }
      for (FormulaList::const_iterator fi = properties.begin();
	   fi != properties.end(); fi++) {
	std::cout << std::endl << "Model checking " << **fi << " ..."
		  << std::endl;
	current_property = fi - properties.begin();
	size_t accepts = 0;
	std::vector<double> times;
	double total_time = 0.0;
	total_samples = 0;
	samples.clear();
	total_path_lengths = 0.0;
	for (size_t i = 0; i < trials; i++) {
	  timeval start_time;
	  gettimeofday(&start_time, NULL);
	  itimerval timer = { { 0, 0 }, { 40000000L, 0 } };
#ifdef PROFILING
	  setitimer(ITIMER_VIRTUAL, &timer, NULL);
#else
	  setitimer(ITIMER_PROF, &timer, NULL);
#endif
	  bool sol = (*fi)->verify(*global_model, init_state,
				   delta, alpha, beta);
#ifdef PROFILING
	  getitimer(ITIMER_VIRTUAL, &timer);
#else
	  getitimer(ITIMER_PROF, &timer);
#endif
	  timeval end_time;
	  gettimeofday(&end_time, NULL);
	  double t;
	  if (client_socket != -1) {
	    long sec = end_time.tv_sec - start_time.tv_sec;
	    long usec = end_time.tv_usec - start_time.tv_usec;
	    if (usec < 0) {
	      sec--;
	      usec = 1000000 + usec;
	    }
	    t = std::max(0.0, sec + usec*1e-6);
	  } else {
	    long sec = 40000000L - timer.it_value.tv_sec;
	    long usec = 1000000 - timer.it_value.tv_usec;
	    if (usec < 1000000) {
	      sec--;
	    }
	    t = std::max(0.0, sec + usec*1e-6);
	  }
	  if (trials == 1) {
	    std::cout << "Model checking completed in " << t << " seconds."
		      << std::endl;
	    if (sol) {
	      std::cout << "Property is true in the initial state."
			<< std::endl;
	    } else {
	      std::cout << "Property is false in the initial state."
			<< std::endl;
	    }
	  } else {
	    if (sol) {
	      accepts++;
	    }
	    total_time += t;
	    times.push_back(t);
	  }
	}
	if (trials > 1) {
	  double time_avg = total_time/trials;
	  double sample_avg = double(total_samples)/trials;
	  double path_avg = total_path_lengths/total_samples;
	  double time_var = 0.0;
	  double sample_var = 0.0;
	  for (size_t i = 0; i < trials; i++) {
	    double diff = times[i] - time_avg;
	    time_var += diff*diff;
	    diff = samples[i] - sample_avg;
	    sample_var += diff*diff;
	  }
	  time_var /= trials - 1;
	  sample_var /= trials - 1;
	  std::cout << "Average model checking time: " << time_avg
		    << " seconds" << std::endl
		    << "Time standard deviation: " << sqrt(time_var)
		    << std::endl
		    << "Average number of samples: " << sample_avg << std::endl
		    << "Samples standard deviation: " << sqrt(sample_var)
		    << std::endl
		    << "Average path lengths: " << path_avg << std::endl
		    << accepts << " accepted, " << (trials - accepts)
		    << " rejected" << std::endl;
	}
      }
    } else if (engine == HYBRID_ENGINE) {
      std::cout << "Hybrid engine: epsilon=" << epsilon << std::endl;
      DdManager* dd_man = Cudd_Init(2*num_model_bits, 0, CUDD_UNIQUE_SLOTS,
				    CUDD_CACHE_SLOTS, 0);
      Cudd_SetEpsilon(dd_man, 1e-15);
      itimerval timer = { { 0L, 0L }, { 40000000L, 0L } };
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, NULL);
#else
      setitimer(ITIMER_PROF, &timer, NULL);
#endif
      global_model->cache_dds(dd_man, moments);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      long sec = 40000000L - timer.it_value.tv_sec;
      long usec = 1000000L - timer.it_value.tv_usec;
      if (usec < 1000000) {
	sec--;
      }
      double t = std::max(0.0, sec + usec*1e-6);
      std::cout << "Model built in " << t << " seconds." << std::endl;
      if (verbosity > 0) {
	std::cout << "States:      " << global_model->num_states(dd_man)
		  << std::endl
		  << "Transitions: " << global_model->num_transitions(dd_man)
		  << std::endl;
	DdNode* ddR = global_model->rate_mtbdd(dd_man);
	std::cout << "Rate matrix";
	Cudd_PrintDebug(dd_man, ddR, Cudd_ReadSize(dd_man), 1);
	Cudd_RecursiveDeref(dd_man, ddR);
	std::cout << "ODD:         " << get_num_odd_nodes() << " nodes"
		  << std::endl;
      }
      global_model->init_index(dd_man);
      DdNode* init = global_model->init_bdd(dd_man);
      for (FormulaList::const_iterator fi = properties.begin();
	   fi != properties.end(); fi++) {
	std::cout << std::endl << "Model checking " << **fi << " ..."
		  << std::endl;
	itimerval timer = { { 0, 0 }, { 40000000L, 0 } };
#ifdef PROFILING
	setitimer(ITIMER_VIRTUAL, &timer, NULL);
#else
	setitimer(ITIMER_PROF, &timer, NULL);
#endif
	DdNode* ddf = (*fi)->verify(dd_man, *global_model, epsilon);
	DdNode* sol = Cudd_bddAnd(dd_man, ddf, init);
	Cudd_Ref(sol);
	Cudd_RecursiveDeref(dd_man, ddf);
#ifdef PROFILING
	getitimer(ITIMER_VIRTUAL, &timer);
#else
	getitimer(ITIMER_PROF, &timer);
#endif
	long sec = 40000000L - timer.it_value.tv_sec;
	long usec = 1000000 - timer.it_value.tv_usec;
	if (usec < 1000000) {
	  sec--;
	}
	double t = std::max(0.0, sec + usec*1e-6);
	std::cout << "Model checking completed in " << t << " seconds."
		  << std::endl;
	if (sol != Cudd_ReadLogicZero(dd_man)) {
	  std::cout << "Property is true in the initial state." << std::endl;
	} else {
	  std::cout << "Property is false in the initial state." << std::endl;
	}
	Cudd_RecursiveDeref(dd_man, sol);
      }
      Cudd_RecursiveDeref(dd_man, init);
      global_model->uncache_dds(dd_man);
      int unrel = Cudd_CheckZeroRef(dd_man);
      if (unrel != 0) {
	std::cerr << unrel << " unreleased DDs" << std::endl;
      }
      Cudd_Quit(dd_man);
    } else if (engine == MIXED_ENGINE) {
      init_genrand(seed);
      std::cout << "Mixed engine: alpha=" << alpha << ", beta=" << beta
		<< ", delta=" << delta << ", epsilon=" << epsilon
		<< ", seed=" << seed << std::endl;
      DdManager* dd_man = Cudd_Init(2*num_model_bits, 0, CUDD_UNIQUE_SLOTS,
				    CUDD_CACHE_SLOTS, 0);
      Cudd_SetEpsilon(dd_man, 1e-15);
      itimerval timer = { { 0, 0 }, { 40000000L, 0 } };
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, NULL);
#else
      setitimer(ITIMER_PROF, &timer, NULL);
#endif
      global_model->cache_dds(dd_man, moments);
      const State init_state(*global_model);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      long sec = 40000000L - timer.it_value.tv_sec;
      long usec = 1000000 - timer.it_value.tv_usec;
      if (usec < 1000000) {
	sec--;
      }
      double t = std::max(0.0, sec + usec*1e-6);
      std::cout << "Model built in " << t << " seconds." << std::endl;
      if (verbosity > 0) {
	std::cout << "Variables: " << init_state.values().size() << std::endl;
	std::cout << "Events:    " << global_model->commands().size()
		  << std::endl;
	std::cout << "States:      " << global_model->num_states(dd_man)
		  << std::endl
		  << "Transitions: " << global_model->num_transitions(dd_man)
		  << std::endl;
	DdNode* ddR = global_model->rate_mtbdd(dd_man);
	std::cout << "Rate matrix";
	Cudd_PrintDebug(dd_man, ddR, Cudd_ReadSize(dd_man), 1);
	Cudd_RecursiveDeref(dd_man, ddR);
	std::cout << "ODD:         " << get_num_odd_nodes() << " nodes"
		  << std::endl;
      }
      for (FormulaList::const_iterator fi = properties.begin();
	   fi != properties.end(); fi++) {
	std::cout << std::endl << "Model checking " << **fi << " ..."
		  << std::endl;
	size_t accepts = 0;
	std::vector<double> times;
	double total_time = 0.0;
	total_samples = 0;
	samples.clear();
	total_path_lengths = 0.0;
	for (size_t i = 0; i < trials; i++) {
	  itimerval timer = { { 0, 0 }, { 40000000L, 0 } };
#ifdef PROFILING
	  setitimer(ITIMER_VIRTUAL, &timer, NULL);
#else
	  setitimer(ITIMER_PROF, &timer, NULL);
#endif
	  bool sol = (*fi)->verify(dd_man, *global_model, init_state,
				   delta, alpha, beta, epsilon);
#ifdef PROFILING
	  getitimer(ITIMER_VIRTUAL, &timer);
#else
	  getitimer(ITIMER_PROF, &timer);
#endif
	  long sec = 40000000L - timer.it_value.tv_sec;
	  long usec = 1000000 - timer.it_value.tv_usec;
	  if (usec < 1000000) {
	    sec--;
	  }
	  double t = std::max(0.0, sec + usec*1e-6);
	  if (trials == 1) {
	    std::cout << "Model checking completed in " << t << " seconds."
		      << std::endl;
	    if (sol) {
	      std::cout << "Property is true in the initial state."
			<< std::endl;
	    } else {
	      std::cout << "Property is false in the initial state."
			<< std::endl;
	    }
	  } else {
	    if (sol) {
	      accepts++;
	    }
	    total_time += t;
	    times.push_back(t);
	  }
	}
	if (trials > 1) {
	  double time_avg = total_time/trials;
	  double sample_avg = double(total_samples)/trials;
	  double path_avg = total_path_lengths/total_samples;
	  double time_var = 0.0;
	  double sample_var = 0.0;
	  for (size_t i = 0; i < trials; i++) {
	    double diff = times[i] - time_avg;
	    time_var += diff*diff;
	    diff = samples[i] - sample_avg;
	    sample_var += diff*diff;
	  }
	  time_var /= trials - 1;
	  sample_var /= trials - 1;
	  std::cout << "Average model checking time: " << time_avg
		    << " seconds" << std::endl
		    << "Time standard deviation: " << sqrt(time_var)
		    << std::endl
		    << "Average number of samples: " << sample_avg << std::endl
		    << "Samples standard deviation: " << sqrt(sample_var)
		    << std::endl
		    << "Average path lengths: " << path_avg << std::endl
		    << accepts << " accepted, " << (trials - accepts)
		    << " rejected" << std::endl;
	}
      }
      global_model->uncache_dds(dd_man);
      int unrel = Cudd_CheckZeroRef(dd_man);
      if (unrel != 0) {
	std::cerr << unrel << " unreleased DDs" << std::endl;
      }
      Cudd_DebugCheck(dd_man);
      Cudd_Quit(dd_man);
    }
    delete global_model;
    for (FormulaList::const_iterator fi = properties.begin();
	 fi != properties.end(); fi++) {
      StateFormula::destructive_deref(*fi);
    }
    properties.clear();
  } catch (const std::exception& e) {
    std::cerr << std::endl << PACKAGE ": " << e.what() << std::endl;
    return -1;
  } catch (...) {
    std::cerr << std::endl << PACKAGE ": fatal error" << std::endl;
    return -1;
  }
  if (Distribution::mts != NULL) {
    free_mt_struct(Distribution::mts);
  }
  for (ClientTable::const_iterator ci = registered_clients.begin();
       ci != registered_clients.end(); ci++) {
    std::cout << "terminating client " << (*ci).first << std::endl;
    ServerMsg smsg = { ServerMsg::QUIT };
    const sockaddr* addr = (sockaddr*) &(*ci).second;
    if (-1 == sendto(server_socket, &smsg, sizeof smsg, 0,
		     addr, sizeof *addr)) {
      perror(PACKAGE);
      return -1;
    }
  }
#if 0
  if (client_socket != -1) {
    close(client_socket);
  }
  if (server_socket != -1) {
    close(server_socket);
  }
#endif

  return 0;
}

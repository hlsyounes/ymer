/*
 * Main program.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
 * Copyright (C) 2011-2012 Google Inc
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
 */
#include <config.h>
#include "comm.h"
#include "states.h"
#include "models.h"
#include "formulas.h"
#include <cudd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#if HAVE_GETOPT_LONG
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <getopt.h>
#else
#include "getopt.h"
#endif
#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
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
/* Constant overrides. */
std::map<std::string, Rational> const_overrides;
/* Verbosity level. */
int verbosity;
/* Whether memoization is enabled. */
bool memoization = false;
/* Fixed nested error. */
double nested_error = -1.0;
/* Total number of samples (for statistics). */
size_t total_samples;
/* Number of samples per trial (for statistics). */
std::vector<size_t> samples;
/* Total path lengths (for statistics). */
double total_path_lengths;
/* Sockets for communication. */
int server_socket = -1;
/* Current property. */
size_t current_property;

/* Set default delta. */
static double delta = 1e-2;
/* Whether delta is relative or not. */
static bool relative_delta = false;

/* Program options. */
static option long_options[] = {
  { "alpha", required_argument, 0, 'A' },
  { "beta", required_argument, 0, 'B' },
  { "delta", required_argument, 0, 'D' },
  { "relative-delta", required_argument, 0, 'd' },
  { "epsilon", required_argument, 0, 'E' },
  { "const", required_argument, 0, 'c' },
  { "engine", required_argument, 0, 'e' },
  { "host", required_argument, 0, 'H' },
  { "memoization", no_argument, 0, 'M' },
  { "matching-moments", required_argument, 0, 'm' },
  { "nested-error", required_argument, 0, 'n' },
  { "estimate-probabilities", no_argument, 0, 'p' },
  { "port", required_argument, 0, 'P' },
  { "sampling-algorithm", required_argument, 0, 's' },
  { "seed", required_argument, 0, 'S' },
  { "trials", required_argument, 0, 'T' },
  { "verbose", optional_argument, 0, 'v' },
  { "version", no_argument, 0, 'V' },
  { "help", no_argument, 0, 'h' },
  { 0, 0, 0, 0 }
};
static const char OPTION_STRING[] = "A:B:c:D:d:E:e:H:hMm:n:pP:s:S:T:v::V";


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
	    << "  -d d,  --relative-delta=d\t"
	    << "use indifference region of relative with sampling"
	    << std::endl
	    << "\t\t\t  engine (default is 1e-2)" << std::endl
	    << "  -E e,  --epsilon=e\t"
	    << "use precision e with hybrid engine (default is 1e-6)"
	    << std::endl
	    << "  -e e,  --engine=e\t"
	    << "use engine e; can be `sampling' (default), `hybrid',"
	    << std::endl
	    << "\t\t\t  or `mixed'" << std::endl
	    << "  -H h,  --host=h\t"
	    << "connect to server on host h" << std::endl
	    << "  -M,    --memoization\t"
	    << "use memoization for sampling engine" << std::endl
	    << "  -m m,  --matching-moments=m" << std::endl
	    << "\t\t\tmatch the first m moments of general distributions"
	    << std::endl
	    << "  -p,    --estimate-probabilities" << std::endl
	    << "\t\t\testimates probabilities of path formulae holding"
	    << std::endl
	    << "  -P p,  --port=p\t"
	    << "communicate using port p" << std::endl
	    << "  -s s,  --sampling-algorithm=s" << std::endl
	    << "\t\t\tuse sampling algorithm s"
	    << std::endl
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
	    << "  -h,    --help\t\t"
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
	    << "Copyright (C) 2003--2005 Carnegie Mellon University"
	    << std::endl
            << "Copyright (C) 2011--2012 Google Inc" << std::endl
	    << PACKAGE_NAME
	    << " comes with NO WARRANTY, to the extent permitted by law."
	    << std::endl
	    << "For information about the terms of redistribution,"
	    << std::endl
	    << "see the file named COPYING in the " PACKAGE_NAME
	    << " distribution." << std::endl
	    << std::endl
	    << "Written by Haakan Younes." << std::endl;
}


/* Parses spec for const overrides.  Returns true on success. */
static bool parse_const_overrides(
    const std::string& spec, std::map<std::string, Rational>* const_overrides) {
  if (spec.empty()) {
    return true;
  }
  std::string::const_iterator comma = spec.begin() - 1;
  while (comma != spec.end()) {
    std::string::const_iterator next_comma = find(comma + 1, spec.end(), ',');
    std::string::const_iterator assignment = find(comma + 1, next_comma, '=');
    if (assignment == next_comma) {
      return false;
    }
    const std::string name(comma + 1, assignment);
    const std::string value(assignment + 1, next_comma);
    if (!const_overrides->insert(std::make_pair(name, value.c_str())).second) {
      return false;
    }
    comma = next_comma;
  }
  return true;
}

/* Parses the given file, and returns true on success. */
static bool read_file(const char* name) {
  yyin = fopen(name, "r");
  if (yyin == 0) {
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
static bool extract_path_formula(const PathFormula*& pf, double& theta,
			  const StateFormula& f) {
  const Conjunction* cf = dynamic_cast<const Conjunction*>(&f);
  if (cf != 0) {
    for (FormulaList::const_iterator fi = cf->conjuncts().begin();
	 fi != cf->conjuncts().end(); fi++) {
      if (!extract_path_formula(pf, theta, **fi)) {
	return false;
      }
    }
    return true;
  }
  const Disjunction* df = dynamic_cast<const Disjunction*>(&f);
  if (df != 0) {
    for (FormulaList::const_iterator fi = df->disjuncts().begin();
	 fi != df->disjuncts().end(); fi++) {
      if (!extract_path_formula(pf, theta, **fi)) {
	return false;
      }
    }
    return true;
  }
  const Negation* nf = dynamic_cast<const Negation*>(&f);
  if (nf != 0) {
    return extract_path_formula(pf, theta, nf->negand());
  }
  const Implication* wf = dynamic_cast<const Implication*>(&f);
  if (wf != 0) {
    return (extract_path_formula(pf, theta, wf->antecedent())
	    && extract_path_formula(pf, theta, wf->consequent()));
  }
  const Probabilistic* qf = dynamic_cast<const Probabilistic*>(&f);
  if (qf != 0) {
    if (pf == 0) {
      pf = &qf->formula();
      theta = qf->threshold().double_value();
      return true;
    } else {
      pf = 0;
      return false;
    }
  }
  return true;
}


double indifference_region(double theta) {
  if (relative_delta) {
    return 2*delta*((theta <= 0.5) ? theta : 1.0 - theta);
  } else {
    return delta;
  }
}


/* The main program. */
int main(int argc, char* argv[]) {
  /* Set default alpha. */
  double alpha = 1e-2;
  /* Set default beta. */
  double beta = 1e-2;
  /* Set default epsilon. */
  double epsilon = 1e-6;
  /* Verification without estimation by default. */
  bool estimate = false;
  /* Set default engine. */
  enum { SAMPLING_ENGINE, HYBRID_ENGINE, MIXED_ENGINE } engine =
							  SAMPLING_ENGINE;
  /* Sampling algorithm. */
  SamplingAlgorithm algorithm = SPRT;
  /* Number of moments to match. */
  size_t moments = 3;
  /* Set default seed. */
  size_t seed = time(0);
  /* Set default number of trials. */
  size_t trials = 1;
  /* Set default verbosity. */
  verbosity = 0;
  /* Sever hostname */
  std::string hostname;
  /* Server port. */
  int port = -1;

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
      case 'c':
        if (!parse_const_overrides(optarg, &const_overrides)) {
          throw std::invalid_argument("bad --const specification");
        }
        break;
      case 'D':
	delta = atof(optarg);
	relative_delta = false;
	if (delta < 1e-10) {
	  throw std::invalid_argument("delta < 1e-10");
	} else if (delta > 0.5) {
	  throw std::invalid_argument("delta > 0.5");
	}
	break;
      case 'd':
	delta = atof(optarg);
	relative_delta = true;
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
      case 'H':
	hostname = optarg;
	break;
      case 'M':
	memoization = true;
	break;
      case 'm':
	moments = atoi(optarg);
	if (moments < 1) {
	  throw std::invalid_argument("must match at least one moment");
	} else if (moments > 3) {
	  throw std::invalid_argument("cannot match more than three moments");
	}
	break;
      case 'n':
	nested_error = atof(optarg);
	break;
      case 'p':
	estimate = true;
	algorithm = ESTIMATE;
	break;
      case 'P':
	port = atoi(optarg);
	if (port < 0 || port > 0xffff) {
	  throw std::invalid_argument("invalid port number");
	}
	break;
      case 's':
	if (strcasecmp(optarg, "sequential") == 0) {
	  algorithm = SEQUENTIAL;
	} else if (strcasecmp(optarg, "sprt") == 0) {
	  algorithm = SPRT;
	} else {
	  throw std::invalid_argument("unsupported sampling algorithm `"
				      + std::string(optarg) + "'");
	}
	break;
      case 'S':
	seed = atoi(optarg);
	break;
      case 'T':
	trials = atoi(optarg);
	break;
      case 'v':
	verbosity = (optarg != 0) ? atoi(optarg) : 1;
	break;
      case 'V':
	display_version();
	return 0;
      case 'h':
        display_help();
        return 0;
      case ':':
      default:
	std::cerr << "Try `" PACKAGE " --help' for more information."
		  << std::endl;
	return 1;
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
	  return 1;
	}
      }
    } else {
      /*
       * No remaining command line argument, so read from standard input.
       */
      yyin = stdin;
      if (yyparse() != 0) {
	return 1;
      }
    }
    clear_declarations();
    if (global_model == 0) {
      std::cout << "no model" << std::endl;
      return 0;
    }
    if (verbosity > 1) {
      std::cout << *global_model << std::endl;
    }

    std::cout.setf(std::ios::unitbuf);
    if (port >= 0) {
      sockaddr_in srvaddr;
      srvaddr.sin_family = AF_INET;
      srvaddr.sin_port = htons(port);
      if (!hostname.empty()) {
	/* Client mode. */
	hostent* host = gethostbyname(hostname.c_str());
	if (host == 0) {
	  herror(PACKAGE);
	  return 1;
	}
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
	  perror(PACKAGE);
	  return 1;
	}
	srvaddr.sin_addr = *(in_addr*) host->h_addr;
	if (-1 == connect(sockfd, (sockaddr*) &srvaddr, sizeof srvaddr)) {
	  perror(PACKAGE);
	  return 1;
	}
	ServerMsg smsg;
	int nbytes = recv(sockfd, &smsg, sizeof smsg, 0);
	if (nbytes == -1) {
	  perror(PACKAGE);
	  return 1;
	} else if (nbytes == 0) {
	  if (verbosity > 0) {
	    std::cout << "Shutting down (server unavailable)" << std::endl;
	  }
	  return 0;
	}
	if (smsg.id != ServerMsg::REGISTER) {
	  throw std::logic_error("expecting register message");
	}
	int client_id = smsg.value;
	if (verbosity > 0) {
	  std::cout << "Client " << client_id << std::endl;
	  std::cout << "Initializing random number generator...";
	}
	Distribution::mts = get_mt_parameter_id(client_id);
	init_genrand_id(seed, Distribution::mts);
	if (verbosity > 0) {
	  std::cout << "done" << std::endl;
	}
	fd_set master_fds;
	FD_ZERO(&master_fds);
	FD_SET(sockfd, &master_fds);
	const State init_state(*global_model);
	const PathFormula* pf = 0;
	double alphap = alpha, betap = beta;
	timeval timeout;
	timeval* to = 0;
	while (true) {
	  if (to != 0) {
	    to->tv_sec = 0;
	    to->tv_usec = 0;
	  }
	  fd_set read_fds = master_fds;
	  int result = select(sockfd + 1, &read_fds, 0, 0, to);
	  if (result == -1) {
	    perror(PACKAGE);
	    exit(-1);
	  } else if (result == 0) {
	    if (pf != 0) {
	      ClientMsg msg = { ClientMsg::SAMPLE };
	      msg.value = pf->sample(*global_model, init_state,
				     indifference_region, alphap, betap,
				     algorithm);
	      if (verbosity > 1) {
		std::cout << "Sending sample " << msg.value << std::endl;
	      }
	      nbytes = send(sockfd, &msg, sizeof msg, 0);
	      if (nbytes == -1) {
		perror(PACKAGE);
		return 1;
	      } else if (nbytes == 0) {
		if (verbosity > 0) {
		  std::cout << "Shutting down (server unavailable)"
			    << std::endl;
		}
		return 0;
	      }
	    }
	  } else {
	    nbytes = recv(sockfd, &smsg, sizeof smsg, 0);
	    if (nbytes == -1) {
	      perror(PACKAGE);
	      return 1;
	    } else if (nbytes == 0) {
	      if (verbosity > 0) {
		std::cout << "Shutting down (server unavailable)" << std::endl;
	      }
	      return 0;
	    }
	    if (smsg.id == ServerMsg::START) {
	      double theta;
	      if (!extract_path_formula(pf, theta, *properties[smsg.value])) {
		std::cerr << PACKAGE << ": multiple path formulae"
			  << std::endl;
		return 1;
	      }
	      if (pf != 0) {
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
	      to = 0;
	      if (verbosity > 0) {
		std::cout << "Sampling stopped." << std::endl;
	      }
	    } else {
	      std::cerr << "Message with bad id (" << smsg.id << ") ignored."
			<< std::endl;
	    }
	  }
	}
      } else {
	/* Server mode. */
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
	  perror(PACKAGE);
	  return 1;
	}
	int yes = 1;
	if (-1 == setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes,
			     sizeof yes)) {
	  perror(PACKAGE);
	  return 1;
	}
	srvaddr.sin_addr.s_addr = INADDR_ANY;
	if (-1 == bind(server_socket, (sockaddr*) &srvaddr, sizeof srvaddr)) {
	  perror(PACKAGE);
	  return 1;
	}
	if (-1 == listen(server_socket, 100)) {
	  perror(PACKAGE);
	  return 1;
	}
	if (verbosity > 0) {
	  std::cout << "Server at port " << port << std::endl;
	}
      }
    }

    if (engine == SAMPLING_ENGINE) {
      init_genrand(seed);
      std::cout << "Sampling engine: alpha=" << alpha << ", beta=" << beta
		<< ", delta=" << delta << ", seed=" << seed << std::endl;
      itimerval timer = { { 0L, 0L }, { 40000000L, 0L } };
      itimerval stimer;
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, 0);
      getitimer(ITIMER_VIRTUAL, &stimer);
#else
      setitimer(ITIMER_PROF, &timer, 0);
      getitimer(ITIMER_PROF, &stimer);
#endif
      const State init_state(*global_model);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
      long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
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
	double total_cached = 0.0;
	for (size_t i = 0; i < trials; i++) {
	  timeval start_time;
	  itimerval timer = { { 0, 0 }, { 40000000L, 0 } };
	  itimerval stimer;
	  if (server_socket != -1) {
	    gettimeofday(&start_time, 0);
	  } else {
#ifdef PROFILING
	    setitimer(ITIMER_VIRTUAL, &timer, 0);
	    getitimer(ITIMER_VIRTUAL, &stimer);
#else
	    setitimer(ITIMER_PROF, &timer, 0);
	    getitimer(ITIMER_PROF, &stimer);
#endif
	  }
	  bool sol = (*fi)->verify(*global_model, init_state,
				   indifference_region, alpha, beta,
				   algorithm);
	  total_cached += (*fi)->clear_cache();
	  double t;
	  if (server_socket != -1) {
	    timeval end_time;
	    gettimeofday(&end_time, 0);
	    long sec = end_time.tv_sec - start_time.tv_sec;
	    long usec = end_time.tv_usec - start_time.tv_usec;
	    if (usec < 0) {
	      sec--;
	      usec = 1000000 + usec;
	    }
	    t = std::max(0.0, sec + usec*1e-6);
	  } else {
#ifdef PROFILING
	    getitimer(ITIMER_VIRTUAL, &timer);
#else
	    getitimer(ITIMER_PROF, &timer);
#endif
	    long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
	    long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
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
	  double cached_avg = total_cached/trials;
	  std::cout << "Average model checking time: " << time_avg
		    << " seconds" << std::endl
		    << "Time standard deviation: " << sqrt(time_var)
		    << std::endl
		    << "Average number of samples: " << sample_avg << std::endl
		    << "Samples standard deviation: " << sqrt(sample_var)
		    << std::endl
		    << "Average path lengths: " << path_avg << std::endl
		    << accepts << " accepted, " << (trials - accepts)
		    << " rejected" << std::endl
		    << "Average cached: " << cached_avg << std::endl;
	}
      }
    } else if (engine == HYBRID_ENGINE) {
      std::cout << "Hybrid engine: epsilon=" << epsilon << std::endl;
      DdManager* dd_man = Cudd_Init(2*num_model_bits, 0, CUDD_UNIQUE_SLOTS,
				    CUDD_CACHE_SLOTS, 0);
      Cudd_SetEpsilon(dd_man, 1e-15);
      itimerval timer = { { 0L, 0L }, { 40000000L, 0L } };
      itimerval stimer;
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, 0);
      getitimer(ITIMER_VIRTUAL, &stimer);
#else
      setitimer(ITIMER_PROF, &timer, 0);
      getitimer(ITIMER_VIRTUAL, &stimer);
#endif
      global_model->cache_dds(dd_man, moments);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
      long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
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
	double total_time = 0.0;
	bool accepted = false;
	int old_verbosity = verbosity;
	for (size_t i = 0; i < trials; i++) {
	  itimerval timer = { { 0L, 0L }, { 40000000L, 0L } };
	  itimerval stimer;
#ifdef PROFILING
	  setitimer(ITIMER_VIRTUAL, &timer, 0);
	  getitimer(ITIMER_VIRTUAL, &stimer);
#else
	  setitimer(ITIMER_PROF, &timer, 0);
	  getitimer(ITIMER_PROF, &stimer);
#endif
	  DdNode* ddf = (*fi)->verify(dd_man, *global_model, epsilon,
				      estimate);
	  DdNode* sol = Cudd_bddAnd(dd_man, ddf, init);
	  Cudd_Ref(sol);
	  Cudd_RecursiveDeref(dd_man, ddf);
#ifdef PROFILING
	  getitimer(ITIMER_VIRTUAL, &timer);
#else
	  getitimer(ITIMER_PROF, &timer);
#endif
	  long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
	  long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
	  double t = std::max(0.0, sec + usec*1e-6);
	  total_time += t;
	  accepted = (sol != Cudd_ReadLogicZero(dd_man));
	  Cudd_RecursiveDeref(dd_man, sol);
	  if (t > 1.0) {
	    total_time *= trials;
	    break;
	  }
	  if (trials > 1) {
	    verbosity = 0;
	  }
	}
	verbosity = old_verbosity;
	std::cout << "Model checking completed in " << total_time/trials
		  << " seconds." << std::endl;
	if (accepted) {
	  std::cout << "Property is true in the initial state." << std::endl;
	} else {
	  std::cout << "Property is false in the initial state." << std::endl;
	}
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
      itimerval timer = { { 0L, 0L }, { 40000000L, 0L } };
      itimerval stimer;
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, 0);
      getitimer(ITIMER_VIRTUAL, &stimer);
#else
      setitimer(ITIMER_PROF, &timer, 0);
      getitimer(ITIMER_PROF, &stimer);
#endif
      global_model->cache_dds(dd_man, moments);
      const State init_state(*global_model);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
      long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
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
	  itimerval timer = { { 0L, 0L }, { 40000000L, 0L } };
	  itimerval stimer;
#ifdef PROFILING
	  setitimer(ITIMER_VIRTUAL, &timer, 0);
	  getitimer(ITIMER_VIRTUAL, &stimer);
#else
	  setitimer(ITIMER_PROF, &timer, 0);
	  getitimer(ITIMER_PROF, &stimer);
#endif
	  bool sol = (*fi)->verify(dd_man, *global_model, init_state,
				   indifference_region, alpha, beta, algorithm,
				   epsilon);
	  (*fi)->clear_cache();
#ifdef PROFILING
	  getitimer(ITIMER_VIRTUAL, &timer);
#else
	  getitimer(ITIMER_PROF, &timer);
#endif
	  long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
	  long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
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
    return 1;
  } catch (...) {
    std::cerr << std::endl << PACKAGE ": fatal error" << std::endl;
    return 1;
  }
  if (Distribution::mts != 0) {
    free_mt_struct(Distribution::mts);
  }

  return 0;
}

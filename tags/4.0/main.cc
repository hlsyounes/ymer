/*
 * Main program.
 *
 * Copyright (C) 2003--2005 Carnegie Mellon University
 * Copyright (C) 2011 Google Inc
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
#include "learning.h"
#include "simulator.h"
#if 0
#include "symbolic.h"
#endif
#include "sampling.h"
#include "comm.h"
#include "models.h"
#include "formulas.h"
#include "verbosity.h"
#include <sys/time.h>
#include <getopt.h>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#if HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
namespace std {
  typedef std::ostrstream ostringstream;
}
#endif


/* The parse function. */
extern int yyparse();
/* File to parse. */
extern FILE* yyin;
/* Current model. */
extern const Model* global_model;
/* Parsed properties. */
extern FormulaList properties;
/* Global path formula. */
extern const PathFormula* global_path_formula;
/* Clears all previously parsed declarations. */
extern void clear_declarations();

/* Name of current file. */
std::string current_file;
/* Global verbosity settings. */
static Verbosity verbosity_;
const Verbosity& verbosity = verbosity_;

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
  { "failure-analysis", no_argument, 0, 'F' },
  { "gamma", required_argument, 0, 'G' },
  { "host", required_argument, 0, 'H' },
  { "help", no_argument, 0, 'h' },
  { "memoization", no_argument, 0, 'M' },
  { "nested-error", required_argument, 0, 'n' },
  { "port", required_argument, 0, 'P' },
  { "estimate-probabilities", no_argument, 0, 'p' },
  { "sampling-algorithm", required_argument, 0, 's' },
  { "seed", required_argument, 0, 'S' },
  { "termination-probability", required_argument, 0, 't' },
  { "trials", required_argument, 0, 'T' },
  { "verbose", required_argument, 0, 'v' },
  { "version", no_argument, 0, 'V' },
  { "engine", required_argument, 0, 'x' },
  { 0, 0, 0, 0 }
};
static const char OPTION_STRING[] = "A:B:D:d:E:FG:H:hMn:P:ps:S:t:T:v:Vx:";


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
	    << "  -d d,  --relative-delta=d" << std::endl
	    << "\t\t\tuse indifference region of relative with sampling"
	    << std::endl
	    << "\t\t\t  engine (default is 1e-2)" << std::endl
	    << "  -E e,  --epsilon=e\t"
	    << "use precision e with numerical engine (default is 1e-6)"
	    << std::endl
	    << "  -F,    --failure-analysis" << std::endl
	    << "\t\t\tperform failure analysis after verification" << std::endl
	    << "  -G g,  --gamma=g\t"
	    << "use bound g on `don't care'" << std::endl
	    << "  -H h,  --host=h\t"
	    << "connect to server on host h" << std::endl
	    << "  -h,    --help\t\t"
	    << "display this help and exit" << std::endl
	    << "  -M,    --memoization\t"
	    << "use memoization for sampling engine" << std::endl
	    << "  -P p,  --port=p\t"
	    << "communicate using port p" << std::endl
	    << "  -p,    --estimate-probabilities" << std::endl
	    << "\t\t\testimates probabilities of path formulae holding"
	    << std::endl
	    << "  -s s,  --sampling-algorithm=s" << std::endl
	    << "\t\t\tuse sampling algorithm s"
	    << std::endl
	    << "  -S s,  --seed=s\t"
	    << "use seed s with random number generator" << std::endl
	    << "\t\t\t  (sampling engine only)" << std::endl
	    << "  -t t,  --termination-probability=t" << std::endl
	    << "\t\t\ttermination probability for nonparametric sampling"
	    << std::endl
	    << "  -T t,  --trials=t\t"
	    << "number of trials for sampling engine (default is 1)"
	    << std::endl
	    << "  -v v, --verbose v\t"
	    << "use verbosity v;" << std::endl
	    << "  -V,    --version\t"
	    << "display version information and exit" << std::endl
	    << "  -x e,  --engine=e\t"
	    << "use engine e; can be `sampling' (default) or `mtbdd'"
	    << std::endl
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
            << "Copyright (C) 2011 Google Inc" << std::endl
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
  if (yyin == 0) {
    std::cerr << PACKAGE << ':' << name << ": " << strerror(errno)
	      << std::endl;
    return false;
  } else {
    current_file = name;
    bool success = (yyparse() == 0);
    current_file.clear();
    fclose(yyin);
    return success;
  }
}


/* Parses verbosity settings. */
void parse_verbosity(char* arg) {
  while (arg != 0) {
    char* end = strchr(arg, ':');
    int n = (end != 0) ? end - arg : strlen(arg);
    if (n > 0) {
      if (0 == strncasecmp(arg, "model", n)) {
	verbosity_.model = true;
      } else if (0 == strncasecmp(arg, "modelsize", n)) {
	verbosity_.model_size = true;
      } else if (0 == strncasecmp(arg, "engine", n)) {
	verbosity_.engine = true;
      } else if (0 == strncasecmp(arg, "sample", n)) {
	verbosity_.sample = true;
      } else if (0 == strncasecmp(arg, "schedule", n)) {
	verbosity_.schedule = true;
      } else if (0 == strncasecmp(arg, "samplingplan", n)) {
	verbosity_.sampling_plan = true;
      } else {
	std::cerr << std::endl << PACKAGE ": " << "unknown verbosity setting `"
		  << std::string(arg, n) << "'" << std::endl;
      }
    }
    arg = (end != 0) ? end + 1 : 0;
  }
}


/* Indifference region function. */
double indifference_region(double theta) {
  if (relative_delta) {
    return 2*delta*((theta <= 0.5) ? theta : 1.0 - theta);
  } else {
    return delta;
  }
}


/* Client program. */
void client_main(const std::string& hostname, int port) {
  try {
    int sockfd = connect_to_server(hostname, port);
    int client_id;
    unsigned long seed;
    recv_register(client_id, seed, sockfd);
    if (verbosity.engine) {
      std::cout << "Client " << client_id << std::endl;
      std::cout << "Initializing random number generator...";
    }
    Distribution::seed(client_id, seed);
    if (verbosity.engine) {
      std::cout << "done" << std::endl;
    }
    recv_init(sockfd);
    while (true) {
      double alpha, beta, gamma;
      recv_start(alpha, beta, gamma, sockfd);
      while (!recv_stop(sockfd)) {
	int s = Sampling::sample(*global_path_formula, *global_model,
				 alpha, beta, gamma);
	send_sample(sockfd, s);
      }
    }
  } catch (const server_unavailable& e) {
    if (verbosity.engine) {
      std::cout << "Shutting down (server unavailable)" << std::endl;
    }
  }
  clear_declarations();
  RCObject::deref(global_path_formula);
  delete global_model;
}


/* The main program. */
int main(int argc, char* argv[]) {
  /* Set default seed. */
  unsigned long seed = time(0);
  /* Set default alpha. */
  double alpha = 1e-2;
  /* Set default beta. */
  double beta = 1e-2;
  /* Set default gamma. */
  double gamma = 0.0;
  /* Set default epsilon. */
  double epsilon = 1e-6;
  /* Whether a probability estimate is desired. */
  bool estimate = false;
  /* Sampling algorithm. */
  Sampling::Algorithm algorithm = Sampling::SPRT;
  /* Fixed nested error (negative value results in heuristic estimate). */
  double nested_error = -1.0;
  /* Whether memoization is enabled. */
  bool memoization = false;
  /* Solution engine. */
  enum { SAMPLING_ENGINE, MTBDD_ENGINE } engine = SAMPLING_ENGINE;
  /* Set default number of trials. */
  int trials = 1;
  /* Whether failure analysis should be performed. */
  bool failure_analysis = false;
  /* Server hostname */
  std::string hostname;
  /* Server port. */
  int port = -1;
  /* Termination probability for nonparametric engine. */
  double termination_prob = 1e-3;
  bool ignore_tp = true;

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
      case 'F':
	failure_analysis = true;
	break;
      case 'G':
	gamma = atof(optarg);
	if (gamma < 1e-10) {
	  throw std::invalid_argument("gamma < 1e-10");
	} else if (gamma >= 0.5) {
	  throw std::invalid_argument("gamma >= 0.5");
	}
	break;
      case 'H':
	hostname = optarg;
	break;
      case 'h':
	display_help();
	return 0;
      case 'M':
	memoization = true;
	break;
      case 'n':
	nested_error = atof(optarg);
	break;
      case 'P':
	port = atoi(optarg);
	if (port < 0 || port > 0xffff) {
	  throw std::invalid_argument("invalid port number");
	}
	break;
      case 'p':
	estimate = true;
	break;
      case 's':
	if (strcasecmp(optarg, "sssp") == 0) {
	  algorithm = Sampling::SSSP;
          ignore_tp = true;
	} else if (strcasecmp(optarg, "sprt") == 0) {
	  algorithm = Sampling::SPRT;
          ignore_tp = true;
	} else if (strcasecmp(optarg, "hoeffding") == 0) {
	  algorithm = Sampling::HOEFFDING;
          ignore_tp = true;
	} else if (strcasecmp(optarg, "nonparametric") == 0) {
	  algorithm = Sampling::NONPARAMETRIC;
          ignore_tp = false;
        } else if (strcasecmp(optarg, "sen") == 0) {
          algorithm = Sampling::SSSP;
          ignore_tp = false;
	} else {
	  throw std::invalid_argument("unsupported sampling algorithm `"
				      + std::string(optarg) + "'");
	}
	break;
      case 'S':
	seed = atoi(optarg);
	break;
      case 't':
        termination_prob = atof(optarg);
	break;
      case 'T':
	trials = atoi(optarg);
	break;
      case 'v':
	parse_verbosity(optarg);
	break;
      case 'V':
	display_version();
	return 0;
      case 'x':
	if (strcasecmp(optarg, "sampling") == 0) {
	  engine = SAMPLING_ENGINE;
	} else if (strcasecmp(optarg, "mtbdd") == 0) {
	  engine = MTBDD_ENGINE;
	} else {
	  throw std::invalid_argument("unsupported engine `"
				      + std::string(optarg) + "'");
	}
	break;
      case ':':
      default:
	std::cerr << "Try `" PACKAGE " --help' for more information."
		  << std::endl;
	return 1;
      }
    }
    if (ignore_tp) {
      termination_prob = 0.0;
    }

    std::cout.setf(std::ios::unitbuf);
    if (port >= 0) {
      if (!hostname.empty()) {
	client_main(hostname, port);
	return 0;
      } else {
	establish_server(port);
	if (verbosity.engine) {
	  std::cout << "Server at port " << port << std::endl;
	}
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

    if (verbosity.model) {
      std::cout << *global_model << std::endl;
    }

    Distribution::seed(seed);

    global_model->compile();
    const State init_state(*global_model);
    if (verbosity.model_size) {
      std::cout << "Variables: " << init_state.values().size() << std::endl;
      std::cout << "Events:    " << global_model->commands().size()
		<< std::endl;
    }
    if (engine == SAMPLING_ENGINE) {
      Sampling::set_parameters(indifference_region, algorithm, estimate,
			       nested_error, memoization, failure_analysis,
			       termination_prob);
      if (verbosity.engine) {
	std::cout << "Sampling engine: alpha = " << alpha << ", beta = "
		  << beta << ", delta = " << delta << ", seed = " << seed
		  << std::endl;
      }
      for (FormulaList::const_iterator fi = properties.begin();
	   fi != properties.end(); fi++) {
	if (verbosity.engine) {
	  std::cout << std::endl << "Model checking " << **fi << " ..."
		    << std::endl;
	}
	int accepts = 0, rejects = 0;
	std::vector<double> times;
	double total_time = 0.0;
	double memo_size = 0.0;
	for (int i = 1; i <= trials; i++) {
	  if (!verbosity.engine && !verbosity.sample && !verbosity.schedule) {
	    if (i % 1000 == 0) {
	      std::cout << ':';
	    } else if (i % 100 == 0) {
	      std::cout << '.';
	    }
	  }
	  itimerval timer = { { 0, 0 }, { 40000000L, 0 } };
	  itimerval stimer;
#ifdef PROFILING
	  setitimer(ITIMER_VIRTUAL, &timer, 0);
	  getitimer(ITIMER_VIRTUAL, &stimer);
#else
	  setitimer(ITIMER_PROF, &timer, 0);
	  getitimer(ITIMER_PROF, &stimer);
#endif
	  VerificationResult sol =
	    Sampling::verify(**fi, *global_model, alpha, beta, gamma);
	  memo_size += Sampling::clear_memo();
	  double t;
#ifdef PROFILING
	  getitimer(ITIMER_VIRTUAL, &timer);
#else
	  getitimer(ITIMER_PROF, &timer);
#endif
	  long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
	  long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
	  t = std::max(0.0, sec + usec*1e-6);
	  if (trials == 1) {
	    if (verbosity.engine) {
	      double sample_avg, sample_var, path_avg, path_var;
	      Sampling::get_statistics(sample_avg, sample_var,
				       path_avg, path_var);
	      std::cout << "Model checking completed in " << t << " seconds."
			<< std::endl
			<< "Average path length: " << path_avg << std::endl
			<< "Path length std. dev.: " << sqrt(path_var)
			<< std::endl;
	    }
	    if (sol == ACCEPT) {
	      std::cout << "Property is true in the initial state."
			<< std::endl;
	    } else if (sol == REJECT) {
	      std::cout << "Property is false in the initial state."
			<< std::endl;
	    } else {
	      std::cout << "UNDECIDED" << std::endl;
	    }
	    if (failure_analysis) {
	      analyze_trajectories(global_model->commands());
	      for (std::vector<const Trajectory*>::const_iterator ti =
		     success_trajectories.begin();
		   ti != success_trajectories.end(); ti++) {
		delete *ti;
	      }
	      success_trajectories.clear();
	      for (std::vector<const Trajectory*>::const_iterator ti =
		     failure_trajectories.begin();
		   ti != failure_trajectories.end(); ti++) {
		delete *ti;
	      }
	      failure_trajectories.clear();
	    }
	  } else {
            std::cout << '.';
	    if (sol == ACCEPT) {
	      accepts++;
	    } else if (sol == REJECT) {
	      rejects++;
	    }
	    total_time += t;
	    times.push_back(t);
	  }
	}
	if (trials > 1) {
	  double time_avg = total_time/trials;
	  double sample_avg, sample_var, path_avg, path_var;
	  Sampling::get_statistics(sample_avg, sample_var, path_avg, path_var);
	  double time_var = 0.0;
	  for (int i = 0; i < trials; i++) {
	    double diff = times[i] - time_avg;
	    time_var += diff*diff;
	  }
	  time_var /= trials - 1;
	  double memo_avg = memo_size/trials;
	  std::cout << std::endl << "Average model checking time: " << time_avg
		    << " seconds" << std::endl
		    << "Time standard deviation: " << sqrt(time_var)
		    << std::endl
		    << "Average number of samples: " << sample_avg
		    << std::endl
		    << "Samples standard deviation: " << sqrt(sample_var)
		    << std::endl
		    << "Average path lengths: " << path_avg << std::endl
		    << "Path lengths std. dev.: " << sqrt(path_var)
		    << std::endl
		    << accepts << " accepted, " << rejects << " rejected, "
		    << (trials - accepts - rejects) << " undecided"
		    << std::endl
		    << "Average memo size: " << memo_avg << std::endl;
	}
	Sampling::clear_statistics();
      }
    } else { /* engine == MTBDD_ENGINE */
      if (verbosity.engine) {
	std::cout << "Numerical (MTBDD) engine: epsilon = " << epsilon
		  << std::endl;
      }
      for (FormulaList::const_iterator fi = properties.begin();
	   fi != properties.end(); fi++) {
	if (verbosity.engine) {
	  std::cout << std::endl << "Model checking " << **fi << " ..."
		    << std::endl;
	}
	itimerval timer = { { 0, 0 }, { 40000000L, 0 } };
	itimerval stimer;
#ifdef PROFILING
	setitimer(ITIMER_VIRTUAL, &timer, 0);
	getitimer(ITIMER_VIRTUAL, &stimer);
#else
	setitimer(ITIMER_PROF, &timer, 0);
	getitimer(ITIMER_PROF, &stimer);
#endif
#if 0
	bool sol = Symbolic::verify(**fi, *global_model, init_state, epsilon);
	double t;
#ifdef PROFILING
	getitimer(ITIMER_VIRTUAL, &timer);
#else
	getitimer(ITIMER_PROF, &timer);
#endif
	long sec = stimer.it_value.tv_sec - timer.it_value.tv_sec;
	long usec = stimer.it_value.tv_usec - timer.it_value.tv_usec;
	t = std::max(0.0, sec + usec*1e-6);
	if (verbosity.engine) {
	  std::cout << "Model checking completed in " << t << " seconds."
		    << std::endl;
	}
	if (sol) {
	  std::cout << "Property is true in the initial state." << std::endl;
	} else {
	  std::cout << "Property is false in the initial state." << std::endl;
	}
#else
	throw std::logic_error("symbolic engine not implemented");
#endif
      }
    }

    delete global_model;
    for (FormulaList::const_iterator fi = properties.begin();
	 fi != properties.end(); fi++) {
      StateFormula::deref(*fi);
    }
    properties.clear();
  } catch (const std::exception& e) {
    std::cerr << std::endl << PACKAGE ": " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << std::endl << PACKAGE ": fatal error" << std::endl;
    return 1;
  }

  return 0;
}

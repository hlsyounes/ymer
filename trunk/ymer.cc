/*
 * Main program.
 *
 * Copyright (C) 2003 Carnegie Mellon University
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
 * $Id: ymer.cc,v 1.7 2003-11-12 03:56:52 lorens Exp $
 */
#include <config.h>
#include "states.h"
#include "models.h"
#include "formulas.h"
#include "rng.h"
#include <util.h>
#include <cudd.h>
#include <sys/time.h>
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

/* Program options. */
static struct option long_options[] = {
  { "alpha", required_argument, NULL, 'A' },
  { "beta", required_argument, NULL, 'B' },
  { "delta", required_argument, NULL, 'D' },
  { "epsilon", required_argument, NULL, 'E' },
  { "engine", required_argument, NULL, 'e' },
  { "seed", required_argument, NULL, 'S' },
  { "trials", required_argument, NULL, 'T' },
  { "verbose", optional_argument, NULL, 'v' },
  { "version", no_argument, NULL, 'V' },
  { "help", no_argument, NULL, '?' },
  { 0, 0, 0, 0 }
};
static const char OPTION_STRING[] = "A:B:D:E:e:S:T:v::V?";


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
	    << "  -S s,  --seed=s\t"
	    << "use seed s with random number generator"
	    << std::endl
	    << "\t\t\t  (sampling engine only)"
	    << std::endl
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
	    << "Copyright (C) 2003 Carnegie Mellon University" << std::endl
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
  /* Set default seed. */
  size_t seed = time(NULL);
  /* Set default number of trials. */
  size_t trials = 1;
  /* Set default verbosity. */
  verbosity = 0;

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
    if (engine == SAMPLING_ENGINE) {
      init_genrand(seed);
      std::cout << "Sampling engine: alpha=" << alpha << ", beta=" << beta
		<< ", delta=" << delta << ", seed=" << seed << std::endl;
      struct itimerval timer = { { 1000000, 900000 }, { 1000000, 900000 } };
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
      double t = 1000000.9
	- (timer.it_value.tv_sec + timer.it_value.tv_usec*1e-6);
      if (t < 1e-3) {
	t = 0.0;
      }
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
	size_t accepts = 0;
	std::vector<double> times;
	double total_time = 0.0;
	total_samples = 0;
	samples.clear();
	total_path_lengths = 0.0;
	for (size_t i = 0; i < trials; i++) {
	  struct itimerval timer =
	    { { 1000000, 900000 }, { 1000000, 900000 } };
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
	  double t = 1000000.9
	    - (timer.it_value.tv_sec + timer.it_value.tv_usec*1e-6);
	  if (t < 1e-3) {
	    t = 0.0;
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
      struct itimerval timer = { { 1000000, 900000 }, { 1000000, 900000 } };
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, NULL);
#else
      setitimer(ITIMER_PROF, &timer, NULL);
#endif
      global_model->cache_dds(dd_man);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      double t = 1000000.9
	- (timer.it_value.tv_sec + timer.it_value.tv_usec*1e-6);
      if (t < 1e-3) {
	t = 0.0;
      }
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
	struct itimerval timer = { { 1000000, 900000 }, { 1000000, 900000 } };
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
	double t = 1000000.9
	  - (timer.it_value.tv_sec + timer.it_value.tv_usec*1e-6);
	if (t < 1e-3) {
	  t = 0.0;
	}
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
      struct itimerval timer = { { 1000000, 900000 }, { 1000000, 900000 } };
#ifdef PROFILING
      setitimer(ITIMER_VIRTUAL, &timer, NULL);
#else
      setitimer(ITIMER_PROF, &timer, NULL);
#endif
      global_model->cache_dds(dd_man);
      const State init_state(*global_model);
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      double t = 1000000.9
	- (timer.it_value.tv_sec + timer.it_value.tv_usec*1e-6);
      if (t < 1e-3) {
	t = 0.0;
      }
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
	  struct itimerval timer =
	    { { 1000000, 900000 }, { 1000000, 900000 } };
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
	  double t = 1000000.9
	    - (timer.it_value.tv_sec + timer.it_value.tv_usec*1e-6);
	  if (t < 1e-3) {
	    t = 0.0;
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
      global_model->uncache_dds(dd_man);
      int unrel = Cudd_CheckZeroRef(dd_man);
      if (unrel != 0) {
	std::cerr << unrel << " unreleased DDs" << std::endl;
      }
      Cudd_Quit(dd_man);
    }
    delete global_model;
    for (FormulaList::const_iterator fi = properties.begin();
	 fi != properties.end(); fi++) {
      StateFormula::unregister_use(*fi);
    }
    properties.clear();
  } catch (const std::exception& e) {
    std::cerr << PACKAGE ": " << e.what() << std::endl;
    return -1;
  } catch (...) {
    std::cerr << PACKAGE ": fatal error" << std::endl;
    return -1;
  }

  return 0;
}

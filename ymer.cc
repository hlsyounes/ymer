/*
 * Main program.
 *
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by Håkan L. S. Younes.
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: ymer.cc,v 1.1 2003-08-10 01:51:48 lorens Exp $
 */
#include <config.h>
#include "models.h"
#include "exceptions.h"
#include "util.h"
#include "cudd.h"
#include <sys/time.h>
#if HAVE_GETOPT_LONG
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <getopt.h>
#else
#include "getopt.h"
#endif
#include <iostream>
#include <string>


/* The parse function. */
extern int yyparse();
/* File to parse. */
extern FILE* yyin;
/* Current model. */
extern const Model* global_model;
/* Number of bits required by binary encoding of state space. */
extern int num_model_bits;
/* Clears all previously parsed declarations. */
extern void clear_declarations();

/* Name of current file. */
std::string current_file;
/* Verbosity level. */
int verbosity;

/* Program options. */
static struct option long_options[] = {
  { "verbose", optional_argument, NULL, 'v' },
  { "version", no_argument, NULL, 'V' },
  { "help", no_argument, NULL, '?' },
  { 0, 0, 0, 0 }
};
static const char OPTION_STRING[] = "v::V?";


/* Displays help. */
static void display_help() {
  std::cout << "usage: " << PACKAGE << " [options] [file ...]" << std::endl
	    << "options:" << std::endl
	    << "  -v[n], --verbose[=n]\t"
	    << "use verbosity level n;" << std::endl
	    << "\t\t\t  n is a number from 0 (verbose mode off) and up;"
	    << std::endl
	    << "\t\t\t  default level is 1 if optional argument is left out"
	    << std::endl
	    << "  -V,    --version\t"
	    << "display version information and exit" << std::endl
	    << "  -?     --help\t\t"
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
  /* Set default verbosity. */
  verbosity = 0;

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

  try {
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
    if (verbosity > 1) {
      std::cout << *global_model << std::endl;
    }
    DdManager* dd_man = Cudd_Init(2*num_model_bits, 0, CUDD_UNIQUE_SLOTS,
				  CUDD_CACHE_SLOTS, 0);
    struct itimerval timer = { { 1000000, 900000 }, { 1000000, 900000 } };
#ifdef PROFILING
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
#else
    setitimer(ITIMER_PROF, &timer, NULL);
#endif
    DdNode* ddR = global_model->mtbdd(dd_man);
#ifdef PROFILING
    getitimer(ITIMER_VIRTUAL, &timer);
#else
    getitimer(ITIMER_PROF, &timer);
#endif
    double t = 1000000.9
      - (timer.it_value.tv_sec + timer.it_value.tv_usec*1e-6);
    std::cout << "Model built in " << std::max(0.0, t) << " seconds."
	      << std::endl;
    if (verbosity > 1) {
      Cudd_PrintDebug(dd_man, ddR, Cudd_ReadSize(dd_man), verbosity - 1);
    }
    Cudd_RecursiveDeref(dd_man, ddR);
    int unrel = Cudd_CheckZeroRef(dd_man);
    if (unrel != 0) {
      std::cerr << unrel << " unreleased DDs" << std::endl;
    }
    Cudd_Quit(dd_man);
    delete global_model;
  } catch (const Exception& e) {
    std::cerr << PACKAGE ": " << e << std::endl;
    return -1;
  } catch (...) {
    std::cerr << PACKAGE ": fatal error" << std::endl;
    return -1;
  }

  return 0;
}

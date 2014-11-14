/*
 * Failure analysis.
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
#include "learning.h"
#include "formulas.h"
#include "distributions.h"
#include "gsl/gsl_sf_gamma.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>


/* Success trajectories. */
std::vector<const Trajectory*> success_trajectories;
/* Failure trajectories. */
std::vector<const Trajectory*> failure_trajectories;


/* Failure analysis. */
void analyze_trajectories(const CommandList& commands) {
  std::cout << "positive: " << success_trajectories.size()
	    << "; negative: " << failure_trajectories.size()
	    << "; success ratio: "
	    << (double(success_trajectories.size())
		/(success_trajectories.size() + failure_trajectories.size()))
	    << std::endl;
  std::map<const Constant<double>*, int> parameters;
  std::vector<int> command_index(commands.size(), -1);
  for (size_t i = 0; i < commands.size(); i++) {
    const Exponential* dist =
      dynamic_cast<const Exponential*>(&commands[i]->delay());
    if (dist != 0) {
      const Constant<double>* rate =
	dynamic_cast<const Constant<double>*>(&dist->rate());
      if (rate != 0) {
	std::map<const Constant<double>*, int>::const_iterator ri =
	  parameters.find(rate);
	if (ri != parameters.end()) {
	  command_index[i] = (*ri).second;
	} else {
	  command_index[i] = parameters.size();
	  parameters.insert(std::make_pair(rate, parameters.size()));
	}
      }
    }
  }
  std::vector<int> success_count(parameters.size(), 0);
  std::vector<double> success_sum(parameters.size(), 0.0);
  std::vector<int> failure_count(parameters.size(), 0);
  std::vector<double> failure_sum(parameters.size(), 0.0);
  std::vector<double> holding_times(commands.size(), 0.0);
  for (std::vector<const Trajectory*>::const_iterator ti =
	 success_trajectories.begin();
       ti != success_trajectories.end(); ti++) {
    const Trajectory* node = *ti;
    do {
      for (size_t i = 0; i < commands.size(); i++) {
	int j = command_index[i];
	if (j >= 0) {
	  if (commands[i]->guard().holds(node->state())) {
	    holding_times[i] += node->time();
	    if (commands[i] == node->trigger()) {
	      // record holding_times[i] as a regular sample for commands[i]
	      success_count[j]++;
	      success_sum[j] += holding_times[i];
	      holding_times[i] = 0.0;
	    }
	  } else if (holding_times[i] > 0.0) {
	    // record holding_times[i] as censored sample for commands[i]
	    success_sum[j] += holding_times[i];
	    holding_times[i] = 0.0;
	  }
	}
      }
      node = node->next();
    } while (node != 0);
    for (size_t i = 0; i < commands.size(); i++) {
      int j = command_index[i];
      if (j >= 0) {
	if (holding_times[i] > 0.0) {
	  // record holding_times[i] as censored sample for commands[i]
	  success_sum[j] += holding_times[i];
	  holding_times[i] = 0.0;
	}
      }
    }
  }
  for (std::vector<const Trajectory*>::const_iterator ti =
	 failure_trajectories.begin();
       ti != failure_trajectories.end(); ti++) {
    const Trajectory* node = *ti;
    do {
      for (size_t i = 0; i < commands.size(); i++) {
	int j = command_index[i];
	if (j >= 0) {
	  if (commands[i]->guard().holds(node->state())) {
	    holding_times[i] += node->time();
	    if (commands[i] == node->trigger()) {
	      // record holding_times[i] as a regular sample for commands[i]
	      failure_count[j]++;
	      failure_sum[j] += holding_times[i];
	      holding_times[i] = 0.0;
	    }
	  } else if (holding_times[i] > 0.0) {
	    // record holding_times[i] as censored sample for commands[i]
	    failure_sum[j] += holding_times[i];
	    holding_times[i] = 0.0;
	  }
	}
      }
      node = node->next();
    } while (node != 0);
    for (size_t i = 0; i < commands.size(); i++) {
      int j = command_index[i];
      if (j >= 0) {
	if (holding_times[i] > 0.0) {
	  // record holding_times[i] as censored sample for commands[i]
	  failure_sum[j] += holding_times[i];
	  holding_times[i] = 0.0;
	}
      }
    }
  }
  for (std::map<const Constant<double>*, int>::const_iterator ri =
	 parameters.begin();
       ri != parameters.end(); ri++) {
    const Constant<double>& rate = *(*ri).first;
    int i = (*ri).second;
    double success_rate = success_count[i]/success_sum[i];
    double failure_rate = failure_count[i]/failure_sum[i];
    double total_rate =
      (success_count[i] + failure_count[i])/(success_sum[i] + failure_sum[i]);
    double success_ratio =
      -2.0*(success_count[i]*log(rate.value()/success_rate)
	    + (success_rate - rate.value())*success_sum[i]);
    double failure_ratio =
      -2.0*(failure_count[i]*log(rate.value()/failure_rate)
	    + (failure_rate - rate.value())*failure_sum[i]);
    std::cout << rate << '\t' << std::setw(10) << rate.value()
	      << std::setw(10) << success_rate
	      << std::setw(10) << failure_rate
	      << std::setw(10) << total_rate
	      << std::setw(10) << gsl_sf_gamma_inc_P(0.5, 0.5*success_ratio)
	      << std::setw(10) << gsl_sf_gamma_inc_P(0.5, 0.5*failure_ratio)
	      << std::endl;
  }
}

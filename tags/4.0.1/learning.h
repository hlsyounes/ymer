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
#ifndef LEARNING_H
#define LEARNING_H

#include "trajectories.h"
#include "modules.h"
#include <vector>


/* Success trajectories. */
extern std::vector<const Trajectory*> success_trajectories;
/* Failure trajectories. */
extern std::vector<const Trajectory*> failure_trajectories;


/* Failure analysis. */
extern void analyze_trajectories(const CommandList& commands);


#endif /* LEARNING_H */

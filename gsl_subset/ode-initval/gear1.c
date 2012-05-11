/* ode-initval/gear1.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Gear 1 */

/* Author:  G. Jungman
 */
#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv.h>

#include "odeiv_util.h"

typedef struct
{
  double *k;
  double *y0;
}
gear1_state_t;

static void *
gear1_alloc (size_t dim)
{
  gear1_state_t *state = (gear1_state_t *) malloc (sizeof (gear1_state_t));

  if (state == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for gear1_state", GSL_ENOMEM);
    }

  state->k = (double *) malloc (dim * sizeof (double));

  if (state->k == 0)
    {
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for k", GSL_ENOMEM);
    }

  state->y0 = (double *) malloc (dim * sizeof (double));

  if (state->y0 == 0)
    {
      free (state->k);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for y0", GSL_ENOMEM);
    }

  return state;
}

static int
gear1_apply(void * vstate,
            size_t dim,
            double t,
            double h,
            double y[],
            double yerr[],
            const double dydt_in[],
            double dydt_out[],
            const gsl_odeiv_system * sys)
{
  gear1_state_t *state = (gear1_state_t *) vstate;

  const int iter_steps = 3;
  int status = 0;
  int nu;
  size_t i;

  double * const k = state->k;
  double * const y0 = state->y0;

  DISCARD_POINTER(dydt_in); /* prevent warning about unused parameter */

  DBL_MEMCPY(y0, y, dim);

  /* iterative solution */
  for(nu=0; nu<iter_steps; nu++) {
    int s = GSL_ODEIV_FN_EVAL(sys, t + h, y, k);
    GSL_STATUS_UPDATE(&status, s);
    for(i=0; i<dim; i++) {
      y[i] = y0[i] + h * k[i];
    }
  }

  /* fudge the error estimate */
  for(i=0; i<dim; i++) {
    yerr[i] = h * h * k[i];
  }

  if(dydt_out != NULL) {
    DBL_MEMCPY(dydt_out, k, dim);
  }

  return status;
}

static int
gear1_reset (void *vstate, size_t dim)
{
  gear1_state_t *state = (gear1_state_t *) vstate;

  DBL_ZERO_MEMSET (state->k, dim);
  DBL_ZERO_MEMSET (state->y0, dim);

  return GSL_SUCCESS;
}

static unsigned int
gear1_order (void *vstate)
{
  gear1_state_t *state = (gear1_state_t *) vstate;
  state = 0; /* prevent warnings about unused parameters */
  return 2;
}

static void
gear1_free (void *vstate)
{
  gear1_state_t *state = (gear1_state_t *) vstate;
  free (state->k);
  free (state->y0);
  free (state);
}

static const gsl_odeiv_step_type gear1_type = { "gear1",        /* name */
  1,                            /* can use dydt_in */
  0,                            /* gives exact dydt_out */
  &gear1_alloc,
  &gear1_apply,
  &gear1_reset,
  &gear1_order,
  &gear1_free
};

const gsl_odeiv_step_type *gsl_odeiv_step_gear1 = &gear1_type;

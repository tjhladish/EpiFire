/* ode-initval/gear2.c
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

/* Gear 2 */

/* Author:  G. Jungman
 */
#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>
#include "odeiv_util.h"
#include <gsl/gsl_odeiv.h>


/* gear2 state object */
typedef struct {
  int primed;                /* flag indicating that yim1 is ready */
  double t_primed;           /* system was primed for this value of t */
  double last_h;             /* last step size */
  gsl_odeiv_step * primer;   /* stepper to use for priming */
  double * yim1;             /* y_{i-1}    */
  double * k;                /* work space */
  double * y0;               /* work space */
  int stutter;
}
gear2_state_t;

static void *
gear2_alloc (size_t dim)
{
  gear2_state_t *state = (gear2_state_t *) malloc (sizeof (gear2_state_t));

  if (state == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for gear2_state", GSL_ENOMEM);
    }

  state->yim1 = (double *) malloc (dim * sizeof (double));

  if (state->yim1 == 0)
    {
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for yim1", GSL_ENOMEM);
    }

  state->k = (double *) malloc (dim * sizeof (double));

  if (state->k == 0)
    {
      free (state->yim1);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for k", GSL_ENOMEM);
    }

  state->y0 = (double *) malloc (dim * sizeof (double));

  if (state->y0 == 0)
    {
      free (state->k);
      free (state->yim1);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for y0", GSL_ENOMEM);
    }

  state->primed = 0;
  state->primer = gsl_odeiv_step_alloc (gsl_odeiv_step_rk4imp, dim);
  state->last_h = 0.0;

  return state;
}

static int
gear2_apply(void * vstate,
            size_t dim,
            double t,
            double h,
            double y[],
            double yerr[],
            const double dydt_in[],
            double dydt_out[],
            const gsl_odeiv_system * sys)
{
  gear2_state_t *state = (gear2_state_t *) vstate;

  state->stutter = 0;

  if(state->primed == 0 || t == state->t_primed || h != state->last_h) {

    /* Execute a single-step method to prime the process.  Note that
     * we do this if the step size changes, so frequent step size
     * changes will cause the method to stutter. 
     * 
     * Note that we reuse this method if the time has not changed,
     * which can occur when the adaptive driver is attempting to find
     * an appropriate step-size on its first iteration */

    int status;
    DBL_MEMCPY(state->yim1, y, dim);
    
    status = gsl_odeiv_step_apply(state->primer, t, h, y, yerr, dydt_in, dydt_out, sys);

    /* Make note of step size and indicate readiness for a Gear step. */

    state->primed = 1;
    state->t_primed = t;
    state->last_h = h;
    state->stutter = 1;

    return status;
  } else {
    /* We have a previous y value in the buffer, and the step
     * sizes match, so we go ahead with the Gear step.
     */

    double * const k  = state->k;
    double * const y0 = state->y0;
    double * const yim1 = state->yim1;

    const int iter_steps = 3;
    int status = 0;
    int nu;
    size_t i;

    DBL_MEMCPY(y0, y, dim);

    /* iterative solution */

    if(dydt_out != NULL) {
      DBL_MEMCPY(k, dydt_out, dim);
    }

    for(nu=0; nu<iter_steps; nu++) {
      int s = GSL_ODEIV_FN_EVAL(sys, t + h, y, k);
      GSL_STATUS_UPDATE(&status, s);
      for(i=0; i<dim; i++) {
        y[i] = ((4.0 * y0[i] - yim1[i]) + 2.0 * h * k[i]) / 3.0;
      }
    }

    /* Estimate error and update the state buffer. */
    for(i=0; i<dim; i++) {
      yerr[i] = h * h * (y[i] - y0[i]); /* error is third order */
      yim1[i] = y0[i];
    }

    /* Make note of step size. */
    state->last_h = h;

    return status;
  }
}

static int
gear2_reset(void * vstate, size_t dim)
{
  gear2_state_t *state = (gear2_state_t *) vstate;

  DBL_ZERO_MEMSET (state->yim1, dim);
  DBL_ZERO_MEMSET (state->k, dim);
  DBL_ZERO_MEMSET (state->y0, dim);

  state->primed = 0;
  state->last_h = 0.0;
  return GSL_SUCCESS;
}

static unsigned int
gear2_order (void *vstate)
{
  gear2_state_t *state = (gear2_state_t *) vstate;
  state = 0; /* prevent warnings about unused parameters */
  return 3;
}

static void
gear2_free(void *vstate)
{
  gear2_state_t *state = (gear2_state_t *) vstate;

  free(state->yim1);
  free(state->k);
  free(state->y0);
  gsl_odeiv_step_free(state->primer);

  free(state);
}

static const gsl_odeiv_step_type gear2_type = { "gear2",        /* name */
  1,                            /* can use dydt_in */
  0,                            /* gives exact dydt_out */
  &gear2_alloc,
  &gear2_apply,
  &gear2_reset,
  &gear2_order,
  &gear2_free
};

const gsl_odeiv_step_type *gsl_odeiv_step_gear2 = &gear2_type;

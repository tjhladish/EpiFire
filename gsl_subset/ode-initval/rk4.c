/* ode-initval/rk4.c
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

/* Runge-Kutta 4, Classical */

/* Author:  G. Jungman
 */
#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv.h>

#include "odeiv_util.h"

typedef struct
{
  double *k;
  double *y0;
  double *ytmp;
}
rk4_state_t;

static void *
rk4_alloc (size_t dim)
{
  rk4_state_t *state = (rk4_state_t *) malloc (sizeof (rk4_state_t));

  if (state == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for rk4_state", GSL_ENOMEM);
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

  state->ytmp = (double *) malloc (dim * sizeof (double));

  if (state->ytmp == 0)
    {
      free (state->y0);
      free (state->k);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for ytmp", GSL_ENOMEM);
    }

  return state;
}


static int
rk4_apply (void *vstate,
           size_t dim,
           double t,
           double h,
           double y[],
           double yerr[],
           const double dydt_in[],
           double dydt_out[], 
           const gsl_odeiv_system * sys)
{
  rk4_state_t *state = (rk4_state_t *) vstate;

  size_t i;
  int status = 0;

  double *const k = state->k;
  double *const y0 = state->y0;
  double *const ytmp = state->ytmp;


  /* Copy the starting value. We will write over
   * the y[] vector, using it for scratch and
   * then filling it with the final result.
   */

  DBL_MEMCPY (y0, y, dim);

  if (dydt_in != NULL)
    {
      DBL_MEMCPY (k, dydt_in, dim);
    }
  else
    {
      int s = GSL_ODEIV_FN_EVAL (sys, t, y0, k);
      GSL_STATUS_UPDATE (&status, s);
    }

  for (i = 0; i < dim; i++)
    {
      y[i] = h / 6.0 * k[i];    /* use y[] to store delta_y */
      ytmp[i] = y0[i] + 0.5 * h * k[i];
    }

  /* k2 step */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + 0.5 * h, ytmp, k);
    GSL_STATUS_UPDATE (&status, s);
  }

  for (i = 0; i < dim; i++)
    {
      y[i] += h / 3.0 * k[i];
      ytmp[i] = y0[i] + 0.5 * h * k[i];
    }

  /* k3 step */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + 0.5 * h, ytmp, k);
    GSL_STATUS_UPDATE (&status, s);
  }

  for (i = 0; i < dim; i++)
    {
      y[i] += h / 3.0 * k[i];
      ytmp[i] = y0[i] + h * k[i];
    }

  /* k4 step, error estimate, and final sum */
  {
    int s = GSL_ODEIV_FN_EVAL (sys, t + h, ytmp, k);
    GSL_STATUS_UPDATE (&status, s);
  }

  for (i = 0; i < dim; i++)
    {
      y[i] += h / 6.0 * k[i];
      yerr[i] = h * y[i];
      y[i] += y0[i];
      if (dydt_out != NULL)
        dydt_out[i] = k[i];
    }

  return status;
}

static int
rk4_reset (void *vstate, size_t dim)
{
  rk4_state_t *state = (rk4_state_t *) vstate;

  DBL_ZERO_MEMSET (state->k, dim);
  DBL_ZERO_MEMSET (state->y0, dim);
  DBL_ZERO_MEMSET (state->ytmp, dim);

  return GSL_SUCCESS;
}

static unsigned int
rk4_order (void *vstate)
{
  rk4_state_t *state = (rk4_state_t *) vstate;
  state = 0; /* prevent warnings about unused parameters */
  return 4;
}

static void
rk4_free (void *vstate)
{
  rk4_state_t *state = (rk4_state_t *) vstate;
  free (state->k);
  free (state->y0);
  free (state->ytmp);
  free (state);
}

static const gsl_odeiv_step_type rk4_type = { "rk4",    /* name */
  1,                            /* can use dydt_in */
  0,                            /* gives exact dydt_out */
  &rk4_alloc,
  &rk4_apply,
  &rk4_reset,
  &rk4_order,
  &rk4_free
};

const gsl_odeiv_step_type *gsl_odeiv_step_rk4 = &rk4_type;

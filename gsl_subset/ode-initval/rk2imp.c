/* ode-initval/rk2imp.c
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

/* Runge-Kutta 2, Gaussian implicit */

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
  double *knu;
  double *ytmp;
}
rk2imp_state_t;

static void *
rk2imp_alloc (size_t dim)
{
  rk2imp_state_t *state = (rk2imp_state_t *) malloc (sizeof (rk2imp_state_t));

  if (state == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for rk2imp_state",
                      GSL_ENOMEM);
    }

  state->knu = (double *) malloc (dim * sizeof (double));

  if (state->knu == 0)
    {
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for knu", GSL_ENOMEM);
    }

  state->ytmp = (double *) malloc (dim * sizeof (double));

  if (state->ytmp == 0)
    {
      free (state->knu);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for ytmp", GSL_ENOMEM);
    }

  return state;
}


static int
rk2imp_apply (void *vstate,
              size_t dim,
              double t,
              double h,
              double y[],
              double yerr[],
              const double dydt_in[],
              double dydt_out[], const gsl_odeiv_system * sys)
{
  rk2imp_state_t *state = (rk2imp_state_t *) vstate;

  const int iter_steps = 3;
  int status = 0;
  int nu;
  size_t i;

  double *const knu = state->knu;
  double *const ytmp = state->ytmp;

  /* initialization step */
  if (dydt_in != NULL)
    {
      DBL_MEMCPY (knu, dydt_in, dim);
    }
  else
    {
      int s = GSL_ODEIV_FN_EVAL (sys, t, y, knu);
      GSL_STATUS_UPDATE (&status, s);
    }

  /* iterative solution */
  for (nu = 0; nu < iter_steps; nu++)
    {
      for (i = 0; i < dim; i++)
        {
          ytmp[i] = y[i] + 0.5 * h * knu[i];
        }
      {
        int s = GSL_ODEIV_FN_EVAL (sys, t + 0.5 * h, ytmp, knu);
        GSL_STATUS_UPDATE (&status, s);
      }
    }

  /* assignment */
  for (i = 0; i < dim; i++)
    {
      y[i] += h * knu[i];
      yerr[i] = h * h * knu[i];
      if (dydt_out != NULL)
        dydt_out[i] = knu[i];
    }

  return status;
}


static int
rk2imp_reset (void *vstate, size_t dim)
{
  rk2imp_state_t *state = (rk2imp_state_t *) vstate;

  DBL_ZERO_MEMSET (state->knu, dim);
  DBL_ZERO_MEMSET (state->ytmp, dim);

  return GSL_SUCCESS;
}

static unsigned int
rk2imp_order (void *vstate)
{
  rk2imp_state_t *state = (rk2imp_state_t *) vstate;
  state = 0; /* prevent warnings about unused parameters */
  return 2;
}

static void
rk2imp_free (void *vstate)
{
  rk2imp_state_t *state = (rk2imp_state_t *) vstate;
  free (state->knu);
  free (state->ytmp);
  free (state);
}

static const gsl_odeiv_step_type rk2imp_type = { "rk2imp",      /* name */
  1,                            /* can use dydt_in */
  0,                            /* gives exact dydt_out */
  &rk2imp_alloc,
  &rk2imp_apply,
  &rk2imp_reset,
  &rk2imp_order,
  &rk2imp_free
};

const gsl_odeiv_step_type *gsl_odeiv_step_rk2imp = &rk2imp_type;

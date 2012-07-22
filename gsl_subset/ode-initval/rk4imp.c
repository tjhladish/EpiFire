/* ode-initval/rk4imp.c
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

/* Runge-Kutta 4, Gaussian implicit */

/* Author:  G. Jungman */

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv.h>

#include "odeiv_util.h"

typedef struct
{
  double *k1nu;
  double *k2nu;
  double *ytmp1;
  double *ytmp2;
}
rk4imp_state_t;

static void *
rk4imp_alloc (size_t dim)
{
  rk4imp_state_t *state = (rk4imp_state_t *) malloc (sizeof (rk4imp_state_t));

  if (state == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for rk4imp_state",
                      GSL_ENOMEM);
    }

  state->k1nu = (double *) malloc (dim * sizeof (double));

  if (state->k1nu == 0)
    {
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for k1nu", GSL_ENOMEM);
    }

  state->k2nu = (double *) malloc (dim * sizeof (double));

  if (state->k2nu == 0)
    {
      free (state->k1nu);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for k2nu", GSL_ENOMEM);
    }

  state->ytmp1 = (double *) malloc (dim * sizeof (double));

  if (state->ytmp1 == 0)
    {
      free (state->k2nu);
      free (state->k1nu);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for ytmp1", GSL_ENOMEM);
    }

  state->ytmp2 = (double *) malloc (dim * sizeof (double));

  if (state->ytmp2 == 0)
    {
      free (state->ytmp1);
      free (state->k2nu);
      free (state->k1nu);
      free (state);
      GSL_ERROR_NULL ("failed to allocate space for ytmp2", GSL_ENOMEM);
    }

  return state;
}


static int
rk4imp_apply (void *vstate,
              size_t dim,
              double t,
              double h,
              double y[],
              double yerr[],
              const double dydt_in[],
              double dydt_out[], 
              const gsl_odeiv_system * sys)
{
  rk4imp_state_t *state = (rk4imp_state_t *) vstate;

  const double ir3 = 1.0 / M_SQRT3;
  const int iter_steps = 3;
  int status = 0;
  int nu;
  size_t i;

  double *const k1nu = state->k1nu;
  double *const k2nu = state->k2nu;
  double *const ytmp1 = state->ytmp1;
  double *const ytmp2 = state->ytmp2;

  /* initialization step */
  if (dydt_in != 0)
    {
      DBL_MEMCPY (k1nu, dydt_in, dim);
    }
  else
    {
      int s = GSL_ODEIV_FN_EVAL (sys, t, y, k1nu);
      GSL_STATUS_UPDATE (&status, s);
    }

  DBL_MEMCPY (k2nu, k1nu, dim);

  /* iterative solution */
  for (nu = 0; nu < iter_steps; nu++)
    {
      for (i = 0; i < dim; i++)
        {
          ytmp1[i] =
            y[i] + h * (0.25 * k1nu[i] + 0.5 * (0.5 - ir3) * k2nu[i]);
          ytmp2[i] =
            y[i] + h * (0.25 * k2nu[i] + 0.5 * (0.5 + ir3) * k1nu[i]);
        }
      {
        int s =
          GSL_ODEIV_FN_EVAL (sys, t + 0.5 * h * (1.0 - ir3), ytmp1, k1nu);
        GSL_STATUS_UPDATE (&status, s);
      }
      {
        int s =
          GSL_ODEIV_FN_EVAL (sys, t + 0.5 * h * (1.0 + ir3), ytmp2, k2nu);
        GSL_STATUS_UPDATE (&status, s);
      }
    }

  /* assignment */
  for (i = 0; i < dim; i++)
    {
      const double d_i = 0.5 * (k1nu[i] + k2nu[i]);
      if (dydt_out != NULL)
        dydt_out[i] = d_i;
      y[i] += h * d_i;
      yerr[i] = h * h * d_i;    /* FIXME: is this an overestimate ? */
    }

  return status;
}

static int
rk4imp_reset (void *vstate, size_t dim)
{
  rk4imp_state_t *state = (rk4imp_state_t *) vstate;

  DBL_ZERO_MEMSET (state->k1nu, dim);
  DBL_ZERO_MEMSET (state->k2nu, dim);
  DBL_ZERO_MEMSET (state->ytmp1, dim);
  DBL_ZERO_MEMSET (state->ytmp2, dim);

  return GSL_SUCCESS;
}

static unsigned int
rk4imp_order (void *vstate)
{
  rk4imp_state_t *state = (rk4imp_state_t *) vstate;
  state = 0; /* prevent warnings about unused parameters */
  return 4;
}

static void
rk4imp_free (void *vstate)
{
  rk4imp_state_t *state = (rk4imp_state_t *) vstate;
  free (state->k1nu);
  free (state->k2nu);
  free (state->ytmp1);
  free (state->ytmp2);
  free (state);
}

static const gsl_odeiv_step_type rk4imp_type = { "rk4imp",      /* name */
  1,                            /* can use dydt_in */
  0,                            /* gives exact dydt_out */
  &rk4imp_alloc,
  &rk4imp_apply,
  &rk4imp_reset,
  &rk4imp_order,
  &rk4imp_free
};

const gsl_odeiv_step_type *gsl_odeiv_step_rk4imp = &rk4imp_type;

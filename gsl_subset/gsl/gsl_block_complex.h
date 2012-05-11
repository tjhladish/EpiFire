#ifndef __GSL_BLOCK_COMPLEX_H__
#define __GSL_BLOCK_COMPLEX__

#define  GSL_BLOCK_REAL(z, i)  ((z)->data[2*(i)])
#define  GSL_BLOCK_IMAG(z, i)  ((z)->data[2*(i) + 1])

#if GSL_RANGE_CHECK
#define GSL_BLOCK_COMPLEX(zv, i) (((i) >= (zv)->size ? (gsl_error ("index out of range", __FILE__, __LINE__, GSL_EINVAL), 0):0 , *GSL_COMPLEX_AT((zv),(i))))
#else
#define GSL_BLOCK_COMPLEX(zv, i) (GSL_COMPLEX_AT((zv),(i)))
#endif

#define GSL_COMPLEX_AT(zv,i) ((gsl_complex*)&((zv)->data[2*(i)]))
#define GSL_COMPLEX_FLOAT_AT(zv,i) ((gsl_complex_float*)&((zv)->data[2*(i)]))
#define GSL_COMPLEX_LONG_DOUBLE_AT(zv,i) ((gsl_complex_long_double*)&((zv)->data[2*(i)]))

#endif /* __GSL_BLOCK_COMPLEX__ */

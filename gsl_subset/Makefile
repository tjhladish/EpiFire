CFLAGS=--ansi --pedantic -O2

all: objects libgsl_subset

objects: 
	gcc -c cblas/*.c blas/*.c err/*.c ode-initval/*.c -I. 
	
libgsl_subset: 
	ar -rv libgsl_subset.a *.o

clean:
	rm -f *.o libgsl_subset.a


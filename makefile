CC = gcc
FLAG = -fopenmp

all: myocean myocean-omp

myocean: myocean.c
	$(CC) $(FLAG) $^ -o $@

myocean-omp: myocean-omp.c
	$(CC) $(FLAG) $^ -o $@

clean:
	rm -f myocean myocean-omp

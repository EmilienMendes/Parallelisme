GCC := gcc
MPICC := mpicc
OPENMP := openmp
HOST_LIST := 17 18 19 20 21
HOSTS_FILE := composition_cluster

all: MPI
	
	
bruteforce: bruteforce.c
	$(MPICC) -Wall -f$(OPENMP) -o bruteforce bruteforce.c md5.c conversion_hash.c

sequentiel : 
	$(GCC) -o sequentiel sequentiel.c md5.c conversion_hash.c
	./sequentiel

MPI: bruteforce
	echo $(foreach host,$(HOST_LIST),fst-o-i-211-$(host).unilim.fr) | tr ' ' '\n' > $(HOSTS_FILE)
	mpirun -np 5 -hostfile $(HOSTS_FILE) ./bruteforce

OPENMP: bruteforce
		./bruteforce

clean:
	rm -f bruteforce
	rm -f sequentiel
	rm -f $(HOSTS_FILE)

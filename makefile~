main:
	@clear
	@mpicc hotpotato.c -o hotpotato
	@echo "Compilación exitosa..."
	@mpirun -np 5 ./hotpotato -t 10

clean:
	@echo "Limpiando..."	
	@rm 	hotpotato


main:
	@clear
	@u++ exp.cpp -o exp -multi
	@echo "Compilación exitosa..."
	@./exp -i smalldna.txt -o output.txt -h 4 -L 10 -l 10

clean:
	@echo "Limpiando..."	
	@rm exp output.txt


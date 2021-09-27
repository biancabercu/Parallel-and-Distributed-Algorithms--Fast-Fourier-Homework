all: homeworkFT homeworkFFT inputGenerator compareOutputs rosetta

compareOutputs: compareOutputs.c
	gcc $? -o $@ -lpthread -lm 

inputGenerator: inputGenerator.c
	#gcc -o inputGenerator inputGenerator.c -O3 -lm -Wall
	gcc $? -o $@ -lpthread -lm 

homeworkFT: homeworkFT.c
	gcc $? -o $@ -lpthread -lm 

homeworkFFT: homeworkFFT.c
	gcc $? -o $@ -lpthread -lm 

rosetta: rosetta.c
	gcc $? -o $@ -lpthread -lm 

.PHONY: clean
clean:
	rm -f homeworkFFT homeworkFT inputGenerator compareOutputs rosetta
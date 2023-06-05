filename = MiniDataSet
k = 4

all: main
	@echo All programs compiled.

main: main.cpp SparseGraph.cpp
	@echo Compiling main module...
	@g++ -o main -O2 main.cpp SparseGraph.cpp
	@echo Finished.

.PHONY: run clean cleandata

run:
	./main "./$(filename).txt" $(k)

clean:
	rm -f main
	mkdir -p tempcc && rm -f tempcc/*.txt
	mkdir -p kvcc && rm -f kvcc/*.txt
	mkdir -p temppart && rm -f temppart/*.txt

cleandata:
	mkdir -p tempcc && rm -f tempcc/*.txt
	mkdir -p kvcc && rm -f kvcc/*.txt
	mkdir -p temppart && rm -f temppart/*.txt

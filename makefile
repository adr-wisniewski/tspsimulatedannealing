all: porr porromp

porr:
	g++ -Wall -o porr *.cpp -lrt

porromp:
	g++ -fopenmp -Wall -o porromp *.cpp -lrt

clean:
	rm -f porr porromp *.o

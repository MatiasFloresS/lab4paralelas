#include <fstream>
#include <iostream>
#include <uC++.h>
#include <cstdlib>	
#include <string>
#include <sstream>
#include <getopt.h>
using namespace std;

#define FIRST_ELEMENT 0

string recognizer(string line);

_Monitor BoundedBuffer{
	uCondition full, empty;
	int front, back, count;
	int Largo_buffer;
	string *array;

public:
	BoundedBuffer(): front(0), back(0), count(0) {}
	BoundedBuffer(int largo) : front(0), back(0), count(0), Largo_buffer(largo), array(new string[Largo_buffer]) {}
	~BoundedBuffer(){
  		delete [] array;
  	};
	_Nomutex int query() { return count; }

	void insert(string elem) {
		if (count == Largo_buffer) empty.wait();
			array[back] = elem;
			back = (back+1)% Largo_buffer;
			count += 1;
			full.signal();
	}

	string remove() {
		if (count == 0) full.wait();
			string elem = array[front];
			front = (front+1)%Largo_buffer;
			count -= 1;
			empty.signal();
			return elem;
	}
};

_Task Producer{
	BoundedBuffer &Buffer;
	char *input;

public:
	Producer(BoundedBuffer &buffer, char *input) : Buffer(buffer), input(input) {}

private:
	void main(){
		string line;
		ifstream myfile(input);
		if (myfile){
			while (getline(myfile,line)){
				if (line != ""){
					Buffer.insert(line);
				}
			}
		}
		myfile.close();
	}
};

_Task Recognizer{
	BoundedBuffer &Buffer; 
	BoundedBuffer &Buffer2; 

public:
	Recognizer( BoundedBuffer &buffer , BoundedBuffer &buffer2 ) : Buffer( buffer ), Buffer2( buffer2 ) {}

private:
	void main(){
		string item,line;
		while(true){
			item = Buffer.remove();
			if (item == "-1") break;
			line = recognizer(item);
			Buffer2.insert(line);
		}
	}
};


_Task Writer{
	BoundedBuffer &Buffer2;
	char* output;

public:
	Writer(BoundedBuffer &buffer2, char* output) : Buffer2(buffer2), output(output) {}

private:
	void main(){
		string line;
		ofstream myfileOut(output, ios::out);

		if (myfileOut.is_open()){
			while(true){
				line = Buffer2.remove();
				if (line == "-1") break;
				myfileOut << line <<endl;
			}
		}
		myfileOut.close();
	}
};

void uMain::main(){

	int c, Largo_buffer, Largo_buffer2;
	char inFile[100];
	char outFile[100];
	int NoOfRec = 0;
	const int NoOfProds = 1, NoOfWriter = 1;

	static struct option long_options[] = {
		{"Input",1,NULL,'i'},
		{"Output",1,NULL,'o'},
		{"Numero_de_tareas",1,NULL,'h'},
		{"Largo_buffer",1,NULL,'L'},
		{"Largo_buffer2",1,NULL,'l'},
		{0,0,0,0}
	};

	while(1){
		c = getopt_long(argc, argv, "i:o:h:L:l:",long_options,NULL);

		if(c==-1){
			break;
		}

		switch(c){
			case 'i':
				strcpy(inFile,optarg);
				break;
			case 'o':
				strcpy(outFile,optarg);
				break;
			case 'h':
				NoOfRec = atoi(optarg);
				break;
			case 'L':
				Largo_buffer = atoi(optarg);
				break;
			case 'l':
				Largo_buffer2 = atoi(optarg);
				break;

			default:
				
				break;
		}
	}

	BoundedBuffer buffer( Largo_buffer );
	BoundedBuffer buffer2( Largo_buffer2 );
	
	Producer *prod[NoOfProds];
	Recognizer *rec[NoOfRec];
	Writer *wri[NoOfWriter];

	wri[FIRST_ELEMENT] = new Writer(buffer2, outFile);
	
	for (int i = 0; i < NoOfRec; i += 1){
		rec[i] = new Recognizer(buffer, buffer2);
	}
	prod[FIRST_ELEMENT] = new Producer(buffer, inFile);
	delete prod[FIRST_ELEMENT];

	for (int i = 0; i < NoOfRec; i += 1){
		buffer.insert("-1");
	}

	for (int i = 0; i < NoOfRec; i += 1){
		delete rec[i];
	}
	buffer2.insert("-1");
	delete wri[FIRST_ELEMENT];
	
	return ;
}

string recognizer(string linea){

	int estado_actual = 0 ;
	bool estado(true);

	while (estado){
		for (int i = 0; i < linea.size(); i++){
			switch(linea[i]){
				case 'A':
					if (estado_actual >= 0 && estado_actual <= 5){
						estado_actual = 1;
					}
					if (estado_actual >= 6 && estado_actual <= 14){
						estado_actual = 7;
					}
					break;

				case 'C':
					if (estado_actual >= 0 && estado_actual <= 4){
						estado_actual = 2;
					}
					if (estado_actual == 5){
						estado_actual = 6;
					}
					if (estado_actual >= 6 && estado_actual <= 14 && estado_actual != 12){
						estado_actual = 8;
					}
					if (estado_actual == 12){
						estado_actual = 13;
					}
					break;

				case 'G':
					if (estado_actual >= 0 && estado_actual <= 5){
						estado_actual = 3;
					}
					if (estado_actual >= 6 && estado_actual <= 14){
						estado_actual = 9;
					}
					break;

				case 'T':
					if (estado_actual >= 0 && estado_actual <= 4 && estado_actual != 3){
						estado_actual = 4;
					}
					if (estado_actual == 3 || estado_actual == 5){
						estado_actual = 5;
					}
					if (estado_actual == 6 || estado_actual == 10){
						estado_actual = 10;
					}
					if (estado_actual == 7 || estado_actual == 8 || estado_actual == 11){
						estado_actual = 11;
					}
					if (estado_actual == 9 || estado_actual == 12){
						estado_actual == 12;
					}
					if (estado_actual == 13 || estado_actual == 14){
						estado_actual = 14;
					}
					break;

				default:
					break;
			}
		}
		if (estado_actual >= 0 && estado_actual <= 5){
			return linea + " no";
		}
		else {
			return linea + " si";
		}
		estado=false;
	}
}
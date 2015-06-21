#include <fstream>
#include <iostream>
#include <uC++.h>
#include <string>
#include <getopt.h>
using namespace std;


_Monitor BoundedBuffer {
	uCondition full, empty;
	int front, back, count;
	string *array;
	int tamanio;

public:
	BoundedBuffer(int s) : front(0), back(0), count(0), tamanio(s), array(new string[tamanio]) {}
	_Nomutex int query() { return count; }

	void insert(string elem) {
		if (count == tamanio) empty.wait();
			array[back] = elem;
			back = (back+1)% tamanio;
			count += 1;
			full.signal();
	}

	string remove() {
		if (count == 0) full.wait();
			string elem = array[front];
			front = (front+1)%tamanio;
			count -= 1;
			empty.signal();
			return elem;
	}
};

_Monitor BoundedBuffer2 {
	uCondition full, empty;
	int front, back, count;
	string *array;
	int tamanio;

public:
	BoundedBuffer2(int s) : front(0), back(0), count(0), tamanio(s), array(new string[tamanio])  {}
	_Nomutex int query() { return count; }

	void insert(string elem) {
		if (count == tamanio) empty.wait();
			array[back] = elem;
			back = (back+1)% tamanio;
			count += 1;
			full.signal();
	}

	string remove() {
		if (count == 0) full.wait();
			string elem = array[front];
			front = (front+1)%tamanio;
			count -= 1;
			empty.signal();
			return elem;
	}
};


_Task Producer{
	BoundedBuffer &Buffer;
	const char* input = NULL;

public:
	Producer(BoundedBuffer &buf, const char* in) : Buffer(buf), input(in) {}

private:
	void main(){
		ifstream myfile;
		myfile.open(input, ios::in);
		string line;
		while(getline(myfile,line)){
			Buffer.insert( line );
		}
		myfile.close();
	}
};

_Task Recognizer{
	BoundedBuffer &Buffer; 
	BoundedBuffer2 &Buffer2; 

public:
	recognizer( BoundedBuffer &buf , BoundedBuffer2 &buf2 ) : Buffer( buf ) , Buffer2( buf2 ) {}

private:
	void main(){
		string item;
		string line
		for(;;){
			item = Buffer.remove();
			if (item == "T") break;
			line = reconocedor(item);
			Buffer2.insert(line);
		}
	}
};


Task Writer{
	BoundedBuffer2 &Buffer2;
	const char* output = NULL;

public:
	Writer(BoundedBuffer2 &buf2, const char* out) : Buffer2(buf2), output(out) {}

private:
	void main(){
		string line;
		ofstream myfileOut(output, ios::out);

		if (myfileOut.is_open()){
			for(;;){
				line = Buffer2.remove();
				if ( line == "T") break;
			}
		}
		myfileOut.close();
	}
};

void uMain::main(){

	int c, Largo_buffer = 0, Largo_buffer2;
	const char* inFile = NULL;
	const char* outFile = NULL;
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
				inFile = optarg;
				break;
			case 'o':
				outFile = optarg;
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

	
	BoundedBuffer buf( Largo_buffer );
	BoundedBuffer2 buf2( Largo_buffer2 );
	
	Producer *prod[NoOfProds];
	Recognizer *rec[NoOfRec];
	Writer *wri[NoOfWriter];

	for (int i = 0; i < NoOfWriter; i++){
		wri[i] = new Writer(buf2, outFile);
	}

	for (int i = 0; i < NoOfRec; i++){
		rec[i] = new Recognizer(buf, buf2);
	}

	for (int i = 0; i < NoOfProds; i++){
		prod[i] = new Producer(buf, inFile);
	}

	for (int i = 0; i < NoOfProds; i++){
		delete prod[i];
	}

	for (int i = 0; i < NoOfRec; i++){
		buf.insert("T");
	}

	for (int i = 0; i < NoOfRec; i++){
		delete rec[i];
	}

	for (int i = 0; i < NoOfWriter; i++){
		buf2.insert("T");
	}

	for (int i = 0; i < NoOfWriter; i++){
		delete wri[i];
	}

  	return ;
}



void reconocedor(string linea){

	int estado_actual = 0 ;
	bool estado(true);

	while (estado){
		for (int i = 0; i < linea.size(); i++){

			cout << linea[i];
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
			cout << " no"<< endl;
		}
		else {
			cout << " si"<< endl;
		}
		estado=false;
	}

}

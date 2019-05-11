#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include "CacheMemory.h"
#include "VictimCache.h"
#include "CpuSim.h"


using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::stringstream;



int main(int argc, char *argv[]) {

	if (argc < 21) {
		cerr << "Not enough arguments" << endl;
		return 0;
	}

	// File
	// Assuming it is the first argument
	char * fileString = argv[1];
	ifstream file(fileString); //input file stream
	string line;
	if (!file || !file.good()) {
		// File doesn't exist or some other error
		cerr << "File not found" << endl;
		return 0;
	}

	unsigned int 	MemCyc = 0, BSize = 0, L1Size = 0, L2Size = 0,L1Assoc = 0,L2Assoc = 0,
			L1Cyc = 0,L2Cyc = 0,WrAlloc = 0,VicCache = 0;

	for (int i = 2; i < 21; i += 2) {
		string s(argv[i]);
		if (s == "--mem-cyc") {
			MemCyc = atoi(argv[i + 1]);
		} else if (s == "--bsize") {
			BSize = atoi(argv[i + 1]);
		} else if (s == "--l1-size") {
			L1Size = atoi(argv[i + 1]);
		} else if (s == "--l2-size") {
			L2Size = atoi(argv[i + 1]);
		} else if (s == "--l1-cyc") {
			L1Cyc = atoi(argv[i + 1]);
		} else if (s == "--l2-cyc") {
			L2Cyc = atoi(argv[i + 1]);
		} else if (s == "--l1-assoc") {
			L1Assoc = atoi(argv[i + 1]);
		} else if (s == "--l2-assoc") {
			L2Assoc = atoi(argv[i + 1]);
		} else if (s == "--wr-alloc") {
			WrAlloc = atoi(argv[i + 1]);
		} else if (s == "--vic-cache") {
			VicCache = atoi(argv[i + 1]);
		} else {
			cerr << "Error in arguments" << endl;
			return 0;
		}
	}

	CpuSim *cpu = new CpuSim(BSize,  L1Size, L1Assoc, BSize, L2Size, L2Assoc, WrAlloc, VicCache);
	double numOfMemAccess[MAX_MEMORY_LEVELS] = {0} , numOfMisses[2] = {0} ;
	unsigned int latenciesArray[MAX_MEMORY_LEVELS] = {L1Cyc , L2Cyc , VicCache ,MemCyc};
	double totalNumOfMemAccess = 0 , totalLatency = 0;

	while (getline(file, line))
	{
		totalNumOfMemAccess++;
		stringstream ss(line);
		string address;
		char operation = 0; // read (R) or write (W)
		if (!(ss >> operation >> address)) {
			// Operation appears in an Invalid format
			cout << "Command Format error" << endl;
			return 0;
		}

		// DEBUG - remove this line
		//cout << "operation: " << operation;

		string cutAddress = address.substr(2); // Removing the "0x" part of the address

		// DEBUG - remove this line
		//cout << ", address (hex)" << cutAddress;

		unsigned long int num = 0;
		num = strtoul(cutAddress.c_str(), NULL, 16);

		// DEBUG - remove this line
		//cout << " (dec) " << num ;
		//cout << " (binary) " << std::bitset<32>(num) << endl;
		if(operation == 'r') cpu->read(num);
		else if(operation == 'w') cpu->write(num);
		cpu->getAccessAmount(numOfMemAccess, numOfMisses);
		cpu->resetAccessAmount();
	}

	for(int i = 0 ; i < MAX_MEMORY_LEVELS ; i++){
		totalLatency += latenciesArray[i]*numOfMemAccess[i];
	}

	double L1MissRate =numOfMisses[0] / totalNumOfMemAccess , L2MissRate = numOfMisses[1] / numOfMisses[0] , avgAccTime = totalLatency / totalNumOfMemAccess;
			avgAccTime = totalLatency / totalNumOfMemAccess;
	//cout << "L1 misses: " << numOfMisses[0] << endl << "L2 misses: " << numOfMisses[1] << endl << "rate is: ";
	printf("L1miss=%.03f ", L1MissRate );
	printf("L2miss=%.03f ", L2MissRate);
	printf("AccTimeAvg=%.03f\n", avgAccTime);

	delete cpu;
	return 0;
}



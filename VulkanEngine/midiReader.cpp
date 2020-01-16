#include <fstream>
#include <vector>
#include <iostream>

using namespace std;

void readFile(string fileName) {

	vector<char*> chunks;

	char* type;
	char* length;

	

	// Open file
	ifstream file(fileName, ios::in | ios::binary);


	// Read header
	file.read(type, 4);
	file.read(length, 4);

	int format; // 0, 1, 2
	file.read((char*)format, 2);
	cout << format;
	cout << "\n";

	int tracks;
	file.read((char*)tracks, 2);
	cout << tracks;
	cout << "\n";

	int division;
	file.read((char*)division, 2);
	cout << division;
	cout << "\n";


	//int length = 1;
	//for (size_t i = 0; i < length; i++)
	//{

	//	file.read(type, 4);
	//	file.read(length, 4);
	//	
	//}
	
	// Get size
	/*file.read((char*)&m, sizeof(m));
	file.read((char*)&n, sizeof(n));*/

	

	// Fill matrix
	/*for (size_t i = 0; i < m * n; i++) {
		int val;
		file.read((char*)&val, sizeof(m));
		mat.set(i, val);
	}*/
};

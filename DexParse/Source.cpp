#include <cstdio>
#include <iostream>
#include <fstream>
#include "DexParser.h"
using namespace std;
using namespace myDexSpace;

int main(int argc, char* argv[])
{
	cout << " ###    ####  ## ##" << endl;
	cout << " #  #   #      # # " << endl;
	cout << " #  ##  ##      #  " << endl;
	cout << " #  #   #      # # " << endl;
	cout << " ###    ####  ## ##" << endl;
	cout << '\n';
	cout << " ####    ####   #####    ####  #####" << endl;
	cout << " #   #  #    #  #    #  #      #    " << endl;
	cout << " #   #  #    #  #    #   #     ###  " << endl;
	cout << " ###    ######  #####     #    #    " << endl;
	cout << " #      #    #  #  #       #   #    " << endl;
	cout << " #      #    #  #   #  ####    #####" << endl << endl;


	// dexparcer file_name [parameters]
	// parameters example: hfc
	//				a - print all info
	//				h - print header info in console
	//				t - print types
	//				p - print prototypes
	//				f - fields
	//				m - methods
	//				c - classes

	if (argc != 3)
	{
		cout << "Please type all parameters!\n";
		cout << "> file_name [parameters]\n";
		return EXIT_SUCCESS;
	}

	DexParser parser(argv[1]);
	if (!parser.isFileOpened())
	{
		cout << "File \'" << argv[1] << "\' was not found\n";
		return EXIT_SUCCESS;
	}

	parser.readHeaderInfo();

	string parameters = argv[2];
	for (char p : parameters)
	{
		if (p == 'h' || p == 'a')
			parser.printHeaderInfo();
		if (p == 't' || p == 'a')
			parser.printTypes();
		if (p == 'p' || p == 'a')
			parser.printPrototypes();
		if (p == 'f' || p == 'a')
			parser.printFields();
		if (p == 'm' || p == 'a')
			parser.printMethods();
		if (p == 'c' || p == 'a')
			parser.printClasses();
		if (p == 'a') break;
	}

	return EXIT_SUCCESS;

}
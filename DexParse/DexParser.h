#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <map>
using namespace std;

namespace myDexSpace
{
#define TYPES_OUTPUT_FILE "types.txt"
#define PROTOTYPES_OUTPUT_FILE "prototypes.txt"
#define FIELDS_OUTPUT_FILE "fields.txt"
#define METHODS_OUTPUT_FILE "methods.txt"
#define CLASSES_OUTPUT_FILE "classes.txt"

	typedef unsigned char ubyte;
	typedef unsigned int uint;

	class DexParser
	{
	private:
		string fileName; // dex file path
		ifstream openedFile;

		struct DexFileHeader {
			ubyte magic[8];
			int checksum;
			ubyte signature[20];
			uint filesize;
			uint header_size;
			uint endian_tag;
			uint link_size;
			uint link_off;
			uint map_off;
			uint string_ids_size;
			uint string_ids_off;
			uint type_ids_size;
			uint type_ids_off;
			uint proto_ids_size;
			uint proto_ids_off;
			uint field_ids_size;
			uint field_ids_off;
			uint method_ids_size;
			uint method_ids_off;
			uint class_defs_size;
			uint class_defs_off;
			uint data_size;
			uint data_off;
		};
		DexFileHeader headerInfo;

		struct proto_id {
			string shorty_idx; // short description
			string return_type_idx;
			uint parameters_off;
		};

		struct field_id {
			string class_name; // field belong tp class class_name
			string type; // type name
			string name;
		};

		struct method_id {
			string class_name;
			proto_id prototype;
			string name;
		};

		// old info?
		map<uint, string> access_flags_class = {
			{0x1, "public"},
			{0x2, "private"},
			{0x4, "protected"},
			{0x8, "static"},
			{0x10, "final"},
			{0x200, "interface"},
			{0x400, "abstract"}
		};



		uint readFourBytes(int offset); // reading 4 bytes (little endian)

		// INTERNAL METHODS
		string readString(uint id);
		string readType(uint id);
		proto_id readPrototype(uint id);
		field_id readField(uint id);
		method_id readMethod(uint id);

	public:
		DexParser(string fileName): fileName(fileName)
		{
			openedFile.open(fileName, ios::binary);
			if (!openedFile.is_open())
				cout << "Incorrect file path\n";
			/*else cout << "File " << fileName << " was opened\n";*/
		}

		~DexParser()
		{
			if (openedFile.is_open())
			{
				openedFile.close();
				/*cout << "File " << fileName << " was closed\n";*/
			}
		}

		bool isFileOpened()
		{
			return openedFile.is_open();
		}

		void readHeaderInfo();
		void printHeaderInfo();

		void printString(uint);
		void printTypes(); // print all types
		void printPrototypes();
		void printFields();
		void printMethods();
		void printClasses();
	};
}


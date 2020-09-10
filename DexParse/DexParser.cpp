#include "DexParser.h"

namespace myDexSpace
{
	uint convert_to_big_endian(char* i)
	{
		return (
				(((uint)i[3] & 0xFF) << 24) |
				(((uint)i[2] & 0xFF) << 16) |
				(((uint)i[1] & 0xFF) << 8)  |
				((uint)i[0] & 0xFF)
			);
	}

	uint DexParser::readFourBytes(int offset)
	{
		// offset in DECIMAL
		char tmp[4] = { 0 };
		openedFile.seekg(offset, openedFile.beg);
		openedFile.read(tmp, 4);
		return convert_to_big_endian(tmp);
	}

	void DexParser::readHeaderInfo()
	{
		headerInfo.header_size = 112; // this is always 0x70 (112 bytes)
		headerInfo.string_ids_off = 112; // go after header block

		// READ FILE_SIZE FROM HEADER 
		headerInfo.filesize = readFourBytes(32); // set carriage to the file_size position

		// READ NUMBER OF STRINGS
		headerInfo.string_ids_size = readFourBytes(56);

		// READ TYPES INFO
		headerInfo.type_ids_size = readFourBytes(64);
		headerInfo.type_ids_off = readFourBytes(68);
		
		// READ PROTOTYPES INFO
		headerInfo.proto_ids_size = readFourBytes(72);
		headerInfo.proto_ids_off = readFourBytes(76);

		// FIELDS
		headerInfo.field_ids_size = readFourBytes(80);
		headerInfo.field_ids_off = readFourBytes(84);

		// METHODS
		headerInfo.method_ids_size = readFourBytes(88);
		headerInfo.method_ids_off = readFourBytes(92);

		// CLASS DEFINITIONS
		headerInfo.class_defs_size = readFourBytes(96);
		headerInfo.class_defs_off = readFourBytes(100);

		// DATA
		headerInfo.data_size = readFourBytes(104);
		headerInfo.data_off = readFourBytes(108);
	}

	void DexParser::printHeaderInfo()
	{
		cout << "+-------------------------\n";
		cout << "|      Header Info\n";
		cout << "+-------------------------\n";
		cout << "| Header size        | " << headerInfo.header_size << '\n';
		cout << "| All file size      | " << headerInfo.filesize << '\n';
		cout << "| Num. of strings    | " << headerInfo.string_ids_size << '\n';
		cout << "| Num. of types      | " << headerInfo.type_ids_size << '\n';
		cout << "| Types offset       | " << headerInfo.type_ids_off << '\n';
		cout << "| Num. of prototypes | " << headerInfo.proto_ids_size << '\n';
		cout << "| Prototypes offset  | " << headerInfo.proto_ids_off << '\n';
		cout << "| Num. of fields     | " << headerInfo.field_ids_size << '\n';
		cout << "| Fields offset      | " << headerInfo.field_ids_off<< '\n';
		cout << "| Num. of methods    | " << headerInfo.method_ids_size << '\n';
		cout << "| Methods offset     | " << headerInfo.method_ids_off << '\n';
		cout << "| Num. of classes    | " << headerInfo.class_defs_size << '\n';
		cout << "| Classes offset     | " << headerInfo.class_defs_off << '\n';
		cout << "| Data size          | " << headerInfo.data_size << '\n';
		cout << "| Data offset        | " << headerInfo.data_off << '\n';
		cout << "+-------------------------\n";
	}

	// STRINGS
	string DexParser::readString(uint id)
	{
		if (id > headerInfo.string_ids_size) return "";

		uint pointer_to_address = headerInfo.string_ids_off + (id * 4);
		pointer_to_address = readFourBytes(pointer_to_address); // here we have offset to the string first byte

		string res = "";

		openedFile.seekg(pointer_to_address, openedFile.beg);
		while (char tmp = openedFile.get())
		{
			if (tmp == '\0') break;
			res += tmp;
		}

		return res;
	}


	void DexParser::printString(uint id)
	{
		cout << "String " << id << ": \'" << readString(id) << "\'\n";
	}


	// TYPES
	string DexParser::readType(uint id)
	{
		if (id > headerInfo.type_ids_size) return "";
		uint p = headerInfo.type_ids_off + (id * 4);
		p = readFourBytes(p); // here string id, that contain type name
		return readString(p);
	}


	void DexParser::printTypes()
	{
		ofstream out(TYPES_OUTPUT_FILE, ios::out);
		for (int i = 0; i < headerInfo.type_ids_size; i++)
			out << "Type " << i << ": \'" << readType(i) << "\'\n";
		out.close();
		cout << "Types list saved to " << TYPES_OUTPUT_FILE << endl;
	}


	// PROTOTYPES
	DexParser::proto_id DexParser::readPrototype(uint id)
	{
		proto_id res;
		uint pStart = headerInfo.proto_ids_off + (id * 12); // 3 * 4
		uint str_id = readFourBytes(pStart);
		res.shorty_idx = readString(str_id);

		str_id = readFourBytes(pStart + 4);
		res.return_type_idx = readType(str_id);

		res.parameters_off = readFourBytes(pStart + 8);

		return res;
	}

	void DexParser::printPrototypes()
	{
		ofstream out(PROTOTYPES_OUTPUT_FILE, ios::out);
		proto_id res;
		for (int i = 0; i < headerInfo.proto_ids_size; i++)
		{
			res = readPrototype(i);
			out << "+--------------------\n";
			out << "| Type ID: " << i << endl;
			out << "| Description: " << res.shorty_idx << endl;
			out << "| Return type: " << res.return_type_idx << endl;
			out << "| Parameters address: 0x" << hex << res.parameters_off << dec << endl;
			out << "| General view : " << res.return_type_idx << ' ' << res.shorty_idx << " ( " << res.parameters_off << " )\n";
		}
		out.close();
		cout << "Prototypes list saved to " << PROTOTYPES_OUTPUT_FILE << endl;
	}


	// FIELDS
	DexParser::field_id DexParser::readField(uint id)
	{
		uint pStart = headerInfo.field_ids_off + (id * 8);
		uint class_type = readFourBytes(pStart);

		field_id res;
		res.class_name = readType((class_type & 0xFFFF));
		res.type = readType((class_type >> 16));

		uint pName = readFourBytes(pStart + 4);
		res.name = readString(pName);

		return res;
	}

	void DexParser::printFields()
	{
		ofstream out(FIELDS_OUTPUT_FILE, ios::out);
		
		for (int i = 0; i < headerInfo.field_ids_size; i++)
		{
			field_id res = readField(i);
			out << "+-------------------------\n";
			out << "| Belong to class: " << res.class_name << endl;
			out << "| Type: " << res.type << endl;
			out << "| Name: " << res.name << endl;
		}
		out.close();

		cout << "Fields list saved to " << FIELDS_OUTPUT_FILE << endl;
	}


	// METHODS
	DexParser::method_id DexParser::readMethod(uint id)
	{
		method_id res;

		uint pStart = headerInfo.method_ids_off + (id * 8);
		uint class_proto = readFourBytes(pStart);

		res.class_name = readType((class_proto & 0xFFFF));
		res.prototype = readPrototype((class_proto >> 16));

		uint pName = readFourBytes(pStart + 4);
		res.name = readString(pName);

		return res;
	}

	void DexParser::printMethods()
	{
		ofstream out(METHODS_OUTPUT_FILE, ios::out);
		
		for (int i = 0; i < headerInfo.method_ids_size; i++)
		{
			method_id res = readMethod(i);
			out << "+-------------------------\n";
			out << "| Belong to class: " << res.class_name << endl;
			out << "| Name: " << res.name << endl;
			out << "|+-----------------------\n";
			out << "|| Prototype\n";
			out << "|+-----------------------\n";
			out << "|| Description: " << res.prototype.shorty_idx << endl;
			out << "|| Return Type: " << res.prototype.return_type_idx << endl;
			out << "|+-----------------------\n";
		}
		out.close();

		cout << "Methods list saved to " << METHODS_OUTPUT_FILE << endl;
	}

	void DexParser::printClasses()
	{
		ofstream out(CLASSES_OUTPUT_FILE, ios::out);
		uint pStart = headerInfo.class_defs_off;

		// one class: 64 bytes
		for (int i = 0; i < headerInfo.class_defs_size; i++, pStart += 64)
		{
			uint p = pStart;
			uint tmp = readFourBytes(p); // read class name (type id)
			out << "+---- Class: " << readType(tmp) << endl;

			p += 4;
			tmp = readFourBytes(p); // read access flag
			out << "| Access: " << hex << tmp << dec << endl;

			p += 4;
			tmp = readFourBytes(p); // read superclass
			out << "| Superclass: " << readType(tmp) << endl;

			p += 4; // read interface
			/* TODO: implement interface */

			p += 4;
			tmp = readFourBytes(p);
			out << "| Source file" << readString(tmp) << endl;
		}
		out.close();

		cout << "Classes list saved to " << CLASSES_OUTPUT_FILE << endl;
	}

}
#pragma GCC diagnostic push
#include <StdDiagnosticIgnore.hpp>

#include <Windows.h>
#include <stdio.h>
#include <io.h>
#include <utility>
#include <vector>
#include <fstream>

#pragma GCC diagnostic pop

class mystr {
	public:
	char * ptr;
	int count;
	mystr() {
		ptr = new char[2048];
		ptr[0] = 0;
		count = 0;
	}
	mystr(mystr & obj) {
		ptr = new char[2048];
		char* ptr2 = obj.ptr;
		while ((*ptr++ = *ptr2++) != 0);
		count = obj.count;
	}
	mystr(const char * str) {
		ptr = new char[2048];
		count = 0;
		char * bptr = ptr;
		while ((*bptr++ = *str++) != 0) count++;
	}
	void EndAdd(const char * What) {
		char * Where = ptr + count;
		while ((*Where++ = *What++) != 0) count++;
	}
	void EndDelete(int i) {
		count -= i;
		if (count<0) count = 0;
		ptr[count] = 0;
	}
	~mystr() {
		delete[] ptr;
	}
	char & LastChar() {
		return ptr[count-1];
	}
};

int PosCharEnd(const char * where, char what, int before = -1) {
	int pos = -1;
	const char * where_ptr = where;			
	if (before != -1) {
		int count = 0;
		while (*where_ptr != 0) {				
			if (count >= before) break;		
			if (*where_ptr == what) pos = where_ptr - where;
			where_ptr++;
			count++;
		}
	}
	else {
		while (*where_ptr != 0) {				
			if (*where_ptr == what) pos = where_ptr - where;
			where_ptr++;
		}
	}
	
	return pos;
}

WORD invertWORD(WORD word) {
	return (word % 16)*16 + (word / 16);
}

std::string GetCurrentDirectory() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	
	return std::string(buffer).substr(0, pos);
}

int main(int argc, char * argv[]) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	
	WORD textAtr = csbi.wAttributes;
	
	SetConsoleTextAttribute(hConsole, invertWORD(textAtr));
	
	//if (textAtr != 0x70) SetConsoleTextAttribute(hConsole, 0x70);
	//else SetConsoleTextAttribute(hConsole, 0x07);
	
	mystr color;
	for (int i = 0; i < columns-1; i++) color.EndAdd("~");
	printf("%s\n",color.ptr);
	
	SetConsoleTextAttribute(hConsole, textAtr);
	
	using namespace std;
	
	vector<pair<string, string>> pairs;
	
	{
		ifstream in_file;
		printf("%s\n", (GetCurrentDirectory() + "\\build.global.config").c_str());
		in_file.open(GetCurrentDirectory() + "\\build.global.config", std::ifstream::in);
		
		if (!in_file.is_open()) {
			printf("Error on opening build.global.config file\n");
			exit(0);
		}
		
		for (std::string line; getline(in_file, line); ) {
			
			size_t pos = line.find(' ');
			string left = line.substr(0, pos);
			string right = line.substr(pos + 1);
			
			pairs.push_back(pair(left, right));
			
			//printf("Find %s - %s\n", left.c_str(), right.c_str());
		}
		
		in_file.close();
	}
	
	
	if (argc >= 2) {
		mystr command;
		if ((argc==2) && strcmp(argv[1],"-source") == 0) {
			command.EndAdd("npp D:\\_MPS\\_CPP\\__Projects\\SimpleCppBuild\\Build2_bc.cpp");
			SetConsoleOutputCP(1251);
			printf("%s\n",command.ptr);
			system(command.ptr);
			return 0;
		}
		
		command.EndAdd("gcc ");
		mystr argv1(argv[1]);
		FILE * f = fopen(argv1.ptr, "r");
		if (f == nullptr) {
			printf("No file %s | try make ",argv1.ptr);
			int cnt = 0;
			if (argv1.LastChar() == '.') {argv1.EndAdd("cpp"); cnt += 3;}
			else {argv1.EndAdd(".cpp"); cnt += 4;}
			printf("%s\n",argv1.ptr);
			if ((f = fopen(argv1.ptr, "r")) == nullptr) {
				printf("No file %s | return to ",argv1.ptr);
				argv1.EndDelete(cnt);
				printf("%s\n",argv1.ptr);
			}
			else {
				fclose(f);
			}
		}
		else {
			fclose(f);
		}
		command.EndAdd(argv1.ptr);
		command.EndAdd(" -o ");
		
		command.EndAdd(argv1.ptr);
		int pos = PosCharEnd(command.ptr, '.');
		if (pos != -1) command.EndDelete(command.count - pos);
		for (int i = 2; i < argc; i++) {
			command.EndAdd(" ");
			
			bool find = false;
			
			for (pair<string, string> & p : pairs) {
				if (strcmp(argv[i], p.first.c_str()) == 0) {
					command.EndAdd(p.second.c_str());
					find = true;
					break;
				}
			}
			
			if (!find) command.EndAdd(argv[i]);
		}
		printf("%s\n", command.ptr);
		system(command.ptr);
	}
	else if (argc == 1) {
		printf("C/CPP fast compiler and builder\n");
		printf("Use: BuildS [filename] [additional_flags]\n");
		printf("Use \"BuildS -source\" to open source file\n");
		printf("Defined build flags:\n");
		for (pair<string, string> & p : pairs)
			printf("[%s] => %s\n", p.first.c_str(), p.second.c_str());
	}
	else {
		printf("Use: build [filename] [additional_flags]\n");
	}
	return 0;
}
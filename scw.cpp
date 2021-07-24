#include <cstddef>
#include <string>

#include <Windows.h>
#include <stdio.h>
#include <io.h>
#include <utility>
#include <vector>
#include <fstream>

int PosCharEnd(const char * where, char what) {
	int pos = -1;
	const char * where_ptr = where;			
	while (*where_ptr != 0) {				
		if (*where_ptr == what) pos = where_ptr - where;
		where_ptr++;
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
	
	std::string color(columns - 1, '~');
	printf("%s", color.c_str());
	
	SetConsoleTextAttribute(hConsole, textAtr);
	printf("\n");

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
		std::string command;
		
		command.append("gcc ");
		std::string argv1(argv[1]);
		FILE * f = fopen(argv1.c_str(), "r");

		if (f == nullptr) {
			printf("No file %s | try make ", argv1.c_str());
			
			int cnt = 0;
			if (argv1.back() == '.') {
				argv1.append("cpp"); 
				cnt += 3;
			} else {
				argv1.append(".cpp");
				cnt += 4;
			}
			printf("%s\n", argv1.c_str());

			if ((f = fopen(argv1.c_str(), "r")) == nullptr) {
				printf("No file %s | return to ", argv1.c_str());
				argv1.erase(argv1.length() - cnt, cnt);
				printf("%s\n", argv1.c_str());
			}
			else {
				fclose(f);
			}
		}
		else {
			fclose(f);
		}
		command.append(argv1);
		command.append(" -o ");
		
		command.append(argv1);

		int pos = PosCharEnd(command.c_str(), '.');
		if (pos != -1) command.erase(pos, command.length() - pos);

		for (int i = 2; i < argc; i++) {
			command.append(" ");
			
			bool find = false;
			
			for (pair<string, string> & p : pairs) {
				if (strcmp(argv[i], p.first.c_str()) == 0) {
					command.append(p.second);
					find = true;
					break;
				}
			}
			
			if (!find) command.append(argv[i]);
		}
		printf("%s\n", command.c_str());
		system(command.c_str());
	}
	else if (argc == 1) {
		printf("CPP Compiler Wrapper\n");
		printf("Use: .\\scw [filename] [additional_flags]\n");
		printf("Defined build flags:\n");
		for (pair<string, string> & p : pairs)
			printf("[%s] => %s\n", p.first.c_str(), p.second.c_str());
	}
	else {
		printf("Use: build [filename] [additional_flags]\n");
	}
	return 0;
}
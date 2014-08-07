#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>

#define TYPE_INT 0
#define TYPE_FLOAT 1
#define TYPE_STRING 2

class info {
public:
	size_t address;
	int type;
	std::string name;

	info(void* ca, int ct, std::string cname) {
		address = (size_t)ca;
		type = ct;
		name = cname;
	}
};

std::vector<info> infos;

std::vector<std::string> split(std::string, char);
bool isRaw(std::string);
void* getRaw(std::string);
info* getRawInfo(std::string);
void receive(std::string);

std::vector<std::string> split(std::string str, char delim) {
	std::vector<std::string> res;
	std::stringstream ss(str);
	std::string item;
	while (std::getline(ss, item, delim)) {
		if (!item.empty()) {
			res.push_back(item);
		}
	}

	return res;
}

bool isRaw(std::string from) {
	return (from[0] == '$') || (from[0] == '*') || (from[0] == '@');
}

void* getRaw(std::string from) {
	void* res = nullptr;

	if (from[0] == '$') { // $ means the name
		std::string nname = from.substr(1, from.size());
		std::string ename = nname;
		if (isRaw(nname) && getRawInfo(nname) != nullptr) {
			ename = *(std::string*)(getRawInfo(nname)->address);
		}

		int ci = -1;
		for (int i = 0; i < infos.size(); i++) {
			if (infos[i].name == ename) {
				ci = i;
				break;
			}
		}
		if (ci != -1) {
			res = (void*)infos[ci].address;
		}
	}
	else if (from[0] == '*') { // * means the address
		std::string nname = from.substr(1, from.size());
		size_t eadd;
		if (isRaw(nname) && getRawInfo(nname) != nullptr) {
			eadd = *(size_t*)getRawInfo(nname)->address;
		}
		else {
			eadd = (size_t)std::stoll(nname);
		}

		int ci = -1;
		for (int i = 0; i < infos.size(); i++) {
			if (infos[i].address == eadd) {
				ci = i;
				break;
			}
		}
		if (ci != -1) {
			res = (void*)infos[ci].address;
		}
	}
	else if (from[0] == '@') { // @ means address without safe-check
		std::string nname = from.substr(1, from.size());
		size_t eadd;
		if (isRaw(nname) && getRawInfo(nname) != nullptr) {
			eadd = *(size_t*)getRawInfo(nname)->address;
		}
		else {
			eadd = (size_t)std::stoll(nname);
		}

		res = (void*)eadd;
	}

	return res;
}

info* getRawInfo(std::string from) {
	info* res = nullptr;

	if (from[0] == '$') { // $ means the name
		std::string nname = from.substr(1, from.size());
		std::string ename = nname;
		if (isRaw(nname) && getRawInfo(nname) != nullptr) {
			ename = *(std::string*)(getRawInfo(nname)->address);
		}

		int ci = -1;
		for (int i = 0; i < infos.size(); i++) {
			if (infos[i].name == ename) {
				ci = i;
				break;
			}
		}
		if (ci != -1) {
			res = &infos[ci];
		}
	}
	else if (from[0] == '*' || from[0] == '@') { // * means the address, @ means address without safe-check. safe-check doesn't matter here
		std::string nname = from.substr(1, from.size());
		size_t eadd;
		if (isRaw(nname) && getRawInfo(nname) != nullptr) {
			eadd = *(size_t*)getRawInfo(nname)->address;
		}
		else {
			eadd = (size_t)std::stoll(nname);
		}

		int ci = -1;
		for (int i = 0; i < infos.size(); i++) {
			if (infos[i].address == eadd) {
				ci = i;
				break;
			}
		}
		if (ci != -1) {
			res = &infos[ci];
		}
	}

	return res;
}

void receive(std::string input) {
	std::vector<std::string> toks = split(input, ' ');

	if (toks.size() == 1) {
		if (toks[0] == "clearw") { // clear screen Windows
			system("cls");
			std::cout << "\n\n    the H Programming Language.\n\n";
			return;
		}
		else if (toks[0] == "clearu") { // clear screen UNIX
			system("reset");
			std::cout << "\n\n    the H Programming Language.\n\n";
			return;
		}
	}
	else if (toks.size() == 2) {
		if (toks[0] == "print") { // print something
			info* i = getRawInfo(toks[1]);
			void* v = getRaw(toks[1]);

			if (i == nullptr || v == nullptr) {
				std::cout << "    there is no such pointer as indicated by " << toks[1] << '\n';
				return;
			}

			if (i->type == TYPE_INT) {
				std::cout << "    " << std::to_string(*(int*)v) << '\n';
				return;
			}
			else if (i->type == TYPE_FLOAT) {
				std::cout << "    " << std::to_string(*(float*)v) << '\n';
				return;
			}
			else if (i->type == TYPE_STRING) {
				std::cout << "    " << *(std::string*)v << '\n';
				return;
			}
		}
		else if (toks[0] == "printa") { // print Address
			info* i = getRawInfo(toks[1]);
			void* v = getRaw(toks[1]);

			if (i == nullptr || v == nullptr) {
				std::cout << "    there is no such pointer indicated by " << toks[1] << '\n';
				return;
			}

			std::cout << "    the address of " << toks[1] << " is " << std::to_string((size_t)v) << " of type ";
			if (i->type == TYPE_INT) {
				std::cout << "TYPE_INT: integer\n";
				return;
			}
			else if (i->type == TYPE_FLOAT) {
				std::cout << "TYPE_FLOAT: float\n";
				return;
			}
			else if (i->type == TYPE_STRING) {
				std::cout << "TYPE_STRING: string\n";
				return;
			}
		}
	}
	else if (toks.size() == 3) {
		if (toks[0] == "fprint") { // Force print something as type
			void* v = (isRaw(toks[2]) ? getRaw(toks[2]) : (void*)(size_t)std::stoll(toks[2]));

			if (toks[1] == "i") {
				std::cout << "    " << std::to_string(*(int*)v) << " (forced into type TYPE_INT: integer)\n";
				return;
			}
			else if (toks[1] == "f") {
				std::cout << "    " << std::to_string(*(float*)v) << " (forced into type TYPE_FLOAT: float)\n";
				return;
			}
			else if (toks[1] == "s") {
				std::cout << "    " << *(std::string*)v << " (forced into type TYPE_STRING: string)\n";
				return;
			}
		}
	}
	else if (toks.size() == 4) {
		if (toks[0] == "alloc") { // allocate new var
			if (toks[1] == "i") { // int
				infos.push_back(info(isRaw(toks[2]) ? (int*)getRaw(toks[2]) : new int(std::stoi(toks[2])), TYPE_INT, toks[3]));
				return;
			}
			else if (toks[1] == "f") { // float
				infos.push_back(info(isRaw(toks[2]) ? (float*)getRaw(toks[2]) : new float(std::stof(toks[2])), TYPE_FLOAT, toks[3]));
				return;
			}
			else if (toks[1] == "s") { // string
				infos.push_back(info(isRaw(toks[2]) ? (std::string*)getRaw(toks[2]) : new std::string(toks[2]), TYPE_STRING, toks[3]));
				return;
			}
		}
	}
}

int main() {
	std::cout << "\n\n    the H Programming Language.\n\n";

	std::string input = "";
	while (input != "exit") {
		std::cout << " >> ";
		std::getline(std::cin, input);
		receive(input);
	}
	return 0;
}
// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unistd.h>

using namespace std;

#include "listmap.h"
#include "util.h"
#include "xpair.h"

using str_str_map = listmap<string, string>;
using str_str_pair = str_str_map::value_type;

void scan_options(int argc, char** argv)
{
    opterr = 0;
    for (;;) {
        int option = getopt(argc, argv, "@:");
        if (option == EOF)
            break;
        switch (option) {
        case '@':
            debugflags::setflags(optarg);
            break;
        default:
            complain() << "-" << char(optopt) << ": invalid option"
                       << endl;
            break;
        }
    }
}

void arbitrate(istream* in, str_str_map& test)
{
    smatch result;
    string current_line;
    regex comment_regex{ R"(^\s*(#.*)\s*)" };
    regex key_regex{ R"(^\s*([^=\s]+)\s*)" };
    regex key_equals_regex{ R"(^\s*([^=\s]+)\s*=\s*)" };
    regex key_value_regex{ R"(^\s*([^=\s]+)\s*=\s*(.+)\s*)" };
    regex equals_regex{ R"(^\s*=\s*)" };
    regex equals_value_regex{ R"(^\s*=\s*([^\s]+)\s*)" };
    str_str_map::iterator itor;

    while (getline(*in, current_line)) {
        if (regex_match(current_line, result, comment_regex)) {
            continue;
        } else if (regex_match(current_line, result, key_regex)) {
            itor = test.find(result[1]);
            if (itor == test.end()) {
                cout << result[1] << ": key not found" << endl;
            } else {
                cout << itor->first << " = " << itor->second << endl;
            }
            continue;
        } else if (regex_match(current_line, result,
          key_equals_regex)) {
            itor = test.find(result[1]);
            if (itor == test.end()) {
                cout << result[1] << ": key not found" << endl;
            } else {
                test.erase(itor);
            }
            continue;
        } else if (regex_match(current_line, result,
          key_value_regex)) {
            str_str_pair key_value_pair(result[1], result[2]);
            test.insert(key_value_pair);
            continue;
        } else if (regex_match(current_line, result, equals_regex)) {
            itor = test.begin();
            while (itor != test.end()) {
                cout << itor->first << " = " << itor->second << endl;
                ++itor;
            }
            continue;
        } else if (regex_match(current_line, result,
          equals_value_regex)) {
            itor = test.begin();
            while (itor != test.end()) {
                if (itor->second == result[1]) {
                    cout << itor->first << " = "
                         << itor->second << endl;
                }
                ++itor;
            }
            continue;
        } else {
            continue;
        }
    }
}

class file_error : public runtime_error {
public:
    explicit file_error(const string& what);
};

int main(int argc, char** argv)
{
    sys_info::execname(argv[0]);
    scan_options(argc, argv);

    str_str_map test;
    istream* in;

    if (argc == 1) {
        cout << "At least one filename argument required." << endl
             << "Enter '-' to read from cin." << endl;
    } else if (strcmp(argv[1], "-") == 0) {
        in = &cin;
        arbitrate(in, test);
    } else {
        fstream infile;

        for (int i = 1; i < argc; ++i) {
            infile.open(argv[i]);
            if (infile.is_open()) {
                in = &infile;
                arbitrate(in, test);
                infile.close();
            } else {
              complain() << "File '" << argv[i] 
                         << "' could not be opened" << endl;
            }
        }
    }
    return EXIT_SUCCESS;
}

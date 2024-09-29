#include "oblivion/ordered_list.hpp"
#include <fstream>
#include <iostream>
#include <istream>
#include <string>

void handle_ops(oblivion::OrderedList &list, std::istream &is,
                std::ostream &os) {
  std::string line;
  while (std::getline(is, line)) {
    std::string op = line.substr(0, 3);

    std::string rest;
    if (line.size() > 4) {
      rest = line.substr(4);
    }

    if (op == "INC") {
      list.include(std::stoi(rest));
    } else if (op == "REM") {
      list.erase(std::stoi(rest));
    } else if (op == "SUC") {
      std::optional<int> suc = list.successor(std::stoi(rest));
      if (suc.has_value()) {
        os << *suc << std::endl;
      }
    } else if (op == "IMP") {
      os << list << std::endl;
    } else if (op == "EXT") {
      break;
    }
  }
}

void print_help() {
  const auto HELP_MSG =
      "oblivion-cli v0.1.0\n"
      "Usage: oblivion-cli [option] [filename]\n\n"
      "Options:\n"
      "  -f\tReceive operations from a file\n"
      "  -i\tReceive operations iteratively\n"
      "  -h\tPrint this help menssage\n\n"
      "Operations:\n"
      "  INC <int>\tAdds the specified integer\n"
      "  REM <int>\tRemoves the specified integer\n"
      "  SUC <int>\tSend to the output the successor of the specified integer\n"
      "  IMP <int>\tSend to the output the contents of the list\n"
      "  EXT <int>\tExit the operation loop\n";
  std::cout << HELP_MSG;
}

int main(int argc, char *argv[]) {
  if (argc < 2 || argc > 3) {
    std::cout << "Invalid number of arguments. See -h for help." << std::endl;
    return 1;
  }

  std::string opt = argv[1];
  if (opt == "-h") {
    if (argc != 2) {
      std::cout << "Invalid argument to -h." << std::endl;
      return 1;
    }
    print_help();
  } else if (opt == "-i") {
    if (argc != 2) {
      std::cout << "Invalid argument to -i. See -h for help." << std::endl;
      return 1;
    }
    oblivion::OrderedList list{};
    handle_ops(list, std::cin, std::cout);
  } else if (opt == "-f") {
    if (argc != 3) {
      std::cout << "Invalid argument to -f. See -h for help." << std::endl;
      return 1;
    }

    std::string filename = argv[2];
    std::ifstream in_file(filename);
    if (!in_file) {
      std::cout << "Failed to open the file '" << filename << "'." << std::endl;
      return 1;
    }

    std::string out_filename = filename + ".out";
    std::ofstream out_file(out_filename);
    if (!out_file) {
      std::cout << "Failed to open the file '" << out_filename << "'."
                << std::endl;
      return 1;
    }
    oblivion::OrderedList list{};
    handle_ops(list, in_file, out_file);
  } else {
    std::cout << "Invalid option '" << opt << "'!" << std::endl;
    return 1;
  }

  return 0;
}

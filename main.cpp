#include "FlickrAPI.h"
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#pragma warning(disable:4996)


std::string replaceSpaces(std::string str) {
    std::string result = "";
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == ' ') {
            result += "+";
        }
        else {
            result += str[i];
        }
    }
    return result;
}


std::vector<std::string> parse_command(const std::string& input) {
    std::istringstream stream(input);
    std::string token;
    std::vector<std::string> tokens;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}


std::string replaceCommas(std::string str) {
    std::string result = "";
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == ',') {
            result += "%2C";
        }
        else {
            result += str[i];
        }
    }
    return result;
}


std::pair<std::string, std::string>parse_config() {
    std::string api_key;
    std::string api_main;
    std::ifstream config_file(R"(.\config.ini)");
    if (config_file.is_open())
    {
        std::string line;

        while (std::getline(config_file, line))
        {
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
			
            if (line.find("API_KEY=") == 0)
            {
                api_key = line.substr(8);
            }

            if (line.find("API_MAIN") == 0)
            {
                api_main = line.substr(9);
                break;
            }
        }

        config_file.close();
    }

    return std::make_pair(api_key, api_main);
}


int main(int argc, char* argv[]) {
  	std::string input;

  	while (true) {
          std::cout << "> ";
          std::getline(std::cin, input);
          auto tokens =  parse_command(input);

          if (tokens.empty()) continue;

          const std::string& command = tokens[0];
          if (command == "upload-photo") {
            std::string request = tokens[1];
            std::string tags = tokens[2];
            int photo_count = std::stoi(tokens[3]);
            std::string path = tokens[4];
            std::pair<std::string, std::string> api = parse_config();

            FlickrAPI obj = FlickrAPI(api.first, api.second, path, replaceSpaces(request), replaceCommas(tags));
            std::cout << "Start uploading photos..." << std::endl;
            obj.uploadPhoto(photo_count);
            std::cout << "Done!" << std::endl;
          }

          else if (command == "exit") {
            break;
          }
    }

	return 0;

}
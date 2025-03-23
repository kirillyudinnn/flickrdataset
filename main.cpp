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


std::string parse_config() {
    std::string api_key;
    
    fs::path config_path = fs::current_path() / "config.ini";
    std::ifstream config_file(config_path);
    if (config_file.is_open())
    {
        std::string line;

        while (std::getline(config_file, line))
        {
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
			
            if (line.find("API_KEY") == 0)
            {
                api_key = line.substr(8);
            }

        }

        config_file.close();
    }

    return api_key;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <request> <tags> <img_count> <save_path>";
        return 1;
    }

    std::string api_key = parse_config();
    std::string request = replaceSpaces(argv[1]);
    std::string tags = replaceCommas(argv[2]);
    int img_count = std::stoi(argv[3]);
    std::string path = argv[4];

    FlickrAPI obj = FlickrAPI(api_key, path, request, tags);
    std::cout << "Start uploading photos..." << std::endl;
    obj.uploadPhoto(img_count);
    std::cout << "Done!" << std::endl;
    return 0;
}

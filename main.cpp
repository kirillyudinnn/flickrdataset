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

std::string parse_config(fs::path config_path) {
    std::string api_key;

    std::ifstream config_file(config_path);
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
        }

        config_file.close();
    }

    return api_key
}

int main(int argc, char* argv[]) {
    /// request, tags, path, count
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <request> <tags> <save_path> <img_count>";
        return 1;
    }

    fs::path config_path = fs::path(argv[0]).parent_path() / "config.ini"
    std::string request = replaceSpaces(argv[1])
    std::string tags = replaceCommas(argv[2])
    fs::path save_path = fs::path(argv[3])
    int img_count = std::stoi(argv[4])

    if (!fs::exists(save_path)) {
        std::cerr << "Error path '" << save_path << "' doesn't exist" << std::endl;
        return 1;
    }

    if (!fs::exists(config_path)) {
        std::cerr << "Error: Config file not found at " << configPath << std::endl;
        return 1;
    }

    std::string api_key = parse_config(config_path);

    FlickrAPI obj = FlickrAPI(api_key, save_path);
    std::cout << "Start uploading photos..." << std::endl;
    obj.uploadPhoto(request, tags, img_count);
    std::cout << "Done!" << std::endl;
    std::cin >>;

	return 0;
}
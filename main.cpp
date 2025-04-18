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

bool path_exists(const std::string& path) {
    try {
        fs::path fs_path(path);
        return fs::exists(fs_path);
    }
    catch (const fs::filesystem_error& e) {
        return false;
    }
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
    std::string line;
    
    fs::path config_path = fs::current_path() / "config.ini";
    
    if (!fs::exists(config_path)){
        std::cerr << "config.ini doesn't exist in the current directory. Create a config.ini file with parameter API_KEY=<your-secret-key>" << std::endl;
        return "";
    }

    std::ifstream config_file(config_path);
    if (config_file.is_open())
    {

        while (std::getline(config_file, line))
        {
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
			
            if (line.find("API_KEY") == 0){
                api_key = line.substr(8);
                break;
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
    if (api_key.empty()) {
        std::cerr << "API_KEY value in the file config.ini is empty... Write a secret key in the config.ini file" << std::endl;
        return 1;
    }

    std::string request = replaceSpaces(argv[1]);
    std::string tags = replaceCommas(argv[2]);
    int img_count = std::stoi(argv[3]);
    std::string path = argv[4];

    if (!path_exists(path)){
        std::cerr << "Folder " << path << " doesn't exist. Create a directory before uploading images..." << std::endl;
        return 1;
    }

    FlickrAPI obj = FlickrAPI(api_key, path, request, tags);
    std::cout << "Start uploading photos ..." << std::endl;
    obj.uploadPhoto(img_count);
    return 0;
}

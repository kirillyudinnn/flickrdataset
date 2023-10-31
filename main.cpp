#include "FlickrAPI.h"
#include <string>
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


int main(int argc, char* argv[])
{   
    std::string path;
    std::string request;
    std::string tags;
    int count = 1;

    std::cout << "Enter your request\n";
    getline(std::cin, request);
    std::cout << "Enter tags (single words separated by commas)\n";
    std::cin >> tags;
    std::cout << "Enter the number of images\n";
    std::cin >> count;
    std::cout << "Enter the path to directory" << std::endl;
    std::cin >> path;
    
    std::pair<std::string, std::string> api = parse_config();

    FlickrAPI obj = FlickrAPI(api.first,api.second, path, replaceSpaces(request), replaceCommas(tags), count);
    obj.photos_search();

}
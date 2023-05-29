#include "FlickrAPI.h"

std::string FlickrAPI::getRequest(int k) {

    return this->API_MAIN + this->API_KEY + this->req + this->tag + this->tag_mode + this->license + this->sort + this->content + this->per_page + this->page + std::to_string(k) + this->format + this->callback;
};

size_t writeData(void* ptr, size_t size, size_t nmemb, FILE* stream) {

    return size_t(fwrite(ptr, size, nmemb, stream));
};

size_t FlickrAPI::responseToString(void* data, size_t size, size_t nmemb, void* clientp) {
    ((std::string*)clientp)->append((char*)data, size * nmemb);
    return size * nmemb;
};

void FlickrAPI::photos_search() {
    const int max_json_length = 500; 
    int n = 1; 
    int k = this->count / max_json_length; 
    int z = this->count % max_json_length; 	
    for (int j = 0; j < k; j++) {
        CURL* curl = curl_easy_init();
        json json_response = json::parse(getJsonRequest(j+1));
        json_response = json_response["photos"]["photo"];

        for (int i = 0; i < json_response.size(); i++) {
            int farm_id = json_response[i]["farm"];
            std::string server_id = json_response[i]["server"];
            std::string id = json_response[i]["id"];
            std::string secret = json_response[i]["secret"];
            std::vector<std::string> fields{ std::to_string(farm_id), server_id, id, secret };
            std::string file_name = path + std::to_string(i + max_json_length*j) + ".jpg";
            const char* name = file_name.c_str();
            FILE* file = fopen(name, "wb");

            CURLcode response;

            curl_easy_setopt(curl, CURLOPT_URL, getPhotoURL(fields).c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
            response = curl_easy_perform(curl);

            fclose(file);
        }

        curl_easy_cleanup(curl);

    }

    if (z != 0) {
        CURL* curl = curl_easy_init();
        json json_response = json::parse(getJsonRequest(k + 1));
        json_response = json_response["photos"]["photo"];

        for (int i = 0; i < z; i++) {
            int farm_id = json_response[i]["farm"];
            std::string server_id = json_response[i]["server"];
            std::string id = json_response[i]["id"];
            std::string secret = json_response[i]["secret"];
            std::vector<std::string> fields{ std::to_string(farm_id), server_id, id, secret };
            std::string file_name = path + std::to_string(i + max_json_length * k) + ".jpg";
            const char* name = file_name.c_str();
            FILE* file = fopen(name, "wb");

            CURLcode response;

            curl_easy_setopt(curl, CURLOPT_URL, getPhotoURL(fields).c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
            response = curl_easy_perform(curl);

            fclose(file);
        }

        curl_easy_cleanup(curl);

    }
};

std::string FlickrAPI::getJsonRequest(int k) {
    CURL* curl;
    CURLcode response;

    std::string str_response;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, getRequest(k).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseToString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str_response);
    response = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return str_response;
};


std::string FlickrAPI::getPhotoURL(std::vector<std::string>& fields) {
    return farm + fields[0] + staticflickr + fields[1] + '/' + fields[2] + '_' + fields[3] + jpg;
}
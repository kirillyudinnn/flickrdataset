#include "FlickrAPI.h"

std::string FlickrAPI::getRequest(int n) {

    return this->API_MAIN + this->API_KEY + this->req + this->tag + this->tag_mode + this->license + this->sort + this->content + this->per_page + this->page + std::to_string(n) + this->format + this->callback;
};

size_t writeData(void* ptr, size_t size, size_t nmemb, FILE* stream) {

    return size_t(fwrite(ptr, size, nmemb, stream));
};

size_t FlickrAPI::responseToString(void* data, size_t size, size_t nmemb, void* clientp) {
    ((std::string*)clientp)->append((char*)data, size * nmemb);
    return size * nmemb;
};


void FlickrAPI::processPhoto(int batch_index, int count, int max_json_length) {
    CURL* curl = curl_easy_init();
    json json_response = json::parse(
        getJsonRequest(batch_index + 1)
    );
    json_response = json_response["photos"]["photo"];

    for (int i = 0; i < count; i++) {
        int farm_id = json_response[i]["farm"];
        std::string server_id = json_response[i]["server"];
        std::string id = json_response[i]["id"];
        std::string secret = json_response[i]["secret"];
        std::vector<std::string> fields{ std::to_string(farm_id), server_id, id, secret };
        std::string file_name = save_dir / (std::to_string(i + max_json_length * batch_index) + jpg);
        const char* name = file_name.c_str();
        FILE* file = fopen(name, "wb");

        CURLcode response;
        curl_easy_setopt(curl, CURLOPT_URL, getPhotoURL(fields).c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        response = curl_easy_perform(curl);

        if (response != CURLE_OK) {
          std::cerr << "CURL error: " << curl_easy_strerror(response) << std::endl;
        }

        fclose(file);
    }

    curl_easy_cleanup(curl);
};


void FlickrAPI::uploadPhoto(int photo_count) {
    const int max_json_length = 500;
    int batch_count = photo_count / max_json_length;
    int remaining_count = photo_count % max_json_length;

    for (int batch_index = 0; batch_index < batch_count; batch_index++) {
      	processPhoto(batch_index, max_json_length, max_json_length);
    }

    if (remaining_count != 0) {
        processPhoto(batch_count, remaining_count, max_json_length);
    }
};

std::string FlickrAPI::getJsonRequest(int n) {
    CURL* curl;
    CURLcode response;

    std::string str_response;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, getRequest(n).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseToString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str_response);
    response = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return str_response;
};


std::string FlickrAPI::getPhotoURL(std::vector<std::string>& fields) {
    return farm + fields[0] + staticflickr + fields[1] + '/' + fields[2] + '_' + fields[3] + jpg;
}
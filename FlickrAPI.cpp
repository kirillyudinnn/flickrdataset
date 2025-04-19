#include "FlickrAPI.h"

std::string FlickrAPI::buildRequestString(int n) {
    return this->API_MAIN + this->API_KEY + this->req + this->tag + this->tag_mode + this->license + this->sort + this->content + this->per_page + this->page + std::to_string(n) + this->format + this->callback;
};

size_t writeData(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return size_t(fwrite(ptr, size, nmemb, stream));
};

size_t FlickrAPI::responseToString(void* data, size_t size, size_t nmemb, void* clientp) {
    ((std::string*)clientp)->append((char*)data, size * nmemb);
    return size * nmemb;
};

bool FlickrAPI::checkInternetConnection(){
    CURL* curl = curl_easy_init();
    std::string yandex_response;
    curl_easy_setopt(curl, CURLOPT_URL, "https://ya.ru");
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1024L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 3L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseToString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &yandex_response);

    CURLcode response = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return (response == CURLE_OK);
};

bool FlickrAPI::validateAPIKey(){
    std::string echo_test_resp;
    CURL* curl = curl_easy_init();
    CURLcode response;

    if (curl) {
        std::string test_url = "https://api.flickr.com/services/rest/?method=flickr.test.echo" + this->API_KEY + "&format=json&nojsoncallback=1";
        
        if (!checkInternetConnection()) {
            std::cerr << "No Internet connection..." << std::endl;
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, test_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseToString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &echo_test_resp);
        response = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (response == CURLE_OK) {
            json json_response = json::parse(echo_test_resp);
            std::string stat = json_response["stat"];

            if (stat == "ok") {
                return true;
            }
            else {
                std::string message = json_response["message"];
                std::cerr << message << std::endl;
                return false;
            }
        }
        else {
            std::cerr << "flickr.com is not available..." << std::endl;
            return false;
        }
    }
        else {
            std::cerr << "Something went wrong... Couldn't initialize curl object" << std::endl;
            return false;
        }
}

bool FlickrAPI::processPhoto(int batch_index, int count, int max_json_length) {
    std::string batch_images_metadata = getImagesMetadata(batch_index + 1);

    if (batch_images_metadata != "")
    {
        std::cout << "Uploading images from batch [" << batch_index + 1 << "]" << std::endl;
        json json_response = json::parse(batch_images_metadata);
        json_response = json_response["photos"]["photo"];
        if (json_response.size() == 0) {
            std::cerr << "There are no images for your request. Try something else..." << std::endl;
            return false;
        }
        for (int i = 0; i < count; i++) 
        {
            int percentage = (i * 100) / count;
            if (percentage % 20 == 0 && i > 1) {
                std::cout << percentage << "% (" << i << "/" << count << ") of images from batch uploaded sucessfully" << std::endl;
            }
            

            int farm_id = json_response[i]["farm"];
            std::string server_id = json_response[i]["server"];
            std::string id = json_response[i]["id"];
            std::string secret = json_response[i]["secret"];
            std::vector<std::string> fields{ std::to_string(farm_id), server_id, id, secret };

            std::string image_url = buildImageURL(fields);

            std::string file_name = save_dir / (std::to_string(i + max_json_length * batch_index) + jpg);
            const char* filepath = file_name.c_str();
            FILE* file = fopen(filepath, "wb");

            CURL* curl = curl_easy_init();
            CURLcode response;
            curl_easy_setopt(curl, CURLOPT_URL, image_url.c_str());
            curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
            curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 10L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
            response = curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            if (fs::file_size(filepath) == 0) {
                fs::remove(filepath);
            }
            else {
                fclose(file);
            }

            if (response != CURLE_OK) {
                if (response == 7) {
                    ///bad answer. Just skip image
                }
                else {
                    std::cerr << "Probably you lost Internet connection or speed is less than 1 byte/sec for 10 seconds." << std::endl;
                    std::cerr << "CURL error: " << curl_easy_strerror(response) << std::endl;
                    return false;
                }
            }
        }
    }
    return true;
};


void FlickrAPI::uploadPhoto(int photo_count) {
    bool is_connection_available = validateAPIKey();
    bool is_successfully = false;
    if (is_connection_available) {
        const int max_json_length = 500;
        int batch_count = photo_count / max_json_length;
        int remaining_count = photo_count % max_json_length;

        for (int batch_index = 0; batch_index < batch_count; batch_index++) {
            is_successfully = processPhoto(batch_index, max_json_length, max_json_length);
            if (!is_successfully) {break;}
        }

        if (remaining_count != 0) {
            is_successfully = processPhoto(batch_count, remaining_count, max_json_length);
        }
    }
    
    else {}
};


std::string FlickrAPI::getImagesMetadata(int n_batch) {
    std::string batch_images_request = buildRequestString(n_batch);
    std::string batch_images_metadata;

    CURL* curl;
    CURLcode response;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, batch_images_request.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseToString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &batch_images_metadata);
    response = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (response == CURLE_OK){
        return batch_images_metadata;
    }
    else {
        std::cerr << "Something went wrong with connection. Please try again or check your Internet connection...";
        return "";
    }
};


std::string FlickrAPI::buildImageURL(std::vector<std::string>& fields) {
    return farm + fields[0] + staticflickr + fields[1] + '/' + fields[2] + '_' + fields[3] + jpg;
}
#pragma once
#define CURL_STATICLIB
#pragma warning(disable:4996)

#include <iostream>
#include <vector>
#include <curl/curl.h>
#include "nlohmann/json.hpp"
#include <fstream>
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

/*!
 * \class FlickrAPI
 * \brief Основной класс для сбора изображений по запросу.
 */
 
class FlickrAPI {
public:
	/*!
     * \brief Конструктор класса
	 * \param API_KEY Уникальный ключ пользователя API 
	 * \param dir_path Путь к директории, где сохраняются изображения
	 * \param request Запрос пользователя
	 * \param tags Теги
     */
	FlickrAPI(std::string API_KEY, std::string dir_path, std::string request, std::string tags) {
		this->API_KEY += API_KEY;
		this->save_dir = dir_path;
		this->tags = tags;
		this->tag += tags;
		this->text = request;
		this->req += request;
	}

	/*!
     * \brief Функция для приведения ответа в виде потока байтов к типу String
     * \param data Указатель на блок данных, который нужно сохранить
     * \param size Размер каждого элемента данных
     * \param nmemb Количество элементов данных
     * \param clientp Указатель на пользовательские данные
     *
     * \return Возвращает общий размер данных, сохраненных в строку
     */
	static size_t responseToString(void* data, size_t size, size_t nmemb, void* clientp);

	/*!
     * \brief  Функция для формирования URL запрос
	 * \param n Номер батча (номер страницы в запросе)
     * \return Возвращает строку для отправки GET-запроса
     */
	std::string buildRequestString(int n_batch);

	/*!
     * \brief Отправляет GET-запрос к flickr.com с методом photos.search 
	 * \param n Номер батча (номер страницы в запросе)
     * \return Возвращает JSON ответ в виде строки
     */
	std::string getImagesMetadata(int n_batch);

	/*!
     * \brief Формирует адрес для запроса 
     * \param fields Необходимые поля, полученные из JSON-ответа, для формирования URL фотогографии
     */
	std::string buildImageURL(std::vector<std::string>& fields);

	/*!
     * \brief Метод для выгрузки изображений.
	 * \param photo_count Количество изображений
     */
	void uploadPhoto(int photo_count);

    /*!
     * \brief Метод, внутри которого формируются и отправляются запросы для батча.
	 * \param batch_index Номер батча
	 * \param count Количество изображений на странице
	 * \param max_json_length Максимальное количество изображений в батче
     */
    bool processPhoto(int batch_index, int count, int max_json_length);

	/*!
     * \brief Метод для валидации API ключа на flickr.com
	 * \return Возвращает true, если API ключ валидный
     */
	bool validateAPIKey();

	/*!
     * \brief Метод для проверки соединения с интернетом
	 * \return Возвращает true если установлено соединение, иначе false
     */
	bool checkInternetConnection();

private:
	std::string text{}; ///< Текст запроса
	fs::path save_dir{}; ///< Путь к директории данных
	std::string tags{}; ///< Теги
	std::string API_KEY = "&api_key="; ///< API ключ
	std::string API_MAIN = "https://www.flickr.com/services/rest/?method=flickr.photos.search"; ///< URL ресурса
	std::string page = "&page="; ///< Строка для указания номера страницы
	std::string per_page = "&per_page=500"; ///< Строка для указания кол-ва фото в ответе
	std::string req = "&text="; ///< Строка для указания запроса 
	std::string tag = "&tags="; ///< Строка для указаний тегов
	int count = 1; ///< Кол-во изображений

	const std::string format = "&format=json"; ///< Строка для указания формата ответа
	const std::string content = "&content_type=1"; ///< Строка для указания типа контента (фото+скрины)
	const std::string callback = "&nojsoncallback=1"; ///< Строка для указания callback
	const std::string license = "&license=0,7,8,9,10"; ///< Строка для указания лицензии фотографий
	const std::string farm = "https://farm"; ///< Строка для указания фермы
	const std::string staticflickr = ".staticflickr.com/"; ///< Вспомогательная строка
	const std::string jpg = ".jpg"; ///< Формат фотографий
	const std::string tag_mode = "&tags_mode=any"; ///< Строка для указания перебора тегов
	const std::string sort = "&sort=relevance"; ///< Строка для указания сортировки ответа по популярности
};
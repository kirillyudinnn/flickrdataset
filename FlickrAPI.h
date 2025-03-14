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
     */
	FlickrAPI(std::string API_KEY, fs::path save_path) {
		this->API_KEY += API_KEY;
		this->save_path = save_path
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
	 * \param page_number Номер батча (номер страницы в запросе)
     * \return Возвращает строку для отправки GET-запроса
     */
	std::string getRequest(int page_number);

	/*!
     * \brief Отправляет GET-запрос к flickr.com
	 * \param page_number Номер батча (номер страницы в запросе)
     * \return Возвращает JSON ответ в виде строки
     */
	std::string getJsonRequest(int page_number);

	/*!
     * \brief Формирует адрес для запроса 
     * \param fields Необходимые поля, полученные из JSON-ответа, для формирования URL фотогографии
     */
	std::string getPhotoURL(std::vector<std::string>& fields);

	/*!
     * \brief Метод для выгрузки изображений.
	 * \param photo_count Количество изображений
     */
	void uploadPhoto(std::string text, std::string tags, int photo_count);

    /*!
     * \brief Метод, внутри которого формируются и отправляются запросы для батча.
	 * \param batch_index Номер батча
	 * \param count Количество изображений на странице
	 * \papram max_json_length Максимальное количество изображений в батче
     */
    void processPhoto(int batch_index, int count, int max_json_length);


private:
	std::string text{}; ///< Текст запроса
	std::string path{}; ///< Путь к директории данных
	std::string tags{}; ///< Теги
	std::string API_KEY = "&api_key="; ///< API ключ
	std::string page = "&page="; ///< Строка для указания номера страницы
	std::string per_page = "&per_page=500"; ///< Строка для указания кол-ва фото в ответе
	std::string req = "&text="; ///< Строка для указания запроса 
	std::string tag = "&tags="; ///< Строка для указаний тегов
	fs::path save_path{};
	int count = 1; ///< Кол-во изображений

	const std::string API_MAIN = "https://www.flickr.com/services/rest/?method=flickr.photos.search"; ///< end-point
	const std::string format = "&format=json"; ///< Строка для указания формата ответа
	const std::string content = "&content_type=1"; ///< Строка для указания типа контента (фотографии, изображения, скрины)
	const std::string callback = "&nojsoncallback=1"; ///< Строка для указания callback
	const std::string license = "&license=0,7,8,9,10"; ///< Строка для указания лицензии изображений
	const std::string farm = "https://farm"; ///< Строка для указания фермы
	const std::string staticflickr = ".staticflickr.com/"; ///< Вспомогательная строка
	const std::string jpg = ".jpg"; ///< Формат фотографий
	const std::string tag_mode = "&tags_mode=all"; ///< Строка для указания перебора тегов
	const std::string sort = "&sort=interestingness-desc"; ///< Строка для указания сортировки ответа по популярности
};
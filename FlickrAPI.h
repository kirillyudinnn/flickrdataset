#pragma once
#define CURL_STATICLIB
#pragma warning(disable:4996)

#include <iostream>
#include <vector>
#include <curl/curl.h>
#include "nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::json;

/*!
 * \class FlickrAPI
 * \brief Основной класс для работы с flickr.com
 */
 
class FlickrAPI {
public:
	/*!
     * \brief Конструктор класса
	 * \param API_KEY Уникальный ключ пользователя API 
	 * \param API_MAIN Адрес, по которому создается запрос 
	 * \param dir_path Путь к директории, где сохраняются изображения
	 * \param request Запрос пользователя
	 * \param tags Теги
	 * \param count Кол-во изображений
     */
	FlickrAPI(std::string API_KEY,std::string API_MAIN,std::string dir_path, std::string request, std::string tags, int count) {
		this->API_KEY = API_KEY;
		this->API_MAIN = API_MAIN;
		this->path = dir_path;
		this->tags = tags;
		this->tag += tags;
		this->text = request;
		this->req += request;
		this->count = count;
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
     * \return Возвращает строку для отправки GET-запроса
     */
	std::string getRequest(int n);

	/*!
     * \brief Создает GET-запрос к flickr.com
     * \return Возвращает JSON ответ в виде строки
     */
	std::string getJsonRequest(int k);

	/*!
     * \brief Формирует адрес для запроса 
     * \param fields Содержит в себе farm_id, server_id, id, secret
     */
	std::string getPhotoURL(std::vector<std::string>& fields);

	/*!
     * \brief Основный метод, который скачивает изображения
     */
	void photos_search();

private:
	std::string text{}; ///< Текст запроса
	std::string path{}; ///< Путь к директории данных
	std::string tags{}; ///< Теги
	std::string API_KEY{}; ///< API ключ
	std::string API_MAIN{}; ///< URL ресурса
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
	const std::string tag_mode = "&tags_mode=all"; ///< Строка для указания перебора тегов
	const std::string sort = "&sort=interestingness-desc"; ///< Строка для указания сортировки ответа по популярности
};
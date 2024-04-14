#ifndef __MONGO_H__
#define __MONGO_H__

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <string>
#include <optional>

class Mongo{
public:
  Mongo(const char* uri_string, const char* database);
  ~Mongo() = default;
  int addUser(const char*username, const char* password);
  int removeUserByUsername(const std::string& username);
  int addFriendByUsername(const std::string& username, const std::string& friendUsername);
 int removeFriendByUsername(const std::string& username, const std::string& friendUsername);

  int addRestaurant(const std::string& name, const std::string& location, int price_range, double stars, const std::string& website_url, const std::string& image_url, const std::vector<std::string>& tags);
  int addRestaurantToUserList(const std::string& username, const std::string& restaurantName, double rating);
  int removeRestaurantFromUserList(const std::string& username, const std::string& restaurantName);
  int updateRestaurantRatingInUserList(const std::string& username, const std::string& restaurantName, double newRating);
  bool login(std::string& username, std::string& password);

private:
  static mongocxx::instance inst;
  mongocxx::client client;
  mongocxx::database database;
  mongocxx::collection userstoken, username;
  mongocxx::collection restaurants;
  std::optional<bsoncxx::oid> findUserByUsername(const std::string& username);
  std::optional<bsoncxx::oid> findRestaurantByName(const std::string& name);
  std::string hashPassword(const char* password);

};

#endif 

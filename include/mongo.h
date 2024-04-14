#ifndef __MONGO_H__
#define __MONGO_H__

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <string>
#include <optional>
#include <set>
#include <crow.h>

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
  int addRestaurantToVisitList(const std::string& username, const std::string& restaurantName);
  int removeRestaurantFromUserList(const std::string& username, const std::string& restaurantName);
  int updateRestaurantRatingInUserList(const std::string& username, const std::string& restaurantName, double newRating);
  bool login(std::string& username, std::string& password);


  //std::set<std::string> getVisitedRestaurants(const std::string& username);
  //bsoncxx::document::value getVisitedRestaurants(const std::string& username);
  //nlohmann::json getVisitedRestaurants(const std::string& username);
  std::vector<std::pair<std::string, double>> getVisitedRestaurants(const std::string &username);
  std::set<std::string> getWantedRestaurants(const std::string& username);
  std::set<std::string> getAllRestaurants();
  std::string printRestaurantDetails(const std::string &id);
  std::set<std::string> getVisitedRestaurantsId(const std::string &username);
  int moveRestaurantToVisited(const std::string& username, const std::string& restaurantId, double rating);


  //TODO

  //server
  void logout(std::string);

  //get retaurtsn user wants to visit
  //get restaunt user has visited
  //get all restaurants
  //get users friends
  //
  //maybe search all?
  //
  //add wants to visit

private:
  static mongocxx::instance inst;
  mongocxx::client client;
  mongocxx::database database;
  mongocxx::collection users;
  mongocxx::collection restaurants;
  std::optional<bsoncxx::oid> findUserByUsername(const std::string& username);
  std::optional<bsoncxx::oid> findRestaurantByName(const std::string& name);
  std::string hashPassword(const char* password);

};

#endif 

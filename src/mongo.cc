#include "json.hpp"
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <crow.h>
#include <cstdlib>
#include <exception>
#include <mongo.h>
#include <picosha2.h>

mongocxx::instance Mongo::inst{};

Mongo::Mongo(const char *uri_string, const char *database_name) {
  try {
    mongocxx::uri uri(uri_string);
    client = mongocxx::client(uri);
    database = client[database_name];
    users = database["users"];
    restaurants = database["restaurants"];

  } catch (const std::exception &e) {
    std::cerr << "Error initializing mongo connection" << std::endl;
    std::cerr << e.what() << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

std::string Mongo::hashPassword(const char *password) {
  std::string src_str = password;

  std::vector<unsigned char> hash(picosha2::k_digest_size);
  picosha2::hash256(src_str.begin(), src_str.end(), hash.begin(), hash.end());

  std::string hex_str = picosha2::bytes_to_hex_string(hash.begin(), hash.end());
  return hex_str;
}

int Mongo::addUser(const char *username, const char *password) {
  if (findUserByUsername(username))
    return 1;

  try {

    auto builder = bsoncxx::builder::basic::document{};
    builder.append(bsoncxx::builder::basic::kvp("username", username));
    builder.append(
        bsoncxx::builder::basic::kvp("password", hashPassword(password)));

    builder.append(bsoncxx::builder::basic::kvp(
        "friends", [](bsoncxx::builder::basic::sub_array sub_arr) {}));
    builder.append(bsoncxx::builder::basic::kvp(
        "restaurants_visited",
        [](bsoncxx::builder::basic::sub_array sub_arr) {}));
    builder.append(bsoncxx::builder::basic::kvp(
        "restaurants_want_visit",
        [](bsoncxx::builder::basic::sub_array sub_arr) {}));

    bsoncxx::document::view view = builder.view();
    auto result = users.insert_one(view);
    std::cout << "User inserted successfully with empty friends and "
                 "restaurants_visited lists."
              << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "An error occurred while trying to create user: " << username
              << " --> " << e.what() << std::endl;
  }

  return 0;
}

int Mongo::removeUserByUsername(const std::string &username) {
  try {
    auto query = bsoncxx::builder::basic::document{};
    query.append(bsoncxx::builder::basic::kvp("username", username));

    auto result = users.delete_one(query.view());
    if (result) {
      if (result->deleted_count() > 0) {
        std::cout << "User " << username << " deleted successfully."
                  << std::endl;
        return 0; // Success
      } else {
        std::cout << "No user found with the username: " << username
                  << std::endl;
        return 1; // No user found to delete
      }
    }
    std::cerr << "Delete operation failed to execute." << std::endl;
    return 2; // Delete operation failed
  } catch (const std::exception &e) {
    std::cerr << "An exception occurred: " << e.what() << std::endl;
    return 3; // Exception caught
  }
}

std::optional<bsoncxx::oid>
Mongo::findUserByUsername(const std::string &username) {
  auto query = bsoncxx::builder::basic::document{};
  query.append(bsoncxx::builder::basic::kvp("username", username));

  try {
    auto maybe_result = users.find_one(query.view());
    if (maybe_result) {
      auto view = maybe_result->view();
      auto oid = view["_id"].get_oid().value;
      return oid;
    }
  } catch (const std::exception &e) {
    std::cerr << "Error finding user by username: " << e.what() << std::endl;
  }
  return {};
}

int Mongo::addFriendByUsername(const std::string &username,
                               const std::string &friendUsername) {
  auto userIdOpt = findUserByUsername(username);
  if (!userIdOpt.has_value()) {
    std::cerr << "User '" << username << "' does not exist." << std::endl;
    return 1; // Original user not found
  }

  auto friendIdOpt = findUserByUsername(friendUsername);
  if (!friendIdOpt.has_value()) {
    std::cerr << "Friend user '" << friendUsername << "' does not exist."
              << std::endl;
    return 2; // Friend not found
  }

  try {
    auto filter = bsoncxx::builder::basic::document{};
    filter.append(bsoncxx::builder::basic::kvp("username", username));

    auto update = bsoncxx::builder::basic::document{};
    update.append(bsoncxx::builder::basic::kvp(
        "$addToSet", [&](bsoncxx::builder::basic::sub_document subdoc) {
          subdoc.append(bsoncxx::builder::basic::kvp("friends",
                                                     friendIdOpt->to_string()));
        }));

    auto result = users.update_one(filter.view(), update.view());
    if (result && result->modified_count() > 0) {
      std::cout << "Friend '" << friendUsername
                << "' added successfully to user '" << username << "'."
                << std::endl;
      return 0; // Success, friend added
    } else {
      std::cout << "Friend '" << friendUsername
                << "' is already added to the user '" << username << "'."
                << std::endl;
      return 3; // Friend already in the list
    }
  } catch (const std::exception &e) {
    std::cerr << "An exception occurred while adding a friend: " << e.what()
              << std::endl;
    return 4; // Exception caught
  }
}

int Mongo::removeFriendByUsername(const std::string &username,
                                  const std::string &friendUsername) {
  auto userIdOpt = findUserByUsername(username);
  if (!userIdOpt.has_value()) {
    std::cerr << "User '" << username << "' does not exist." << std::endl;
    return 1; // User not found
  }

  auto friendIdOpt = findUserByUsername(friendUsername);
  if (!friendIdOpt.has_value()) {
    std::cerr << "Friend user '" << friendUsername << "' does not exist."
              << std::endl;
    return 2; // Friend not found
  }

  try {
    auto filter = bsoncxx::builder::basic::document{};
    filter.append(bsoncxx::builder::basic::kvp("username", username));

    auto update = bsoncxx::builder::basic::document{};
    update.append(bsoncxx::builder::basic::kvp(
        "$pull", [&](bsoncxx::builder::basic::sub_document subdoc) {
          subdoc.append(bsoncxx::builder::basic::kvp("friends",
                                                     friendIdOpt->to_string()));
        }));

    auto result = users.update_one(filter.view(), update.view());
    if (result && result->modified_count() > 0) {
      std::cout << "Friend successfully removed from user's list." << std::endl;
      return 0; // Success
    } else {
      std::cout << "Failed to remove friend from user's list or friend not "
                   "found in list."
                << std::endl;
      return 3; // Update failed or friend not in list
    }
  } catch (const std::exception &e) {
    std::cerr
        << "An exception occurred while removing a friend from user's list: "
        << e.what() << std::endl;
    return 4; // Exception caught
  }
}

std::optional<bsoncxx::oid>
Mongo::findRestaurantByName(const std::string &name) {
  auto query = bsoncxx::builder::basic::document{};
  query.append(bsoncxx::builder::basic::kvp("name", name));

  try {
    auto result = restaurants.find_one(query.view());
    if (result) {
      auto view = result->view();
      auto oid = view["_id"].get_oid().value;
      return oid; // Return the ObjectId of the restaurant
    }
  } catch (const std::exception &e) {
    std::cerr << "An error occurred while checking for the restaurant: "
              << e.what() << std::endl;
  }
  return {}; // No restaurant found or an error occurred
}

int Mongo::addRestaurant(const std::string &name, const std::string &location,
                         int price_range, double stars,
                         const std::string &website_url,
                         const std::string &image_url,
                         const std::vector<std::string> &tags) {
  auto restaurantIdOpt = findRestaurantByName(name);
  if (restaurantIdOpt.has_value()) {
    std::cout << "Restaurant already exists with ID: "
              << restaurantIdOpt->to_string() << std::endl;
    return 1; // Restaurant already exists
  }

  try {
    auto builder = bsoncxx::builder::basic::document{};
    builder.append(bsoncxx::builder::basic::kvp("name", name));
    builder.append(bsoncxx::builder::basic::kvp("location", location));
    builder.append(bsoncxx::builder::basic::kvp("price_range", price_range));
    builder.append(bsoncxx::builder::basic::kvp("stars", stars));
    builder.append(bsoncxx::builder::basic::kvp("website_url", website_url));
    builder.append(bsoncxx::builder::basic::kvp("image_url", image_url));

    // Add tags to the document
    builder.append(bsoncxx::builder::basic::kvp(
        "tags", [&](bsoncxx::builder::basic::sub_array sub_arr) {
          for (const auto &tag : tags) {
            sub_arr.append(tag);
          }
        }));

    auto result = restaurants.insert_one(builder.view());
    std::cout << "Restaurant added successfully." << std::endl;
    return 0; // Success
  } catch (const std::exception &e) {
    std::cerr << "An error occurred while adding a new restaurant: " << e.what()
              << std::endl;
    return 2; // Error occurred
  }
}

int Mongo::addRestaurantToUserList(const std::string &username,
                                   const std::string &restaurantName,
                                   double rating) {
  auto userIdOpt = findUserByUsername(username);
  if (!userIdOpt.has_value()) {
    std::cerr << "User '" << username << "' does not exist." << std::endl;
    return 1; // User not found
  }

  auto restaurantIdOpt = findRestaurantByName(restaurantName);
  // if (!restaurantIdOpt.has_value()) {
  //   std::cerr << "Restaurant '" << restaurantName << "' does not exist."
  //             << std::endl;
  //   return 2; // Restaurant not found
  // }

  try {
    bsoncxx::oid oid(restaurantName);
    auto filter = bsoncxx::builder::basic::document{};
    filter.append(bsoncxx::builder::basic::kvp("username", username));

    auto update = bsoncxx::builder::basic::document{};
    update.append(bsoncxx::builder::basic::kvp(
        "$push", [&](bsoncxx::builder::basic::sub_document subdoc) {
          subdoc.append(bsoncxx::builder::basic::kvp(
              "restaurants_visited",
              [&](bsoncxx::builder::basic::sub_document subdoc2) {
                subdoc2.append(
                    bsoncxx::builder::basic::kvp("restaurant_id", oid));
                subdoc2.append(bsoncxx::builder::basic::kvp("rating", rating));
              }));
        }));

    auto result = users.update_one(filter.view(), update.view());
    if (result && result->modified_count() > 0) {
      std::cout << "Restaurant added to user's list successfully." << std::endl;
      return 0; // Success
    } else {
      std::cout << "Failed to add restaurant to user's list." << std::endl;
      return 3; // Update failed
    }
  } catch (const std::exception &e) {
    std::cerr
        << "An exception occurred while adding a restaurant to user's list: "
        << e.what() << std::endl;
    return 4; // Exception caught
  }
}

int Mongo::addRestaurantToVisitList(const std::string &username,
                                    const std::string &restaurantId) {
  auto userIdOpt = findUserByUsername(username);
  if (!userIdOpt.has_value()) {
    std::cerr << "User '" << username << "' does not exist." << std::endl;
    return 1; // User not found
  }

  try {
    bsoncxx::oid oid(restaurantId);

    auto filter = bsoncxx::builder::basic::document{};
    filter.append(bsoncxx::builder::basic::kvp("username", username));

    auto update = bsoncxx::builder::basic::document{};
    update.append(bsoncxx::builder::basic::kvp(
        "$push", [&](bsoncxx::builder::basic::sub_document subdoc) {
          subdoc.append(
              bsoncxx::builder::basic::kvp("restaurants_want_visit", oid));
        }));

    auto result = users.update_one(filter.view(), update.view());
    if (result && result->modified_count() > 0) {
      std::cout << "Restaurant added to user's list successfully." << std::endl;
      return 0; // Success
    } else {
      std::cout << "Failed to add restaurant to user's list." << std::endl;
      return 3; // Update failed
    }
  } catch (const std::exception &e) {
    std::cerr << "An exception occurred (possibly due to invalid ObjectId): "
              << e.what() << std::endl;
    return 4; // Exception caught
  }
}

int Mongo::removeRestaurantFromUserList(const std::string &username,
                                        const std::string &restaurantName) {
  auto restaurantIdOpt = findRestaurantByName(restaurantName);
  if (!restaurantIdOpt.has_value()) {
    std::cerr << "Restaurant '" << restaurantName << "' does not exist."
              << std::endl;
    return 1; // Restaurant not found
  }

  try {
    auto filter = bsoncxx::builder::basic::document{};
    filter.append(bsoncxx::builder::basic::kvp("username", username));

    auto update = bsoncxx::builder::basic::document{};
    update.append(bsoncxx::builder::basic::kvp(
        "$pull", [&](bsoncxx::builder::basic::sub_document subdoc) {
          subdoc.append(bsoncxx::builder::basic::kvp(
              "restaurants_visited",
              [&](bsoncxx::builder::basic::sub_document subdoc2) {
                subdoc2.append(bsoncxx::builder::basic::kvp(
                    "restaurant_id", restaurantIdOpt->to_string()));
              }));
        }));

    auto result = users.update_one(filter.view(), update.view());
    if (result && result->modified_count() > 0) {
      std::cout << "Restaurant successfully removed from user's list."
                << std::endl;
      return 0; // Success
    } else {
      std::cout
          << "Failed to remove restaurant from user's list or user not found."
          << std::endl;
      return 2; // Update failed or user not found
    }
  } catch (const std::exception &e) {
    std::cerr << "An exception occurred while removing a restaurant from "
                 "user's list: "
              << e.what() << std::endl;
    return 3; // Exception caught
  }
}

int Mongo::updateRestaurantRatingInUserList(const std::string &username,
                                            const std::string &restaurantName,
                                            double newRating) {
  auto restaurantIdOpt = findRestaurantByName(restaurantName);
  if (!restaurantIdOpt.has_value()) {
    std::cerr << "Restaurant '" << restaurantName << "' does not exist."
              << std::endl;
    return 1; // Restaurant not found
  }

  try {
    auto filter = bsoncxx::builder::basic::document{};
    filter.append(bsoncxx::builder::basic::kvp("username", username));
    filter.append(bsoncxx::builder::basic::kvp(
        "restaurants_visited.restaurant_id", restaurantIdOpt->to_string()));

    auto update = bsoncxx::builder::basic::document{};
    update.append(bsoncxx::builder::basic::kvp(
        "$set", [&](bsoncxx::builder::basic::sub_document subdoc) {
          subdoc.append(bsoncxx::builder::basic::kvp(
              "restaurants_visited.$.rating", newRating));
        }));

    auto result = users.update_one(filter.view(), update.view());
    if (result && result->modified_count() > 0) {
      std::cout << "Restaurant rating updated successfully." << std::endl;
      return 0; // Success
    } else {
      std::cout << "Failed to update restaurant rating or restaurant not found "
                   "in user's list."
                << std::endl;
      return 2; // Update failed or restaurant not found in list
    }
  } catch (const std::exception &e) {
    std::cerr << "An exception occurred while updating the restaurant rating: "
              << e.what() << std::endl;
    return 3; // Exception caught
  }
}

bool Mongo::login(std::string &username, std::string &password) {
  try {
    std::string hashed_pass = hashPassword(password.c_str());
    auto filter = bsoncxx::builder::basic::document{};
    filter.append(bsoncxx::builder::basic::kvp("username", username));
    filter.append(bsoncxx::builder::basic::kvp("password", hashed_pass));

    auto result = users.find_one(filter.view());
    return result ? true : false;
  } catch (const std::exception &e) {
    std::cerr << "error trying to log in" << std::endl;
    return false;
  }
}


std::vector<std::pair<std::string, double>>
Mongo::getVisitedRestaurants(const std::string &username) {
    std::vector<std::pair<std::string, double>> visited_restaurants_info;
    try {
        auto query = bsoncxx::builder::stream::document{}
                 << "username" << username
                 << bsoncxx::builder::stream::finalize;
        auto userDoc = users.find_one(query.view());
        if (userDoc) {
            auto elements = userDoc->view()["restaurants_visited"].get_array().value;
            for (auto &element : elements) {
                // Extract the restaurant ID and rating from each subdocument
                auto restaurant_id = element["restaurant_id"].get_oid().value.to_string();
                auto rating = element["rating"].get_double().value;
                visited_restaurants_info.emplace_back(restaurant_id, rating);
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Failed to fetch visited restaurants: " << e.what()
                  << std::endl;
    }
    return visited_restaurants_info;
}

std::set<std::string> Mongo::getWantedRestaurants(const std::string &username) {
  std::set<std::string> wanted_restaurants;
  try {
    auto query = bsoncxx::builder::stream::document{}
                 << "username" << username
                 << bsoncxx::builder::stream::finalize;
    auto userDoc = users.find_one(query.view());
    if (userDoc) {
      auto elements =
          userDoc->view()["restaurants_want_visit"].get_array().value;
      for (auto &element : elements) {
        wanted_restaurants.insert(element.get_oid().value.to_string());
      }
    }
  } catch (const std::exception &e) {
    std::cerr << "Failed to fetch restaurants want to visit: " << e.what()
              << std::endl;
  }
  return wanted_restaurants;
}

std::set<std::string> Mongo::getAllRestaurants() {
  std::set<std::string> restaurant_ids;
  try {
    auto cursor = restaurants.find({});

    for (auto &&doc : cursor) {
      auto id = doc["_id"].get_oid().value.to_string();
      restaurant_ids.insert(id);
    }
  } catch (const std::exception &e) {
    std::cerr << "Error fetching all restaurants: " << e.what() << std::endl;
  }
  return restaurant_ids;
}

std::string Mongo::printRestaurantDetails(const std::string &id) {

  try {
    auto oid = bsoncxx::oid(id);
    auto filter = bsoncxx::builder::stream::document{}
                  << "_id" << oid << bsoncxx::builder::stream::finalize;

    auto result = restaurants.find_one(filter.view());
    if (result) {
      auto view = result->view();
      std::string json_str = bsoncxx::to_json(view);
      return json_str;
      ;
    } else {
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return "";
}

std::set<std::string>
Mongo::getVisitedRestaurantsId(const std::string &username) {
  std::set<std::string> visited_restaurants;
  try {
    auto query = bsoncxx::builder::stream::document{}
                 << "username" << username
                 << bsoncxx::builder::stream::finalize;
    auto userDoc = users.find_one(query.view());
    if (userDoc) {
      auto elements = userDoc->view()["restaurants_visited"].get_array().value;
      for (auto &element : elements) {
        // Assume each element is a document with an 'restaurant_id' field which
        // is an ObjectId
        visited_restaurants.insert(
            element["restaurant_id"].get_oid().value.to_string());
      }
    }
  } catch (const std::exception &e) {
    std::cerr << "Failed to fetch visited restaurants: " << e.what()
              << std::endl;
  }
  return visited_restaurants;
}

int Mongo::moveRestaurantToVisited(const std::string &username,
                                   const std::string &restaurantId,
                                   double rating) {
  auto userIdOpt = findUserByUsername(username);
  if (!userIdOpt.has_value()) {
    std::cerr << "User '" << username << "' does not exist." << std::endl;
    return 1; // User not found
  }

  try {
    bsoncxx::oid oid(restaurantId);

    auto filterPull = bsoncxx::builder::basic::document{};
    filterPull.append(bsoncxx::builder::basic::kvp("username", username));

    auto updatePull = bsoncxx::builder::basic::document{};
    updatePull.append(bsoncxx::builder::basic::kvp(
        "$pull", [&](bsoncxx::builder::basic::sub_document subdoc) {
          subdoc.append(
              bsoncxx::builder::basic::kvp("restaurants_want_visit", oid));
        }));

    auto resultPull = users.update_one(filterPull.view(), updatePull.view());
    if (!resultPull || resultPull->modified_count() == 0) {
      std::cerr << "Failed to remove restaurant from want to visit list."
                << std::endl;
      return 2; // Failed to pull
    }

    auto filterPush = bsoncxx::builder::basic::document{};
    filterPush.append(bsoncxx::builder::basic::kvp("username", username));

    auto updatePush = bsoncxx::builder::basic::document{};
    updatePush.append(bsoncxx::builder::basic::kvp(
        "$push", [&](bsoncxx::builder::basic::sub_document subdoc) {
          subdoc.append(bsoncxx::builder::basic::kvp(
              "restaurants_visited",
              [&](bsoncxx::builder::basic::sub_document subdoc2) {
                subdoc2.append(
                    bsoncxx::builder::basic::kvp("restaurant_id", oid));
                subdoc2.append(bsoncxx::builder::basic::kvp("rating", rating));
              }));
        }));

    auto resultPush = users.update_one(filterPush.view(), updatePush.view());
    if (resultPush && resultPush->modified_count() > 0) {
      std::cout << "Restaurant moved to visited list successfully."
                << std::endl;
      return 0; // Success
    } else {
      std::cout << "Failed to add restaurant to visited list." << std::endl;
      return 3; // Failed to push
    }
  } catch (const std::exception &e) {
    std::cerr << "An exception occurred while moving restaurant between lists: "
              << e.what() << std::endl;
    return 4; // Exception caught
  }
}

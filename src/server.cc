#include <server.h>
#include <mongo.h>

Server::Server(int server_port) : port(server_port), conn(std::getenv("URI"), "foodr") {}

std::optional<std::string> Server::getLoggedInUser(const std::string& token){
  auto it = users.find(token);
    
  if (it == users.end()){
    std::cout << "user not found" << std::endl;
    return std::nullopt;
  }

  return it->second;
}

std::string Server::genToken(){
  std::string str;
  srand(time(0));
  char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";

  for (int i = 0; i < 32; i++) {
    char randomChar = charset[rand() % (sizeof(charset) - 1)];
    str.push_back(randomChar);
  }

  if(users.find(str) != users.end()){
    str = genToken(); // Update the token with the recursively generated unique token
  }

  return str;
}


void Server::setupRoutes(){

  CROW_ROUTE(app, "/")([this](){
    for(const auto& user: users){
      std::cout << user.first<< std::endl;
      std::cout << user.second << std::endl;
      std::cout << std::endl;
    }
    return "Hello Hi Hi!";
  });

  CROW_ROUTE(app, "/hello")([](){
        return "Hello Hi Hi!";
    });

  CROW_ROUTE(app, "/createUser").methods(crow::HTTPMethod::Post)([this](const crow::request& req){
    auto x = crow::json::load(req.body);
    if (!x) {
      return crow::response(400, crow::json::wvalue({{"error", "Invalid JSON"}}));
    }

    std::string username;
    std::string password;
    try{
      username = x["username"].s();
      password = x["password"].s();
    }catch(const std::exception& e){
      return crow::response(400, crow::json::wvalue({{"error", "invalid values"}}));
    }
    int added = conn.addUser(username.c_str(), password.c_str());
    if(added == 1){
      return crow::response(400, crow::json::wvalue({{"error", "username already exists"}, {"username", username}}));
    }

    return crow::response(200, crow::json::wvalue({{"success", "user added"}}));
  });

  CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::Post)([this](const crow::request& req){
    auto x = crow::json::load(req.body);
    if (!x) {
      return crow::response(400, crow::json::wvalue({{"error", "Invalid JSON"}}));
    }

    std::string username;
    std::string password;
    try{
      username = x["username"].s();
      password = x["password"].s();
    }catch(const std::exception& e){
      return crow::response(400, crow::json::wvalue({{"error", "invalid json key values"}}));
    }
    bool logged_in = conn.login(username, password);
    if(logged_in){
      std::string token = genToken();
      users.insert({token, username});
      return crow::response(200, crow::json::wvalue({{"success", token}}));
    }

    return crow::response(400, crow::json::wvalue({{"error", "invalid login"}}));
  });


  CROW_ROUTE(app, "/deleteUser").methods(crow::HTTPMethod::Post)([this](const crow::request& req){
    auto x = crow::json::load(req.body);
    if (!x) {
      return crow::response(400, crow::json::wvalue({{"error", "Invalid JSON"}}));
    }

    std::string token;
    try{
      token = x["token"].s();
    }catch(const std::exception& e){
      return crow::response(400, crow::json::wvalue({{"error", "invalid json key values"}}));
    }
    auto user = getLoggedInUser(token);
    if(!user){
      return crow::response(400, crow::json::wvalue({{"error", "invalid request"}}));
    }

    int removed = conn.removeUserByUsername(user.value());
    if(removed == 0){
      for(auto it = users.begin(); it != users.end(); ) {
        if (it->second == user.value()) {
          it = users.erase(it);  
        } else {
          ++it;  
        }
      }
      return crow::response(200, crow::json::wvalue({{"success", "account deleted"}}));
    }

    return crow::response(400, crow::json::wvalue({{"error", "could not delete account"}}));
  });

  CROW_ROUTE(app, "/getSwipeRestaurants").methods(crow::HTTPMethod::Post)([this](const crow::request& req){
    auto x = crow::json::load(req.body);
    if (!x) {
      return crow::response(400, crow::json::wvalue({{"error", "Invalid JSON"}}));
    }

    std::string token;
    try{
      token = x["token"].s();
    }catch(const std::exception& e){
      return crow::response(400, crow::json::wvalue({{"error", "invalid json key values"}}));
    }

    auto user = getLoggedInUser(token);
    if(!user){
      return crow::response(400, crow::json::wvalue({{"error", "invalid request"}}));
    }

    std::set<std::string> result;
    try{
    auto all_restaurants = conn.getAllRestaurants();
    auto visited_restaurants = conn.getVisitedRestaurantsId(user.value());
    auto wanted_restaurants = conn.getWantedRestaurants(user.value());
    visited_restaurants.insert(wanted_restaurants.begin(), wanted_restaurants.end());
    std::set_difference(all_restaurants.begin(), all_restaurants.end(), visited_restaurants.begin(), visited_restaurants.end(), std::inserter(result, result.begin()));
      std::cout << all_restaurants.size() << std::endl;
      std::cout << visited_restaurants.size() << std::endl;
      std::cout << wanted_restaurants.size() << std::endl;

    }catch(const std::exception e){
      std::cerr << e.what() << std::endl;
      return crow::response(400, crow::json::wvalue({{"error", "swipe curator error"}}));
    }
    crow::json::wvalue jsonResult;
    size_t i = 0;
    for (const std::string& value : result)
    {
        jsonResult[i]["value"] = value;
        i++;
    }
    return crow::response(200, jsonResult);

  });


  CROW_ROUTE(app, "/getRestaurant").methods(crow::HTTPMethod::Post)([this](const crow::request& req){
    auto x = crow::json::load(req.body);
    if (!x) {
      return crow::response(400, crow::json::wvalue({{"error", "Invalid JSON"}}));
    }

    std::string rid;
    try{
      rid = x["rid"].s();
    }catch(const std::exception& e){
      return crow::response(400, crow::json::wvalue({{"error", "invalid json key values"}}));
    }

    auto data = conn.printRestaurantDetails(rid);

    return crow::response(200, data);

  });



  CROW_ROUTE(app, "/likeR").methods(crow::HTTPMethod::Post)([this](const crow::request& req){
    auto x = crow::json::load(req.body);
    if (!x) {
      return crow::response(400, crow::json::wvalue({{"error", "Invalid JSON"}}));
    }

    std::string rid;
    std::string token;
    try{
      rid = x["rid"].s();
      token = x["token"].s();
    }catch(const std::exception& e){
      return crow::response(400, crow::json::wvalue({{"error", "invalid json key values"}}));
    }

    auto user = getLoggedInUser(token);
    if(!user){
      return crow::response(400, crow::json::wvalue({{"error", "invalid request"}}));
    }

    auto val = conn.addRestaurantToVisitList(user.value(), rid);

    if(val == 0) return crow::response(200);
    return crow::response(400);

  });


  CROW_ROUTE(app, "/getWantVisit").methods(crow::HTTPMethod::Post)([this](const crow::request& req){
    auto x = crow::json::load(req.body);
    if (!x) {
      return crow::response(400, crow::json::wvalue({{"error", "Invalid JSON"}}));
    }

    std::string token;
    try{
      token = x["token"].s();
    }catch(const std::exception& e){
      return crow::response(400, crow::json::wvalue({{"error", "invalid json key values"}}));
    }

    auto user = getLoggedInUser(token);
    if(!user){
      return crow::response(400, crow::json::wvalue({{"error", "invalid request"}}));
    }

    std::set<std::string> result;
    try{
    result = conn.getWantedRestaurants(user.value());
    }catch(const std::exception e){
      std::cerr << e.what() << std::endl;
      return crow::response(400, crow::json::wvalue({{"error", "want visit error"}}));
    }
    crow::json::wvalue jsonResult;
    size_t i = 0;
    for (const std::string& value : result)
    {
        jsonResult[i]["value"] = value;
        i++;
    }
    return crow::response(200, jsonResult);

  });


  CROW_ROUTE(app, "/getHaveVisit").methods(crow::HTTPMethod::Post)([this](const crow::request& req){
    auto x = crow::json::load(req.body);
    if (!x) {
      return crow::response(400, crow::json::wvalue({{"error", "Invalid JSON"}}));
    }

    std::string token;
    try{
      token = x["token"].s();
    }catch(const std::exception& e){
      return crow::response(400, crow::json::wvalue({{"error", "invalid json key values"}}));
    }

    auto user = getLoggedInUser(token);
    if(!user){
      return crow::response(400, crow::json::wvalue({{"error", "invalid request"}}));
    }

    std::vector<std::pair<std::string, double>> result;
    try{
    result = conn.getVisitedRestaurants(user.value());
    }catch(const std::exception e){
      std::cerr << e.what() << std::endl;
      return crow::response(400, crow::json::wvalue({{"error", "visited error"}}));
    }
    crow::json::wvalue jsonResult;
    size_t i = 0;
    for (const auto& pair : result) {
      // Access the restaurant ID and rating from each pair
      jsonResult[i]["restaurant_id"] = pair.first;
      jsonResult[i]["rating"] = pair.second;
      ++i;
    }
    return crow::response(200, jsonResult);

  });


  CROW_ROUTE(app, "/rate").methods(crow::HTTPMethod::Post)([this](const crow::request& req){
    auto x = crow::json::load(req.body);
    if (!x) {
      return crow::response(400, crow::json::wvalue({{"error", "Invalid JSON"}}));
    }

    std::string rid;
    double rating;
    std::string token;
    try{
      rid = x["rid"].s();
      rating = x["rating"].d();
      token = x["token"].s();
    }catch(const std::exception& e){
      return crow::response(400, crow::json::wvalue({{"error", "invalid json key values"}}));
    }

    auto user = getLoggedInUser(token);
    if(!user){
      return crow::response(400, crow::json::wvalue({{"error", "invalid request"}}));
    }

    auto val = conn.moveRestaurantToVisited(user.value(), rid, rating);

    if(val == 0) return crow::response(200);
    return crow::response(400);

  });




















}

void Server::run(){
  app.port(port).run();
  //react double calling will make this a pain
  //app.port(port).multithreaded().run();
}

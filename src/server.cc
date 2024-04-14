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
      if (users.find(token) != users.end()) {
        users.erase(token);
      }
      return crow::response(200, crow::json::wvalue({{"success", "account deleted"}}));
    }

    return crow::response(400, crow::json::wvalue({{"error", "could not delete account"}}));
  });

  //int removeUserByUsername(const std::string& username);

}

void Server::run(){
  app.port(port).multithreaded().run();
}

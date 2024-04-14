#ifndef __SERVER_H__
#define __SERVER_H__

#include <crow.h>
#include <mongo.h>
#include <map>

class Server{
  public:
Server(int server_port);
  ~Server() = default;
  void setupRoutes();
  void run();
  std::optional<std::string> getLoggedInUser(const std::string& token);
private:
  int port;
  crow::SimpleApp app;
  Mongo conn;
  std::map<std::string, std::string> users;

  std::string genToken();

};

#endif

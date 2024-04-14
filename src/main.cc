#include <mongo.h>
#include <cstdlib>
#include <populate_retaurants.h>
#include <server.h>

int main(){
  //Mongo mongo(std::getenv("URI"), "foodr");
  //std::cout << mongo.addUser("eollich", "asdf") << std::endl;;
  //std::cout << mongo.addUser("nat", "123") << std::endl;;
  //mongo.addFriendByUsername("eollich", "nat");
  //std::cout << mongo.removeUserByUsername("eollich") << std::endl;
  //mongo.addRestaurant("LaPrima", "Silly Place", 4, 4.8, "www.laprima.com");
  //mongo.addFriendByUsername("nat", "eollich");
  //mongo.addRestaurantToUserList("nat", "LaPrima", 3.4);
  //mongo.removeRestaurantFromUserList("nat", "LaPrima");
  //mongo.removeFriendByUsername("nat", "eollich");






  //populate("json_data/", 201);


  Server server(60000);
  server.setupRoutes();
  server.run();

 // crow::SimpleApp app;

 // CROW_ROUTE(app, "/")([](){
 //   return "Hello world";
 // });

 // app.port(18080).run();

  return 0;
}

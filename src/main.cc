#include <mongo.h>
#include <cstdlib>
#include <populate_retaurants.h>

int main(){
  Mongo mongo(std::getenv("URI"), "foodr");
  //std::cout << mongo.addUser("eollich", "asdf") << std::endl;;
  //std::cout << mongo.addUser("nat", "123") << std::endl;;
  //mongo.addFriendByUsername("eollich", "nat");
  //std::cout << mongo.removeUserByUsername("eollich") << std::endl;
  //mongo.addRestaurant("LaPrima", "Silly Place", 4, 4.8, "www.laprima.com");
  //mongo.addFriendByUsername("nat", "eollich");
  //mongo.addRestaurantToUserList("nat", "LaPrima", 3.4);
  //mongo.removeRestaurantFromUserList("nat", "LaPrima");
  //mongo.removeFriendByUsername("nat", "eollich");






  populate("json_data/", 201);




  return 0;
}

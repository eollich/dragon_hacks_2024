#include <populate_retaurants.h>
#include <fstream>
#include <string>
#include <vector>

struct restaurant{
  std::string name;
  double rating;
  double price;
  std::string menu_url;
  std::vector<std::string> tags;
};


void populate(const std::string& dirpath, int num_restaurants){
  Mongo mongo(std::getenv("URI"), "foodr");

  for(int i = 0; i < num_restaurants; i++) {
    std::string file_path = dirpath + "data" + std::to_string(i + 1) + ".json";
    std::ifstream f(file_path);
    if (!f) {
      std::cerr << "Error opening file: " << file_path << std::endl;
      continue;  // Skip to the next file
    }

    nlohmann::json raw;
    try {
      f >> raw;
    } catch (const nlohmann::json::parse_error& e) {
      std::cerr << "JSON parsing error in file: " << file_path << ", error: " << e.what() << std::endl;
      continue;
    }

    if (!raw.contains("data") || !raw["data"].is_object() || !raw["data"].contains("data") || !raw["data"]["data"].is_array()) {
      std::cerr << "Invalid or missing 'data' structure in: " << file_path << std::endl;
      continue;
    }

    const auto& entries = raw["data"]["data"];
    for (const auto& val : entries) {
      std::string name = val.value("name", "Unknown");
      std::cout << name << std::endl;

      double rating = val.value("averageRating", 0.0);
      std::cout << rating << std::endl;

      std::string price_tag = (*val.find("priceTag")==nullptr) ? "" : val.value("priceTag", "");
      double price = std::count(price_tag.begin(), price_tag.end(), '$');
      if (price_tag.find('-') != std::string::npos) {
        price /= 2;
      }

      std::vector<std::string> tags_vect = val.value("establishmentTypeAndCuisineTags", std::vector<std::string>{});
      for (const auto& tag : tags_vect) {
        std::cout << tag << " ";
      }
      std::cout << price << std::endl;

      std::string menu_url = (*val.find("menuUrl")==nullptr) ? "NA" : val.value("menuUrl", "NA");
      std::cout << menu_url << std::endl;

      std::string img_path = val.value("heroImgUrl", "NA");
      std::cout << img_path << std::endl;


      std::cout << std::endl;
      mongo.addRestaurant(name, "Philadelphia", price, rating, menu_url, img_path, tags_vect);
    }
  }
}

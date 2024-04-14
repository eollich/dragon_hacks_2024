#ifndef __POPULATE_RESTAURANT_H__ 
#define __POPULATE_RESTAURANT_H__

#include <mongo.h>
#include <json.hpp>

void populate(const std::string& dirpath, int num_restaurants);

#endif

#!/bin/bash

# Directory where JSON files will be saved
directory="json_data"

# Create the directory if it does not exist
mkdir -p "$directory"

# Check if the TRIPADVISOR_API_KEY environment variable is set
if [ -z "$TRIPADVISOR_API_KEY" ]; then
    echo "Error: TRIPADVISOR_API_KEY environment variable is not set."
    exit 1
fi

# Loop through page numbers from 1 to 201
for i in $(seq 1 201); do
    # Use curl to make the request and save the output
    curl --request GET \
        --url "https://tripadvisor16.p.rapidapi.com/api/v1/restaurant/searchRestaurants?locationId=60795&page=$i" \
        --header "X-RapidAPI-Host: tripadvisor16.p.rapidapi.com" \
        --header "X-RapidAPI-Key: $TRIPADVISOR_API_KEY" \
        --silent --output "$directory/data$i.json"
done

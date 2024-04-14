import React, { useState, useEffect } from 'react';
import { useParams } from 'react-router-dom'; // Import useParams
import { view_v, view_w, getRestaurant } from '../api'; // Adjust API import
import { useAuth } from '../AuthContext';
import { useNavigate } from 'react-router-dom';
import ExternalCard from './ExternalCard'; // Assuming this is your card component

function ExternalRestaurant({ type }) {
  const { isAuthenticated } = useAuth();
  const navigate = useNavigate();
  const [restaurants, setRestaurants] = useState([]);
  const { username } = useParams(); // Get the username from URL

  useEffect(() => {
    if (!isAuthenticated && !localStorage.getItem('token')) {
      navigate('/signin'); 
    } else {
      fetchRestaurants();
    }
  }, [isAuthenticated, navigate, username, type]); // Include username and type dependencies

  const fetchRestaurants = async () => {
    try {
      let data;
      if (type === "v") {
        data = await view_v(username);
      } else if (type === "w") {
        data = await view_w(username);
      }
      const restaurantPromises = data.map(async (item) => {
        const details = await getRestaurant(item.restaurant_id || item.value);
        return {
          ...details,
          userRating: item.rating || 0
        };
      });
      const restaurantsDetails = await Promise.all(restaurantPromises);
      setRestaurants(restaurantsDetails);
    } catch (error) {
      console.error('Failed to fetch restaurants:', error);
    }
  };

  return (
    <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(250px, 1fr))', gap: '20px', padding: '20px' }}>
      {restaurants.length > 0 ? restaurants.map(restaurant => (
        <ExternalCard key={restaurant.restaurant_id} restaurant={restaurant} />
      )) : (
        <h1>No restaurants found for {type === "v" ? "visited" : "want to visit"}</h1>
      )}
    </div>
  );
}

export default ExternalRestaurant;

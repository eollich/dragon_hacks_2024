import React, { useState, useEffect } from 'react';
import { getVisited, getRestaurant} from '../api'; 
import { useAuth } from '../AuthContext';
import { useNavigate } from 'react-router-dom';
import VisitedRestaurantCard from './VisitedRestaurantCard';

function VisitedRestaurants() {
  const { isAuthenticated } = useAuth();
  const navigate = useNavigate();
  const [restaurants, setRestaurants] = useState([]);

  useEffect(() => {
    if (!isAuthenticated && !localStorage.getItem('token')) {
      navigate('/signin'); 
    } else {
      fetchRestaurants();
    }
  }, [isAuthenticated, navigate]);

const fetchRestaurants = async () => {
  try {
    const token = localStorage.getItem('token');
    const visitedData = await getVisited(token);
    const restaurantPromises = visitedData.map(async (item) => {
      const details = await getRestaurant(item.restaurant_id); // Fetch details for each restaurant ID
      return {
        ...details,        // Spread all fetched details
        userRating: item.rating  // Add the user-specific rating to the details
      };
    });
    const restaurantsDetails = await Promise.all(restaurantPromises);
    setRestaurants(restaurantsDetails);  // Set the combined data to state
  } catch (error) {
    console.error('Failed to fetch restaurants:', error);
  }
};

  return (
    <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(250px, 1fr))', gap: '20px', padding: '20px' }}>
      {restaurants.length > 0 ? restaurants.map(restaurant => (
        <VisitedRestaurantCard key={restaurant.restaurant_id} restaurant={restaurant} />
      )) : (
        <h1>No visited restaurants found</h1>
      )}
    </div>
  );
}

export default VisitedRestaurants;

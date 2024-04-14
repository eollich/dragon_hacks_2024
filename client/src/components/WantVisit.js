import React, { useState, useEffect } from 'react';
import { getWant, getRestaurant } from '../api'; // Adjust API calls as needed
import { useAuth } from '../AuthContext';
import { useNavigate } from 'react-router-dom';
import VisitRestaurantCard from './VisitRestaurantCard';

function WantToVisit() {
  const { isAuthenticated } = useAuth();
  const navigate = useNavigate();
  const [restaurants, setRestaurants] = useState([]);

  useEffect(() => {
    if (!isAuthenticated && !localStorage.getItem('token')) {
      navigate('/signin'); // Redirect if not authenticated
    } else {
      fetchRestaurants();
    }
  }, [isAuthenticated, navigate]);

  const fetchRestaurants = async () => {
    try {
      console.log(localStorage.getItem('token'));
      const ids = await getWant(localStorage.getItem('token'));
      const promises = ids.map(id => getRestaurant(id["value"]));
      const restaurantDetails = await Promise.all(promises);
      // Adjust the handling of IDs to extract the string from the $oid structure
      const adjustedRestaurants = restaurantDetails.map(restaurant => ({
        ...restaurant,
        id: restaurant._id.$oid // Assuming the ID is provided as { $oid: "string" }
      }));
      setRestaurants(adjustedRestaurants);
    } catch (error) {
      console.error('Failed to fetch restaurants:', error);
    }
  };

  return (
    <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(250px, 1fr))', gap: '20px', padding: '20px' }}>
      {restaurants.length > 0 ? restaurants.map(restaurant => (
        <VisitRestaurantCard key={restaurant.id} restaurant={restaurant} restaurantId={restaurant.id} />
      )) : (
        <h1>No restaurants to visit</h1>
      )}
    </div>
  );
}

export default WantToVisit;

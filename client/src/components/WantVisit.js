import React, { useState, useEffect } from 'react';
import { getWant, getRestaurant } from '../api'; // Adjust API if needed
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
      setRestaurants(restaurantDetails);
    } catch (error) {
      console.error('Failed to fetch restaurants:', error);
    }
  };

  return (
    <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(250px, 1fr))', gap: '20px', padding: '20px' }}>
      {restaurants.length > 0 ? restaurants.map(restaurant => (
        <VisitRestaurantCard key={restaurant.id} restaurant={restaurant} />
      )) : (
        <h1>No restaurants to visit</h1>
      )}
    </div>
  );
}

export default WantToVisit;

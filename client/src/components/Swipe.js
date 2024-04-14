import React, { useState, useEffect } from 'react';
import { getSwipeRestaurants, getRestaurant, like, dislike } from '../api'; 
import { useAuth } from '../AuthContext'; 
import { useNavigate } from 'react-router-dom';
import RestaurantCard from './RestaurantCard';

function Swipe() {
  const { isAuthenticated } = useAuth();
  const navigate = useNavigate();
  const [restaurants, setRestaurants] = useState([]); // This will store an array of IDs
  const [currentIndex, setCurrentIndex] = useState(0); // This tracks the currently displayed restaurant
  const [currentRestaurant, setCurrentRestaurant] = useState(null); // Details of the current restaurant

  useEffect(() => {
    if (!isAuthenticated && !localStorage.getItem('token')) {
      navigate('/signin');
    } else {
      fetchRestaurants();
    }
  }, [isAuthenticated, navigate]);

  const fetchRestaurants = async () => {
    try {
      const ids = await getSwipeRestaurants(localStorage.getItem('token'));
      setRestaurants(ids);
      if (ids.length > 0) {
        fetchRestaurantDetails(ids[0]["value"]); // Fetch details of the first restaurant
      }
    } catch (error) {
      console.error('Failed to fetch swipe restaurants:', error);
    }
  };

  const fetchRestaurantDetails = async (id) => {
    try {
      const details = await getRestaurant(id);
      setCurrentRestaurant(details);
      console.log('cur')
      console.log(details)
    } catch (error) {
      console.error('Failed to fetch restaurant details:', error);
    }
  };

  const handleLike = async () => {
    if (currentRestaurant) {
      await like(restaurants[currentIndex]["value"], localStorage.getItem('token'));
      moveToNextRestaurant();
    }
  };

  const handleDislike = async () => {
    if (currentRestaurant) {
      await dislike(currentRestaurant.id);
      moveToNextRestaurant();
    }
  };

  const moveToNextRestaurant = () => {
    const nextIndex = currentIndex + 1;
    if (nextIndex < restaurants.length) {
      fetchRestaurantDetails(restaurants[nextIndex]["value"]);
      setCurrentIndex(nextIndex);
    } else {
      console.log('No more restaurants to swipe!');
      // Optionally reset or fetch more restaurants
      setCurrentRestaurant(null);
    }
  };

  return (
    <div>
      {currentRestaurant ? (
        <RestaurantCard
          restaurant={currentRestaurant}
          onLike={handleLike}
          onDislike={handleDislike}
        />
      ) : (
        <h1>No restaurants available</h1>
      )}
    </div>
  );
}

export default Swipe;

import { rate } from '../api';
import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom'; 

function VisitRestaurantCard({ restaurant, restaurantId  }) {
  const [rating, setRating] = useState('');
  const navigate = useNavigate(); 

  const handleRatingChange = (event) => {
    const inputRating = parseFloat(event.target.value);
    if (inputRating >= 1 && inputRating <= 5) {
      setRating(event.target.value); 
    }
  };

  const handleRatingSubmit = async (event) => {
    if (event.key === 'Enter' && rating) { 
      event.preventDefault();
      let numericRating = parseFloat(rating);
      if(numericRating > 5){
        numericRating = 5;
      }else if(numericRating<1){numericRating = 1;}
      if (numericRating >= 1 && numericRating <= 5) {
        try {
          const token = localStorage.getItem('token');
          await rate(restaurantId, numericRating, token); 
          console.log('Rating submitted:', numericRating);
          navigate('/visited'); 
        } catch (error) {
          console.error('Failed to submit rating:', error);
        }
      } else {
        console.error('Rating out of allowed range (1-5)');
      }
    }
  };

  return (
    <div style={{ border: '1px solid #ccc', padding: '10px', borderRadius: '8px' }}>
      <img src={restaurant.image_url} alt={restaurant.name} style={{ width: '100%', height: '200px', objectFit: 'cover' }} />
      <h3>{restaurant.name}</h3>
      <p>Rating: {restaurant.stars}</p>
      <p>Cost: {restaurant.price_range}</p>
      <p>{restaurant.tags.join(', ')}</p>
      <p>Website: <a href={restaurant.website_url} target="_blank" rel="noopener noreferrer">{restaurant.website_url}</a></p>
      <input
        type="number"
        value={rating}
        onChange={handleRatingChange}
        onKeyPress={handleRatingSubmit}
        placeholder="Enter your rating (1-5)"
        style={{ width: '100%', padding: '8px', margin: '10px 0' }}
        min="1"
        max="5"
        step="0.1"
      />
    </div>
  );
}

export default VisitRestaurantCard;

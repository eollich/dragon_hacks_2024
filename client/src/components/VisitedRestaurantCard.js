import React from 'react';


function VisitedRestaurantCard({ restaurant }) {
  return (
    <div style={{ border: '1px solid #ccc', padding: '10px', borderRadius: '8px', marginBottom: '10px' }}>
      <img src={restaurant.image_url} alt={restaurant.name} 
    style={{ width: '100%', height: '200px', objectFit: 'cover' }} />
      <h3>{restaurant.name}</h3>
      <p>Location: {restaurant.location}</p>
      <p>{restaurant.tags.join(', ')}</p>
      <p>Website: <a href={restaurant.website_url} target="_blank" rel="noopener noreferrer">{restaurant.website_url}</a></p>
      <p>Price Range: {restaurant.price_range}</p>
      <p>Rating: {restaurant.rating}</p>
      <p>User Rating: {restaurant.userRating}</p> {/* Display the user's rating */}
    </div>
  );
}



export default VisitedRestaurantCard;

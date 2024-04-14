function RestaurantCard({ restaurant, onLike, onDislike }) {
  console.log(restaurant);
  return (
    <div>
      <h2>{restaurant.name}</h2>
      <img src={restaurant.image_url} alt={restaurant.name} style={{ width: '200px', height: '200px', objectFit: 'cover' }}/>
      <p>Rating: {restaurant.stars}</p>
      <p>Cost: {restaurant.price_range}</p>
      <p>{restaurant.tags.join(', ')}</p>
      <a href={restaurant.website_url} target="_blank" rel="noopener noreferrer">
        {restaurant.website_url}
      </a>
    <div></div>
      <button onClick={onLike}>Like</button>
      <button onClick={onDislike}>Don't Like</button>
    </div>
  );
}

export default RestaurantCard;

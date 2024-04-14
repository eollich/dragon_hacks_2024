function RestaurantCard({ restaurant, onLike, onDislike }) {
  console.log(restaurant);
  return (
    <div>
      <h2>{restaurant.name}</h2>
      <img src={restaurant.image_url} alt={restaurant.name} />
      <p>Rating: {restaurant.stars}</p>
      <p>Cost: {restaurant.price_range}</p>
      <button onClick={onLike}>Like</button>
      //<button onClick={onLike}>Visited</button>
      <button onClick={onDislike}>Don't Like</button>
    </div>
  );
}

export default RestaurantCard;

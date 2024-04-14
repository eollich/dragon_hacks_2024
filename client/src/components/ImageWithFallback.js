import React from 'react';

const ImageWithFallback = ({ src, alt, fallback, style }) => {
  const handleError = ({ currentTarget }) => {
    currentTarget.onerror = null; // Prevents looping
    currentTarget.src = fallback; // Sets fallback image
  };

  return (
    <img
      src={src}
      alt={alt}
      onError={handleError}
      style={style}
    />
  );
};

export default ImageWithFallback;


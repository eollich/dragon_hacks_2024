import React, { useState, useEffect } from 'react';
import { login } from '../api'; 
import { useAuth } from '../AuthContext'; 
import { useNavigate } from 'react-router-dom';

const Login = () => {
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [error, setError] = useState('');
  const { isAuthenticated, setIsAuthenticated } = useAuth();

  const navigate = useNavigate();

  // Check if the user is already authenticated
  useEffect(() => {
    if (isAuthenticated) {
      navigate('/'); // Redirect to home if already logged in
    }
  }, [isAuthenticated, navigate]);

  const handleLogin = async (event) => {
    event.preventDefault();
    try {
      const success = await login(username, password);
      if (success) { // Ensure success.token is valid
        localStorage.setItem('token', success);
        setIsAuthenticated(true);
        navigate('/friends'); // Use navigate for SPA behavior without reloading
      } else {
        setError('Login failed, please check your credentials.');
      }
    } catch (error) {
      setError('An error occurred during login.');
      console.error('Login error:', error);
    }
  };

  return (
    <div>
      <h1>Login</h1>
      <form onSubmit={handleLogin}>
        <div>
          <label>Username:</label>
          <input
            type="text"
            value={username}
            onChange={e => setUsername(e.target.value)}
            required
          />
        </div>
        <div>
          <label>Password:</label>
          <input
            type="password"
            value={password}
            onChange={e => setPassword(e.target.value)}
            required
          />
        </div>
        <button type="submit">Login</button>
        {error && <p>{error}</p>}
      </form>
    </div>
  );
};

export default Login;

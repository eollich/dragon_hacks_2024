import React, { useState } from 'react';
import { login } from './api'; // adjust the path as necessary

const Login = () => {
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [error, setError] = useState('');

  const handleLogin = async (event) => {
    event.preventDefault();
    try {
      const success = await login(username, password);
      if (success) {
        localStorage.setItem('token', success.token); // Assuming token is part of the success response
        alert('Login successful!');
        // Optionally redirect the user or perform other actions
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

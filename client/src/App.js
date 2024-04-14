import React from 'react';
import '@coreui/coreui/dist/css/coreui.min.css';
import {
  BrowserRouter as Router,
  Routes,
  Route
} from 'react-router-dom';

import Navbar from './components/Navbar';
import Home from './components/Home';
import Friends from './components/Friends';
import Swipe from './components/Swipe';
import SignIn from './components/SignIn';
import Register from './components/Register';
import WantToVisit from './components/WantVisit';
import VisitedRestaurants from './components/VisitedRestaurants';

function App() {
  return (
    <Router>
      <div>
        <Navbar />
        <Routes>
          <Route path="/" element={<Home />} />
          <Route path="/friends" element={<Friends />} />
          <Route path="/swipe" element={<Swipe />} />
          <Route path="/signin" element={<SignIn />} />
          <Route path="/register" element={<Register />} />
          <Route path="/want" element={<WantToVisit />} />
          <Route path="/visited" element={<VisitedRestaurants />} />
        </Routes>
      </div>
    </Router>
  );
}

export default App;

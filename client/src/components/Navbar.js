import React, { useState } from 'react';
import {
  CNavbar,
  CContainer,
  CNavbarBrand,
  CNavbarToggler,
  CCollapse,
  CNavbarNav,
  CNavItem,
  CNavLink,
  CButton
} from '@coreui/react';
import { useAuth } from '../AuthContext';  
import { deleteUser } from '../api';

const Navbar = () => {
  const [visible, setVisible] = useState(false);
  const { isAuthenticated, setIsAuthenticated } = useAuth(); 

  const handleLogout = () => {
    localStorage.removeItem('token');
    setIsAuthenticated(false);
    window.location.href = '/signin'; // Redirect to the sign-in page after logout
  };

  const handleDeleteAccount = async () => {
    const token = localStorage.getItem('token');
    try {
      const deleted = await deleteUser(token);
      if (deleted) {
        alert('Account successfully deleted.');
        handleLogout(); // Log out the user after account deletion
      } else {
        alert('Failed to delete account.');
      }
    } catch (error) {
      console.error('Error deleting account:', error);
      alert('Error occurred during account deletion.');
    }
  };

  return (
    <CNavbar expand="lg" colorScheme="dark" className="bg-dark">
      <CContainer fluid>
        <CNavbarBrand href="#">FOODR</CNavbarBrand>
        <CNavbarToggler
          aria-label="Toggle navigation"
          aria-expanded={visible}
          onClick={() => setVisible(!visible)}
        />
        <CCollapse className="navbar-collapse" visible={visible}>
          <CNavbarNav>
            {isAuthenticated ? (
              <>
                <CNavItem>
                  <CNavLink href="/friends">Friends</CNavLink>
                </CNavItem>
                <CNavItem>
                  <CNavLink href="/swipe">Swipe</CNavLink>
                </CNavItem>
              <CNavItem>
                  <CButton onClick={handleDeleteAccount} color="danger">Delete Account</CButton>
                </CNavItem>
                <CNavItem>
                  <CButton onClick={handleLogout}>Logout</CButton>
                </CNavItem>
              </>
            ) : (
              <>
                <CNavItem>
                  <CNavLink href="/signin">Sign In</CNavLink>
                </CNavItem>
                <CNavItem>
                  <CNavLink href="/register">Register</CNavLink>
                </CNavItem>
              </>
            )}
          </CNavbarNav>
        </CCollapse>
      </CContainer>
    </CNavbar>
  );
};

export default Navbar;

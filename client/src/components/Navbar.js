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

const Navbar = () => {
  const [visible, setVisible] = useState(false);
  const isAuthenticated = localStorage.getItem('token'); // Simple auth check

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
                  <CButton onClick={() => {
                    localStorage.removeItem('token');
                    window.location.href = '/'; // Redirect to home or login page
                  }}>Logout</CButton>
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

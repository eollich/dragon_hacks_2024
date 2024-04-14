import axios from 'axios';

const getHome = async() => {
  try{
    const res = await axios.get("http://localhost:60000/");
    console.log('Status:', res.status);
    console.log('Data:', res.data);
    return res.data; 
  }catch(error){
    console.error('Error:', error.res ? error.res.data : error.message);
    throw error; 
  }

};

export const createUser = async(username, password) => {
  try {
    const res = await axios.post('/createUser', {username: username, password:password}, {
      headers: {
        'Content-Type': 'application/json'
      }
    });
    console.log('Status:', res.status);
    console.log('Data:', res.data);
    if(res.status == 200){
      return true;
    }
    return false;
  } catch (error) {
    console.error('Error:', error.res ? error.res.data : error.message);
    throw error; 
  }
};


export const deleteUser = async (token) => {
  try {
    const res = await axios.post('/deleteUser', {
      token: token
    }, {
      headers: {
        'Content-Type': 'application/json'
      }
    });
    console.log('Status:', res.status);
    console.log('Data:', res.data);
    return res.data; 
  } catch (error) {
    console.error('Error:', error.res ? error.res.data : error.message);
    throw error; 
  }
}



export const login = async(username, password) => {
  try {
    const res = await axios.post('/login', {username: username, password:password}, {
      headers: {
        'Content-Type': 'application/json'
      }
    });
    console.log('Status:', res.status);
    console.log('Data:', res.data);
    if(res.status==200){
      return res.data["success"]; 
    }
    return null;
  } catch (error) {
    console.error('Error:', error.res ? error.res.data : error.message);
    throw error; 
  }
};

export const getSwipeRestaurants = async(token) => {
  try {
    const res = await axios.post('/getSwipeRestaurants', {token: token}, {
      headers: {
        'Content-Type': 'application/json'
      }
    });
    console.log('Status:', res.status);
    console.log('Data:', res.data);
    if(res.status==200){
      return res.data; 
    }
    return null;
  } catch (error) {
    console.error('Error:', error.res ? error.res.data : error.message);
    throw error; 
  }
};

export const getRestaurant = async(rid) => {
  try {
    const res = await axios.post('/getRestaurant', {rid: rid}, {
      headers: {
        'Content-Type': 'application/json'
      }
    });
    console.log('Status:', res.status);
    console.log('Data:', res.data);
    if(res.status==200){
      return res.data;; 
    }
    return null;
  } catch (error) {
    console.error('Error:', error.res ? error.res.data : error.message);
    throw error; 
  }
};

export const like = async(rid,token) => {
  try {
    console.log(rid);
    console.log(token);
    const res = await axios.post('/likeR', {rid: rid, token:token}, {
      headers: {
        'Content-Type': 'application/json'
      }
    });
    console.log('Status:', res.status);
    console.log('Data:', res.data);
    return res.status;
  } catch (error) {
    console.error('Error:', error.res ? error.res.data : error.message);
    throw error; 
  }
};

export const dislike = async(rid) => {};


export const getWant = async(token) => {
  try {
    const res = await axios.post('/getWantVisit', {token: token}, {
      headers: {
        'Content-Type': 'application/json'
      }
    });
    console.log('Status:', res.status);
    console.log('Data:', res.data);
    if(res.status==200){
      return res.data;; 
    }
    return null;
  } catch (error) {
    console.error('Error:', error.res ? error.res.data : error.message);
    throw error; 
  }
};


export const getVisited = async(token) => {
  try {
    const res = await axios.post('/getHaveVisit', {token: token}, {
      headers: {
        'Content-Type': 'application/json'
      }
    });
    console.log('Status:', res.status);
    console.log('Data:', res.data);
    if(res.status==200){
      return res.data;; 
    }
    return null;
  } catch (error) {
    console.error('Error:', error.res ? error.res.data : error.message);
    throw error; 
  }
};

export const rate = async(rid,rating, token) => {
  try {
    console.log(rid);
    console.log(rating);
    console.log(token);
    const res = await axios.post('/rate', {rid: rid, rating: rating, token:token}, {
      headers: {
        'Content-Type': 'application/json'
      }
    });
    console.log('Status:', res.status);
    return res.status;
  } catch (error) {
    console.error('Error:', error.res ? error.res.data : error.message);
    throw error; 
  }
};







//(async () => {
//  let token;
//  let username = "t1";
//  let password = "asdf";
//
//  await getHome();
//
//  //token = await login(username, password);
//  //console.log(token);
//
//
//  
//
//  //let val = await getHome();
//  //console.log(val);
//
//
//  //val = await createUser("t1", "asdf");
//  //console.log(val);
//
//})();

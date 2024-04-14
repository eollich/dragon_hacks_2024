import axios from 'axios';
const getHome = async() => {
  try{
    const res = await axios.get("http://localhost:60000/");
    //console.log('Status:', res.status);
    //console.log('Data:', res.data);
    return res.data; 
  }catch(error){
    console.error('Error:', error.res ? error.res.data : error.message);
    throw error; 
  }

};

const createUser = async(username, password) => {
  try {
    const res = await axios.post('http://localhost:60000/createUser', {username: username, password:password}, {
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
};


const deleteUser = async (token) => {
  try {
    const res = await axios.post('http://localhost:60000/deleteUser', {
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

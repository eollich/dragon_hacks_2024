const axios = require('axios');

axios.get("http://localhost:60000/").then((response) => {
  console.log(response.data);
}).catch((error) => {
  console.error("Error fetching data:", error);
});

console.log("starting")

db.createUser(
  {
   user : $USER,
   pwd  : $PASSWORD
   roles : [
     {
      role : "admin",
      db : "esp-plant-watering"
     }
   ]
  }
 )

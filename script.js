function getAtmosScore(temp, hum) {
  let score = 100 -
    (Math.abs(temp - 24) * 2.3) -
    (Math.abs(hum - 50) * 0.9);
  
  if (score < 0) score = 0;
  if (score > 100) score = 100;
  
  return Math.round(score);
}

const dataRef = db.ref("DATA");

// Step 1: get the latest date folder
dataRef.orderByKey().limitToLast(1).once("value")
  .then(snapshot => {
    snapshot.forEach(dateSnap => {
      const latestDate = dateSnap.key;
      
      // Step 2: get the last reading inside that date
      db.ref(`DATA/${latestDate}`)
        .orderByKey()
        .limitToLast(1)
        .once("value")
        .then(innerSnap => {
          
          innerSnap.forEach(entrySnap => {
            const item = entrySnap.val();
            
            // Update UI
            document.getElementById("tempValue").innerHTML = `${item.temp}°C`;
            document.getElementById("humValue").innerHTML = `${item.hum}%`;
            
            const score = getAtmosScore(item.temp, item.hum);
            document.getElementById("scoreValue").innerHTML = score;
          });
          
        });
      
    });
  })
  .catch(error => {
    console.error(error);
  });
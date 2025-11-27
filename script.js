function getAtmosScore(temp, hum) {
  let score = 100 -
    (Math.abs(temp - 24) * 2.3) -
    (Math.abs(hum - 50) * 0.9);
  
  if (score < 0) score = 0;
  if (score > 100) score = 100;
  
  return Math.round(score);
}

const dataRef = db.ref("DATA");

// Function to call AI endpoint
async function generateEnvSummary(temp, hum, score) {
  try {
    const prompt = `The current indoor temperature is ${temp}°C and humidity is ${hum}%. The Atmos Score is ${score}. 
    Give a short friendly summary of how the environment is and any tips for comfort.`;
    
    const res = await fetch("/api/ai-key", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ prompt })
    });
    
    const data = await res.json();
    
    if (data.result) {
      document.getElementById("aiResponse").innerHTML = data.result;
    } else {
      document.getElementById("aiResponse").innerHTML = "Unable to get AI summary.";
    }
  } catch (err) {
    console.error(err);
    document.getElementById("aiResponse").innerHTML = "Error fetching AI summary.";
  }
}

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
            
            // Send to AI endpoint
            generateEnvSummary(item.temp, item.hum, score);
          });
          
        });
      
    });
  })
  .catch(error => {
    console.error(error);
    document.getElementById("aiResponse").innerHTML = "Failed to fetch sensor data.";
  });
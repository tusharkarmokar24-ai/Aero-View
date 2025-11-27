
  const firebaseConfig = {
    apiKey: "AIzaSyCbR32g7I54-bK3B-gmLx2yGCKwKvW-Uzw",
    authDomain: "aeroview-7a470.firebaseapp.com",
    databaseURL: "https://aeroview-7a470-default-rtdb.firebaseio.com",
    projectId: "aeroview-7a470",
    storageBucket: "aeroview-7a470.firebasestorage.app",
    messagingSenderId: "746491548874",
    appId: "1:746491548874:web:2cf336cc072c6fe89c5033",
    measurementId: "G-MFFZWJ1SHT"
  };

  // Initialize Firebase
  const app = firebase.initializeApp(firebaseConfig);
  const db = firebase.database(app);
  

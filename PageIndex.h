const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" rel="stylesheet">
    <style>
      html {
        font-family: Arial;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
      }
      h1 { font-size: 2.0rem; }
      p { font-size: 2.0rem; }
      .units { font-size: 1.2rem; }
      .dht-labels{
        font-size: 1.5rem;
        vertical-align:middle;
        padding-bottom: 15px;
      }
    </style>
  </head>
  <body>
    <h1>Environment Monitoring System</h1>
    <p>
      <span class="dht-labels">Temperature : </span> 
      <span id="TemperatureValue">0</span>
      <sup class="units">&deg;C</sup>
    </p>
    <p>
      <span class="dht-labels">Humidity : </span>
      <span id="HumidityValue">0</span>
      <sup class="units">%</sup>
    </p>
    <p>
      <span class="dht-labels">Pressure : </span>
      <span id="PressureValue">0</span>
      <sup class="units">Pa</sup>
    </p>
    <p>
      <span class="dht-labels">Rain : </span>
      <span id="RainValue">0</span>
      <sup class="units">%</sup>
    </p>
    <p>
      <span class="dht-labels">Altitude : </span>
      <span id="AltitudeValue">0</span>
      <sup class="units">m</sup>
    </p>
    <p>
      <i class="far fa-clock" style="font-size:1.0rem;color:#e3a8c7;"></i>
      <span style="font-size:1.0rem;">Time </span>
      <span id="time" style="font-size:1.0rem;"></span>
      
      <i class="far fa-calendar-alt" style="font-size:1.0rem;color:#f7dc68";></i>
      <span style="font-size:1.0rem;">Date </span>
      <span id="date" style="font-size:1.0rem;"></span>
    </P>
  <p>
    <style="font-size:1.0rem;color:red;"></i>
    <a href="https://docs.google.com/spreadsheets/d/1WZCQhjvzktvAOUlzxoFPSAMuRfAYjuLEpblb20CSIDA/edit#gid=0" target="_blank" style="font-size:1.0rem;">Google sheet</a>
  </P>
    <script>
      setInterval(function() {
        // Call a function repetatively with 2 Second interval
        getTemperatureData();
        getHumidityData();
        getPressureData();
        getRainData();
        getAltitudeData();
      }, 2000); 
      
      setInterval(function() {
        // Call a function repetatively with 1 Second interval
        Time_Date();
      }, 1000); 

      function getTemperatureData() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("TemperatureValue").innerHTML =
            this.responseText;
          }
        };
        xhttp.open("GET", "readTemperature", true);
        xhttp.send();
      }
      function getHumidityData() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
          document.getElementById("HumidityValue").innerHTML =
          this.responseText;
          }
        };
        xhttp.open("GET", "readHumidity", true);
        xhttp.send();
      }
      function getPressureData() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("PressureValue").innerHTML =
            this.responseText;
          }
        };
        xhttp.open("GET", "readPressure", true);
        xhttp.send();
      }
      function getRainData() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("RainValue").innerHTML =
            this.responseText;
          }
        };
        xhttp.open("GET", "readRain", true);
        xhttp.send();
      }
      function getAltitudeData() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("AltitudeValue").innerHTML =
            this.responseText;
          }
        };
        xhttp.open("GET", "readAltitude", true);
        xhttp.send();
      }
      function Time_Date() {
        var t = new Date();
        document.getElementById("time").innerHTML = t.toLocaleTimeString();
        var d = new Date();
        const dayNames = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday","Saturday"];
        const monthNames = ["January", "February", "March", "April", "May", "June","July", "August", "September", "October", "November", "December"];
        document.getElementById("date").innerHTML = dayNames[d.getDay()] + ", " + d.getDate() + "-" + monthNames[d.getMonth()] + "-" + d.getFullYear();
      }
    </script>
  </body>
</html>
)=====";

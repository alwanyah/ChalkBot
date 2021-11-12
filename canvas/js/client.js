
function sendJSON(json, handler) {
  console.log(json);
  sendData(JSON.stringify(json), handler);
}

// sends data to robot
function sendData(data, handler) 
{
  console.log("[send] " + data);
  
  var xhr = new XMLHttpRequest();
  xhr.open("POST", "naoqi", true);
  
  xhr.onload = function (e) {
    if (xhr.readyState === 4) {
      if (xhr.status === 200) {
        console.log("[response] " + xhr.responseText);
        
        if(typeof handler !== "undefined") {
          handler(xhr.responseText);
        }
      } else {
        console.error(xhr.statusText);
      }
    }
  };
  
  xhr.onerror = function (e) {
    console.error(e);
    console.error(xhr.statusText);
    // also call the handler in case of error
    if(typeof handler !== "undefined") {
      handler(xhr.statusText);
    }
  };
  
  xhr.setRequestHeader("Content-Type", "application/json");
  xhr.send(data); 
}

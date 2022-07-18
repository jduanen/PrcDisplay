  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
  }
  function onOpen(event) {
    console.log('Connection opened');
    initView();
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onLoad(event) {
    initWebSocket();
  }
  function setLedMsg() {
    var fontNum = document.getElementById('fonts').value;
    var jsonMsg = JSON.stringify({"msgType": "ledMsg", "mode": "set", "fontNum": parseInt(fontNum), "text": document.getElementById('ledMessage').value});
    websocket.send(jsonMsg);
  }
  function appendLedMsg() {
    var fontNum = document.getElementById('fonts').value;
    var jsonMsg = JSON.stringify({"msgType": "ledMsg", "mode": "append", "fontNum": parseInt(fontNum), "text": document.getElementById('ledMessage').value});
    websocket.send(jsonMsg);
  }
  function onMessage(event) {
    var state;
    var elem;
    const msgObj = JSON.parse(event.data);
    console.log("msgObj: " + JSON.stringify(msgObj));

    elem = document.getElementById("ssid");
    elem.value = msgObj.ssid;

    elem = document.getElementById("password");
    if (msgObj.passwd != null) {
      elem.value = rot47(msgObj.passwd);
    } else {
      elem.value = "";
    }

    elem = document.getElementById('ledState');
    elem.innerHTML = msgObj.led;
    elem.checked = (msgObj.led == "ON");

    elem = document.getElementById('elState');
    elem.innerHTML = msgObj.el;
    elem.checked = (msgObj.el == "ON");

    var rs = (msgObj.randomSequence != 0);
    document.getElementById('randomSequence').checked = rs;
    document.getElementById('sequenceNumber').disabled = rs;

    elem = document.getElementById('ledMessage');
    elem.value = escapeHTML(msgObj.msg);

    elem = document.getElementById('sequenceNumber');
    elem.value = msgObj.sequenceNumber;

    elem = document.getElementById('sequenceSpeed');
    elem.value = msgObj.sequenceSpeed;
    document.getElementById("speed").innerHTML = msgObj.sequenceSpeed;

    document.getElementById("save").disabled = false;
  }
  function toggleCheckbox(element) {
    element.innerHTML = element.checked ? "ON" : "OFF";
    var jsonMsg = JSON.stringify({"msgType": element.id, "state": element.innerHTML});
    websocket.send(jsonMsg);
  }
  function setSequence() {
    var seqNum = document.getElementById("sequenceNumber").value;
    var seqSpeed = document.getElementById("sequenceSpeed").value;
    var jsonMsg = JSON.stringify({"msgType": "sequence", sequenceNumber: seqNum, sequenceSpeed: seqSpeed});
    websocket.send(jsonMsg);
  }
  function saveConfiguration() {
    var ssid = document.getElementById("ssid").value;
    var passwd = document.getElementById("password").value;
    var jsonMsg = JSON.stringify({"msgType": "saveConf", "ssid": ssid, "passwd": rot47(passwd)});
    document.getElementById("save").disabled = true;
    websocket.send(jsonMsg);
  }
  function toggleRandomSequence() {
    var randomSeq = document.getElementById("randomSequence").checked;
    document.getElementById("sequenceNumber").disabled = randomSeq;
    var jsonMsg = JSON.stringify({"msgType": "randomSequence", "state": randomSeq});
    websocket.send(jsonMsg);
  }
  function escapeHTML(s) {
    return s.replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/'/g, '&apos;')
        .replace(/"/g, '&quot;')
        .replace(/\//g, '&sol;');
  }
  function rot13(str) {
    return str.split('').map(char => String.fromCharCode(char.charCodeAt(0) + (char.toLowerCase() < 'n' ? 13 : -13))).join('');
  }
  function rot47(x) {
    var s = [];
    for(var i = 0; (i < x.length); i++) {
      var j = x.charCodeAt(i);
      if ((j >= 33) && (j <= 126)) {
        s[i] = String.fromCharCode(33 + ((j + 14) % 94));
      } else {
        s[i] = String.fromCharCode(j);
      }
    }
    return s.join('');
  }
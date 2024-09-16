#pragma once

#include <pgmspace.h>

const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML><html></html>
<head>
  <title>UG MOBILE</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">

  <style>
    html,body {min-height: 100%;}
    body {display: flex; flex-direction: column; margin: 0; padding: 0; justify-content: center; align-items: center; font-family: Arial, sans-serif; background-color: #2d2d2d;}
    .topnav { display: flex; justify-content: center; align-items: center; width: 100%; background-color: #1a1a1a; color: white; font-size: 2rem; }
    

    .header1 { font-size: 2rem; color: white; font-weight: bold;}
    .header2 { font-size: 2rem; color: white; font-weight: bold;}

    .content { padding: 20px; display: flex; flex-direction: column;  gap: 2rem; }
    .btnList { display: flex; flex-direction: row; gap: 1rem; align-items: center; }

    .notification { display: flex; flex-direction: column; gap: 2rem; position: absolute; top: 0; left: 0; font-size: 1.5rem; }
    .notification p { padding: 1rem; margin: 0; color: white; }
    .success { background-color: #0f0; }
    .error { background-color: #f00; }
    .info { background-color: #00f; }
    
    .btns { display: flex; padding: .5rem 1rem; border-radius: 1rem; font-size: 2rem; background-color: #1a1a1a; color: white; border: none; cursor: pointer; box-shadow: 0 0 10px rgba(0,0,0,.5); }
  </style>
</head>
<body>
  <div class="topnav">
    <h3>UG MOBILE</h3>
  </div>
  <div class="notification">
  </div>

  <main class="content">

    <div class="btnList">
      <button id="btn-lock" class="btns">Lock</button>
      <button id="btn-unlock" class="btns">Unlock</button>
    </div>

    <span class="header1">Relays</span>
    
    <div class="btnList">
      <span class="header2">1: </span>
      <span class="header2" id="r1state">ON</span>

      <button id="btn-r1on" class="btns">ON</button>
      <button id="btn-r1off" class="btns">OFF</button>
    </div>

    <div class="btnList">
      <span class="header2">2: </span>
      <span class="header2" id="r2state">ON</span>

      <button id="btn-r2on" class="btns">ON</button>
      <button id="btn-r2off" class="btns">OFF</button>
    </div>

    <div class="btnList">
      <span class="header2">3: </span>
      <span class="header2" id="r3state">ON</span>

      <button id="btn-r3on" class="btns">ON</button>
      <button id="btn-r3off" class="btns">OFF</button>
    </div>

    <div class="btnList">
      <span class="header2">4: </span>
      <span class="header2" id="r4state">ON</span>

      <button id="btn-r4on" class="btns">ON</button>
      <button id="btn-r4off" class="btns">OFF</button>
    </div>

    <div class="btnList">
      <span class="header2">5: </span>
      <span class="header2" id="r5state">ON</span>

      <button id="btn-r5on" class="btns">ON</button>
      <button id="btn-r5off" class="btns">OFF</button>
    </div>

    <div class="btnList">
      <span class="header2">6: </span>
      <span class="header2" id="r6state">ON</span>

      <button id="btn-r6on" class="btns">ON</button>
      <button id="btn-r6off" class="btns">OFF</button>
    </div>
    

  </main>
<script>

  const btnLock = document.querySelector('#btn-lock');
  const btnUnlock = document.querySelector('#btn-unlock');



  const notification = document.querySelector('.notification');

  function showNotification(type, message) {
    const pMsg = document.createElement('p');
    pMsg.textContent = message;
    pMsg.classList.add(type);
    notification.appendChild(pMsg);
    setTimeout(() => {
      notification.removeChild(pMsg);
    }, 5000);
  }

 async function unlockDoor() {
    try {
      const response = await fetch('/unlock', { method: 'POST' });
      const data = await response.json();
      showNotification(data.type, data.message);
    } catch (error) {
      showNotification('error', 'An error occurred unlocking door');
    }
  }

  async function lockDoor() {
    try {
      const response = await fetch('/lock', { method: 'POST' });
      const data = await response.json();
      showNotification(data.type, data.message);
    } catch (error) {
      showNotification('error', 'An error occurred locking door');
    }
  }

  async function relayGet(relay) {
    try {
      const response = await fetch(`/rget${relay}`, { method: 'GET' });
      const data = await response.json();
      return data.state;
    } catch (error) {
      showNotification('error', 'An error occurred getting relay state');
    }
  }

  async function relaySet(relay, state) {
    try {
      const response = await fetch(`/r${state ? "on":"off"}${relay}`, { method: 'POST' });
      const data = await response.json();
      showNotification(data.type, data.message);
    } catch (error) {
      showNotification('error', 'An error occurred setting relay');
    }

    const res = await relayGet(relay);
    const stateLabel = document.querySelector(`#r${relay}state`);
    stateLabel.textContent = res ? "ON" : "OFF";
  }


  btnLock.addEventListener('click', (e) => {
    lockDoor();
  });

  btnUnlock.addEventListener('click', (e) => {
    unlockDoor();
  });


  for(let i = 1; i <= 6; i++) {
    const btnOn = document.querySelector(`#btn-r${i}on`);
    const btnOff = document.querySelector(`#btn-r${i}off`);

    btnOn.addEventListener('click', (e) => {
      relaySet(i, true);
    });

    btnOff.addEventListener('click', (e) => {
      relaySet(i, false);
    });

    relayGet(i).then(res => {
      const stateLabel = document.querySelector(`#r${i}state`);
      stateLabel.textContent = res ? "ON" : "OFF";
    });
  }

</script>
</body>
</html>)rawliteral";
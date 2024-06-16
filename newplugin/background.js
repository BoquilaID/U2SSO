chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
  console.log('Received message:', message);
  if (message.action === 'openBoquillaLoginPopup') {
    fetch('http://localhost:8080/generate_challenge')
      .then(response => response.json())
      .then(data => {
        const challenge = data.challenge;
        console.log('Challenge received:', challenge);

        chrome.storage.local.set({ 'challenge': challenge }, () => {
          console.log(`Challenge '${challenge}' saved to local storage.`);
          chrome.windows.create({
            url: chrome.runtime.getURL('loginPopup.html') + `?challenge=${challenge}`,
            type: 'popup',
            width: 400,
            height: 400
          }, () => {
            sendResponse({ success: true });
          });
        });
      })
      .catch(error => {
        console.error('Error generating challenge:', error);
        sendResponse({ success: false, error: error.message });
      });
    return true; // Indicate that the response is asynchronous
  } else if (message.action === 'openBoquillaRegisterPopup') {
    fetch('http://localhost:8080/get_website_name')
      .then(response => response.json())
      .then(data => {
        const websiteName = data.website_name;
        console.log('Website name:', websiteName);

        chrome.storage.local.set({ 'websiteName': websiteName }, () => {
          chrome.windows.create({
            url: chrome.runtime.getURL('registerPopup.html') + `?websiteName=${websiteName}`,
            type: 'popup',
            width: 400,
            height: 400
          }, () => {
            sendResponse({ success: true });
          });
        });
      })
      .catch(error => {
        console.error('Error fetching website name:', error);
        sendResponse({ success: false, error: error.message });
      });
    return true; // Indicate that the response is asynchronous
  } else if (message.action === 'generateKeys') {
    fetch('http://localhost:3000/keys')
      .then(response => response.json())
      .then(data => {
        chrome.storage.local.set({ MSK: data.msk, MPK: data.mpk }, () => {
          if (chrome.runtime.lastError) {
            console.error('Error storing keys in local storage:', chrome.runtime.lastError.message);
            sendResponse({ success: false, error: chrome.runtime.lastError.message });
          } else {
            console.log('Keys stored in local storage:', data);
            sendResponse({ success: true });
          }
        });
      })
      .catch(error => {
        console.error('Error fetching keys:', error);
        sendResponse({ success: false, error: error.message });
      });
    return true; // Indicate that the response is asynchronous
  }
});

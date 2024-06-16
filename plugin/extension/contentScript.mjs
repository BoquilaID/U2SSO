window.addEventListener('boquilla-login', () => {
  console.log('Boquilla login event detected');
  chrome.runtime.sendMessage({ action: 'openBoquillaLoginPopup' }, (response) => {
    if (chrome.runtime.lastError) {
      console.error(chrome.runtime.lastError.message);
    } else if (response && response.success) {
      console.log('Login popup opened successfully');
    } else {
      console.error('Failed to open login popup', response ? response.error : 'Unknown error');
    }
  });
});

window.addEventListener('boquilla-register', () => {
  console.log('Boquilla register event detected');

  // Fetch the website name from the server
  fetch('http://localhost:8080/get_website_name')
    .then(response => response.json())
    .then(data => {
      const websiteName = data.website_name;
      console.log('Website name:', websiteName);

      // Send a message to the background script to open the register popup with the website name
      chrome.runtime.sendMessage({ action: 'openBoquillaRegisterPopup', websiteName: websiteName }, (response) => {
        if (chrome.runtime.lastError) {
          console.error(chrome.runtime.lastError.message);
        } else if (response && response.success) {
          console.log('Register popup opened successfully');
        } else {
          console.error('Failed to open register popup', response ? response.error : 'Unknown error');
        }
      });
    })
    .catch(error => {
      console.error('Error fetching website name:', error);
    });
});

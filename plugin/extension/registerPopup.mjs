document.addEventListener('DOMContentLoaded', () => {
  // Display the website name from local storage
  chrome.storage.local.get('websiteName', (result) => {
    if (chrome.runtime.lastError) {
      console.error('Error fetching website name:', chrome.runtime.lastError);
      document.getElementById('websiteName').innerText = 'Error fetching website name';
      return;
    }

    const websiteName = result.websiteName;
    document.getElementById('websiteName').innerText = `Registering on: ${websiteName}`;
  });

  document.getElementById('confirmRegisterButton').addEventListener('click', () => {
    console.log('Confirm Registration button clicked');

    // Fetch the master public key from local storage
    chrome.storage.local.get('Master_keypair', (result) => {
      if (chrome.runtime.lastError) {
        console.error('Error fetching master key pair:', chrome.runtime.lastError);
        document.getElementById('registerResult').innerText = 'Error fetching master key pair';
        return;
      }

      const masterKeyPair = result.Master_keypair;
      if (!masterKeyPair || !masterKeyPair.publicKeyPem) {
        document.getElementById('registerResult').innerText = 'Master public key not found';
        return;
      }

      const publicKeyPem = masterKeyPair.publicKeyPem;
      console.log('Master public key:', publicKeyPem);

      // Send the public key to the server
      fetch('http://localhost:8080/register', { // Ensure the port matches your server
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({ publicKeyPem })
      })
        .then(response => {
          if (!response.ok) {
            throw new Error('Network response was not ok');
          }
          return response.json();
        })
        .then(data => {
          console.log('Server response:', data);
          document.getElementById('registerResult').innerText = `Registration successful: ${data.message}`;
        })
        .catch(error => {
          console.error('Error registering user:', error);
          document.getElementById('registerResult').innerText = 'Error registering user';
        });
    });
  });
});

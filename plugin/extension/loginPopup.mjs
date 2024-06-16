document.addEventListener('DOMContentLoaded', () => {
  // Display the challenge from local storage
  chrome.storage.local.get('challenge', (result) => {
    if (chrome.runtime.lastError) {
      console.error('Error fetching challenge:', chrome.runtime.lastError);
      document.getElementById('challenge').innerText = 'Error fetching challenge';
      return;
    }

    const challenge = result.challenge;
    document.getElementById('challenge').innerText = `Challenge: ${challenge}`;
  });

  document.getElementById('confirmLoginButton').addEventListener('click', () => {
    // Fetch the challenge and the master public key from local storage
    chrome.storage.local.get(['challenge', 'Master_keypair'], (result) => {
      if (chrome.runtime.lastError) {
        console.error('Error fetching data:', chrome.runtime.lastError);
        document.getElementById('loginResult').innerText = 'Error fetching data';
        return;
      }

      const challenge = result.challenge;
      const masterKeyPair = result.Master_keypair;

      if (!challenge || !masterKeyPair || !masterKeyPair.publicKeyPem) {
        document.getElementById('loginResult').innerText = 'Required data not found';
        return;
      }

      console.log('Challenge:', challenge);
      console.log('Master public key:', masterKeyPair.publicKeyPem);

      // Implement the login logic here
      // For now, we just display a confirmation message
      document.getElementById('loginResult').innerText = 'Login confirmed';
    });
  });
});

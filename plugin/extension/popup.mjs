import { saveToLocalStorage, getFromLocalStorage } from './storage.mjs';

// Function to generate RSA key pair
function generateRsaKeyPair() {
  return new Promise((resolve, reject) => {
    forge.pki.rsa.generateKeyPair({ bits: 2048, workers: -1 }, (err, keypair) => {
      if (err) {
        reject(err);
      } else {
        const publicKeyPem = forge.pki.publicKeyToPem(keypair.publicKey);
        const privateKeyPem = forge.pki.privateKeyToPem(keypair.privateKey);
        resolve({ publicKeyPem, privateKeyPem });
      }
    });
  });
}

document.addEventListener('DOMContentLoaded', () => {
  const generateKeysButton = document.getElementById('generate-keys');
  const generateChildKeysButton = document.getElementById('generate-child-keys');
  const saveEthKeysButton = document.getElementById('save-eth-keys');
  const keysOutput = document.getElementById('keys-output');
  const loader = document.getElementById('loader');
  const ethKeysContainer = document.getElementById('eth-keys-container');

  const showLoader = () => {
    loader.style.display = 'block';
  };

  const hideLoader = () => {
    loader.style.display = 'none';
  };

  // Check if Ethereum keys are already set
  chrome.storage.local.get(['eth_pubkey', 'eth_prikey'], (result) => {
    if (result.eth_pubkey && result.eth_prikey) {
      ethKeysContainer.style.display = 'none';
      keysOutput.innerText = 'Ethereum keys are already set.';
    }
  });

  generateKeysButton.addEventListener('click', async () => {
    showLoader();
    keysOutput.innerText = '';
    try {
      const keys = await generateRsaKeyPair();
      hideLoader();

      // Save keys to local storage
      chrome.storage.local.set({ 'Master_keypair': keys }, () => {
        if (chrome.runtime.lastError) {
          console.error('Error storing keys:', chrome.runtime.lastError);
          keysOutput.innerText = 'Error storing keys';
          return;
        }

        keysOutput.innerText = 'RSA keys generated and stored successfully';
        console.log('Generated keys:', keys);
      });
    } catch (error) {
      hideLoader();
      console.error('Error generating RSA keys:', error);
      keysOutput.innerText = 'Error generating RSA keys';
    }
  });

  saveEthKeysButton.addEventListener('click', () => {
    const ethPubKey = document.getElementById('eth-pubkey').value;
    const ethPriKey = document.getElementById('eth-prikey').value;

    if (!ethPubKey || !ethPriKey) {
      keysOutput.innerText = 'Please enter both Ethereum keys';
      return;
    }

    chrome.storage.local.set({ 'eth_pubkey': ethPubKey, 'eth_prikey': ethPriKey }, () => {
      if (chrome.runtime.lastError) {
        console.error('Error storing Ethereum keys:', chrome.runtime.lastError);
        keysOutput.innerText = 'Error storing Ethereum keys';
        return;
      }

      keysOutput.innerText = 'Ethereum keys saved successfully';
      ethKeysContainer.style.display = 'none';
    });
  });
});

import { saveToLocalStorage, getFromLocalStorage } from './storage.mjs';
// import { getDefaultAccaunt, addElement } from './light_client.mjs';

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
    let pk;
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

        pk = String(keys.publicKeyPem);

      });
    } catch (error) {
      hideLoader();
      console.error('Error generating RSA keys:', error);
      keysOutput.innerText = 'Error generating RSA keys';
    }

    //loading to the contract
    let account = await getDefaultAccaunt();
    // const web3 = new Web3(new Web3.providers.HttpProvider('http://localhost:7545'));
    // const account = await web3.eth.getAccounts()[0];
        
    let tx = await addElement(pk, account);

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

      const addr = "9F7B621341C92b3fc43282dd47048Ebf7151f073";
      const sk = "0x34b04df98af5a4b5eb03a5581489446bcf9f3f7b6e997f4e6c784090c3f4b27b"

      keysOutput.innerText = 'Ethereum keys saved successfully';
      ethKeysContainer.style.display = 'none';
    });
  });
});

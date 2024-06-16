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
  const keysOutput = document.getElementById('keys-output');

  const truffleButton = document.getElementById('post-to-truffle');
  const truffleOutput = document.getElementById('truffle-output');

  const proofButton = document.getElementById('gen-proof');
  const proofOutput = document.getElementById('proof-output');

  const saveEthKeysButton = document.getElementById('save-eth-keys');
  const ethKeysContainer = document.getElementById('eth-keys-container');

  const loader = document.getElementById('loader');

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

    // Check if Ethereum keys are already set
    chrome.storage.local.get(['Master_keypair'], (result) => {
      if (result.Master_keypair) {
        ethKeysContainer.style.display = 'none';
        keysOutput.innerText = 'Master keys are present.';
      }
    });

    let mpk;

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

        keysOutput.innerText = 'Master keys generated and stored successfully';
        console.log('Generated keys:', keys);

        mpk = String(keys.publicKeyPem);

        //show the menu for ethereum 
        // ethKeysContainer.style.display = 'flex';

      });
    } catch (error) {
      hideLoader();
      console.error('Error generating RSA keys:', error);
      keysOutput.innerText = 'Error generating RSA keys';
    }

    //loading to the contract
    // const web3 = setProvider('http://localhost:7545');
    // // const web3 = setProvider('https://ethereum-sepolia-rpc.publicnode.com');
    // let account = await getDefaultAccaunt();
    // // let privKey = '0x530457b80da5a3d00859c0a54b55a80b5314540c60b8c848674cb7e29069ba30'; 
    // // let account = await getAccountEth(privKey, web3);
    // let contract = getContract(web3);
    // let tx = await addElement(pk, account, contract);

  });

  truffleButton.addEventListener('click', async () => {
 
    //loading to the contract
    const web3 = setProvider('http://localhost:7545');
    // const web3 = setProvider('https://ethereum-sepolia-rpc.publicnode.com');
    let account = await getDefaultAccaunt();
    // let privKey = '0x530457b80da5a3d00859c0a54b55a80b5314540c60b8c848674cb7e29069ba30'; 
    // let account = await getAccountEth(privKey, web3);
    let contract = getContract(web3);
    let tx = await addElement(mpk, account, contract);

    // truffleOutput.innerText = 'Key sent on the blockchain';
    truffleOutput.innerText = mpk;
    // Ensure that the popup size adjusts based on the content's height and width
    // document.documentElement.style.height = 'auto';
    // document.documentElement.style.width = 'auto';
    // document.body.style.height = 'auto';
    // document.body.style.width = 'auto';

    // Set the body height and width to the actual content's height and width
    // document.body.style.height = document.documentElement.scrollHeight + 'px';
    document.body.style.width = '500px';


  });

  proofButton.addEventListener('click', () => {

    // Here the code to generate the proof 
    proofOutput.innerText = 'Proof generated';
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

document.addEventListener('DOMContentLoaded', () => {
  const generateKeysButton = document.getElementById('generate-keys');
  const keysOutput = document.getElementById('keys-output');

  const truffleButton = document.getElementById('post-to-truffle');
  const truffleOutput = document.getElementById('truffle-output');

  const proofButton = document.getElementById('generate-proof');
  const proofOutput = document.getElementById('proof-output');

  const loader = document.getElementById('loader');

  const showLoader = () => {
    loader.style.display = 'block';
  };

  const hideLoader = () => {
    loader.style.display = 'none';
  };

  const checkMasterKey = () => {
    chrome.storage.local.get(['MPK'], (result) => {
      if (result.MPK) {
        keysOutput.innerText = `Master Key: ${result.MPK}`;
        generateKeysButton.style.display = 'none';
      } else {
        generateKeysButton.style.display = 'block';
      }
    });
  };

  let mpk;

  generateKeysButton.addEventListener('click', async () => {
    showLoader();
    keysOutput.innerText = '';

    try {
      await new Promise((resolve, reject) => {
        chrome.runtime.sendMessage({ action: 'generateKeys' }, async (response) => {
          hideLoader();
          if (chrome.runtime.lastError || !response.success) {
            const errorMessage = chrome.runtime.lastError ? chrome.runtime.lastError.message : response.error;
            console.error('Error generating keys:', errorMessage);
            keysOutput.innerText = 'Error generating keys';
            reject(errorMessage);
            return;
          }

          chrome.storage.local.get(['MPK'], async (result) => {
            if (chrome.runtime.lastError) {
              console.error('Error fetching keys from local storage:', chrome.runtime.lastError.message);
              keysOutput.innerText = 'Error fetching keys from local storage';
              reject(chrome.runtime.lastError.message);
              return;
            }

            keysOutput.innerText = `Master Key: ${result.MPK}`;
            document.body.style.width = '700px';
            mpk = result.MPK;
            // generateKeysButton.style.display = 'none';
            resolve();
          });
        });
      });
    } catch (error) {
      console.error('Error handling Ethereum keys or transaction:', error);
      keysOutput.innerText += `\nError handling Ethereum keys or transaction: ${error.message}`;
      hideLoader();
    }

  });

    //loading to the contract
    // const web3 = setProvider('http://localhost:7545');
    // // const web3 = setProvider('https://ethereum-sepolia-rpc.publicnode.com');
    // let account = await getDefaultAccaunt();
    // // let privKey = '0x530457b80da5a3d00859c0a54b55a80b5314540c60b8c848674cb7e29069ba30'; 
    // // let account = await getAccountEth(privKey, web3);
    // let contract = getContract(web3);
    // let tx = await addElement(pk, account, contract);

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

  proofButton.addEventListener('click', async () => {

    // Here the code to generate the proof 

    showLoader();
    // keysOutput.innerText = '';

    try {
      // Call the Go program directly
      const response = await fetch('http://localhost:3000/proof_gen', {
        method: 'POST'
      });

      if (!response.ok) {
        throw new Error('Network response was not ok');
      }

      const data = await response.json();
      proofOutput.style.display = 'flex';
      document.getElementById("dynamicProof").innerText = `Computation result: ${data.result}`;
      // proofOutput.innerText = `Computation result: ${data.result}`;
    } catch (error) {
      console.error('Error generating proof:', error);
      proofOutput.innerText = 'Error generating proof';
    } finally {
      hideLoader();
    }

  // Initial check for master key
  checkMasterKey();
    // proofOutput.innerText = 'Proof generated';
  });

});

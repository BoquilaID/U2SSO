
document.addEventListener('DOMContentLoaded', () => {
  const generateKeysButton = document.getElementById('generate-keys');
  const generateProofButton = document.getElementById('generate-proof');
  const keysOutput = document.getElementById('keys-output');
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

  generateKeysButton.addEventListener('click', async () => {
    showLoader();
    keysOutput.innerText = '';

    try {
      let mpk;
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
            mpk = result.MPK;
            generateKeysButton.style.display = 'none';
            resolve();
          });
        });
      });

      // Ensure Web3 is initialized with a provider
      const web3 = new Web3(new Web3.providers.HttpProvider('http://localhost:7545'));
      const ethPrivKey = '0x9e1ee24ce851f4ecf5ba14cfffbe59d356fbf87692be583dc67e339859f4f9e1';

      console.log(`Private key length: ${ethPrivKey.length}`);
      console.log(`Private key: ${ethPrivKey}`);

      const account = web3.eth.accounts.privateKeyToAccount(ethPrivKey);
      console.log(`Account address: ${account.address}`);

      web3.eth.accounts.wallet.add(account);
      console.log(`Web3 wallet: ${JSON.stringify(web3.eth.accounts.wallet)}`);

      web3.eth.defaultAccount = account.address;
      console.log(`Default account: ${web3.eth.defaultAccount}`);

      const abi = [
        {
          "inputs": [],
          "name": "getList",
          "outputs": [
            {
              "internalType": "string[]",
              "name": "",
              "type": "string[]"
            }
          ],
          "stateMutability": "view",
          "type": "function",
          "constant": true
        },
        {
          "inputs": [
            {
              "internalType": "string",
              "name": "elem",
              "type": "string"
            }
          ],
          "name": "addElement",
          "outputs": [],
          "stateMutability": "nonpayable",
          "type": "function"
        }
      ];

      const contractAddress = '0xBd064457A88cd1Ca401287Ba02Ad77D62bc57460';
      const contract = new web3.eth.Contract(abi, contractAddress);

      console.log('Sending transaction with private key:', ethPrivKey);

      const result = await contract.methods.addElement(mpk).send({ from: account.address, gas: 3000000 });
      keysOutput.innerText += `\nTransaction sent successfully: ${result.transactionHash}`;
    } catch (error) {
      console.error('Error handling Ethereum keys or transaction:', error);
      keysOutput.innerText += `\nError handling Ethereum keys or transaction: ${error.message}`;
      hideLoader();
    }
  });

  generateProofButton.addEventListener('click', async () => {
    showLoader();
    keysOutput.innerText = '';

    try {
      // Call the Go program directly
      const response = await fetch('http://localhost:3000/proof_gen', {
        method: 'POST'
      });

      if (!response.ok) {
        throw new Error('Network response was not ok');
      }

      const data = await response.json();
      keysOutput.innerText = `Computation result: ${data.result}`;
    } catch (error) {
      console.error('Error generating proof:', error);
      keysOutput.innerText = 'Error generating proof';
    } finally {
      hideLoader();
    }
  });

  // Initial check for master key
  checkMasterKey();
});

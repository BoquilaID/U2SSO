import Web3 from 'web3';

// Set the provider
export function setProvider(providerUrl) {
  return new Web3(new Web3.providers.HttpProvider(providerUrl));
}

// Get the Ethereum account from the private key
export async function getAccountEth(privateKey, web3) {
  const account = web3.eth.accounts.privateKeyToAccount(privateKey);
  web3.eth.accounts.wallet.add(account);
  web3.eth.defaultAccount = account.address;
  return web3.eth.defaultAccount;
}

// Add an element to the contract
export async function addElement(element, defaultAccount, web3) {
  const abi = [
    // The ABI of your contract
    {
      "inputs": [
        {
          "internalType": "uint256",
          "name": "",
          "type": "uint256"
        }
      ],
      "name": "list",
      "outputs": [
        {
          "internalType": "string",
          "name": "",
          "type": "string"
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
          "name": "",
          "type": "string"
        }
      ],
      "name": "pkToIndex",
      "outputs": [
        {
          "internalType": "uint256",
          "name": "",
          "type": "uint256"
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
    },
    {
      "inputs": [
        {
          "internalType": "string",
          "name": "elem",
          "type": "string"
        }
      ],
      "name": "getIndexOfElement",
      "outputs": [
        {
          "internalType": "uint256",
          "name": "",
          "type": "uint256"
        }
      ],
      "stateMutability": "view",
      "type": "function",
      "constant": true
    },
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
    }
  ];

  let contractAddress = '0xEc42bF8f83815A773cf70bB17e6EFF71a99438bd'; // Your contract address
  let contract = new web3.eth.Contract(abi, contractAddress);

  const result = await contract.methods.addElement(element).send({ from: defaultAccount, gas: 3000000 });
  console.log("Transaction Hash: " + result.transactionHash);
  return result.transactionHash;
}

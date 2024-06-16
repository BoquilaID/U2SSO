// const {Web3} = require('web3');
// const ABI = require('../../contracts/truffle/build/contracts/Boquila.json'); // SET HERE ABI of the contract
     
// SET UP THE WEB3 PROVIDER
// let web3 = new Web3('http://localhost:7545');

// let contract_address = '0x8B0A153d65E1744EA08671a58E0D1d75F10965AA'; // PASTE THE CONTRACT ADDRESS HERE
// let contract = new web3.eth.Contract(ABI.abi, contract_address);

// console.log(contract.methods);
// import Web3 from 'web3';


// This is for truffle 
async function getDefaultAccaunt() {    
        // Get the accounts to use one as default to sign transactions 
        // const {Web3} = require('web3');
        const web3 = new Web3(new Web3.providers.HttpProvider('http://localhost:7545'));
        const accounts = await web3.eth.getAccounts();
        return accounts[0];
} 
// This is for ethereum (second param is the provider)
async function getAccountEth(privateKey, web3) {
  const account = web3.eth.accounts.privateKeyToAccount(privateKey);
  web3.eth.accounts.wallet.add(account);
  web3.eth.defaultAccount = account.address;
  return web3.eth.defaultAccount;
}

function setProvider(providerUrl) {
  return new Web3(new Web3.providers.HttpProvider(providerUrl));
}

async function addElement(element, defaultAccount, contract) {
    // const ABI = require('../../contracts/truffle/build/contracts/Boquila.json');
    // const abi = [
    //     {
    //       "inputs": [
    //         {
    //           "internalType": "uint256",
    //           "name": "",
    //           "type": "uint256"
    //         }
    //       ],
    //       "name": "list",
    //       "outputs": [
    //         {
    //           "internalType": "string",
    //           "name": "",
    //           "type": "string"
    //         }
    //       ],
    //       "stateMutability": "view",
    //       "type": "function",
    //       "constant": true
    //     },
    //     {
    //       "inputs": [
    //         {
    //           "internalType": "string",
    //           "name": "",
    //           "type": "string"
    //         }
    //       ],
    //       "name": "pkToIndex",
    //       "outputs": [
    //         {
    //           "internalType": "uint256",
    //           "name": "",
    //           "type": "uint256"
    //         }
    //       ],
    //       "stateMutability": "view",
    //       "type": "function",
    //       "constant": true
    //     },
    //     {
    //       "inputs": [
    //         {
    //           "internalType": "string",
    //           "name": "elem",
    //           "type": "string"
    //         }
    //       ],
    //       "name": "addElement",
    //       "outputs": [],
    //       "stateMutability": "nonpayable",
    //       "type": "function"
    //     },
    //     {
    //       "inputs": [
    //         {
    //           "internalType": "string",
    //           "name": "elem",
    //           "type": "string"
    //         }
    //       ],
    //       "name": "getIndexOfElement",
    //       "outputs": [
    //         {
    //           "internalType": "uint256",
    //           "name": "",
    //           "type": "uint256"
    //         }
    //       ],
    //       "stateMutability": "view",
    //       "type": "function",
    //       "constant": true
    //     },
    //     {
    //       "inputs": [],
    //       "name": "getList",
    //       "outputs": [
    //         {
    //           "internalType": "string[]",
    //           "name": "",
    //           "type": "string[]"
    //         }
    //       ],
    //       "stateMutability": "view",
    //       "type": "function",
    //       "constant": true
    //     }
    //   ];
    // let contract_address = '0x3dc97341E09a14857391f63f4853415dfA89302C'; // PASTE THE CONTRACT ADDRESS HERE
    // let contract = new web3.eth.Contract(abi, contract_address);

    const result = await contract.methods.addElement(element).send({from: defaultAccount, gas: 3000000});
    console.log("Transaction Hash: " + result.transactionHash)
}

function getContract(web3){
  let contract_address = '0x961B50CF253B974BEA3110BbeE4A81414D8c3ae5'; // PASTE THE CONTRACT ADDRESS HERE
  const abi = [
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
  let contract = new web3.eth.Contract(abi, contract_address);
  return contract;
}

async function getList(contract) {
    const list = await contract.methods.getList().call();
    return list;
}

async function getIndexOfElement(element, contract) {
    const index = await contract.methods.getIndexOfElement(element).call();
    return index;
}

async function interact() {
    const web3 = setProvider('http://localhost:7545');
    const defaultAccount = await getDefaultAccaunt();
    await addElement("Element1", defaultAccount, web3);
    await addElement("Element2", defaultAccount, web3);
    await addElement("Element3", defaultAccount, web3);
    await addElement("Element4", defaultAccount, web3);
    await addElement("Element5", defaultAccount, web3);
    const list = await getList();
    console.log("List: " + list);
    const index = await getIndexOfElement("Element3");
    console.log("Index of Element3: " + index);
}

// interact(); 
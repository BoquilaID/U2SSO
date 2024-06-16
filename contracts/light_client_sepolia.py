from web3 import Web3, HTTPProvider
from solcx import compile_source, install_solc, set_solc_version, get_solc_version

from web3 import Web3

# Initialize endpoint URL
node_url = "https://sepolia.infura.io/v3/90f323dad12a4bc5bacb7249ee2931c3"

# Create the node connection
web3 = Web3(Web3.HTTPProvider(node_url))


# Verify if the connection is successful
if web3.is_connected():
    print("-" * 50)
    print("Connection Successful")
    print("-" * 50)
else:
    print("Connection Failed")


# Initialize the address calling the functions/signing transactions
caller = "0xb16070BEB2629363A8BDFA227940124129276669"
private_key = "<YOUR_PRIVATE_KEY>"  # To sign the transaction

# Initialize address nonce
nonce = web3.eth.get_transaction_count(caller)

# Initialize contract ABI and address
abi =    [
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
				"inputs": [],
				"name": "dummy",
				"outputs": [
					{
						"internalType": "uint256",
						"name": "",
						"type": "uint256"
					}
				],
				"stateMutability": "view",
				"type": "function"
			},
			{
				"inputs": [],
				"name": "getDummy",
				"outputs": [
					{
						"internalType": "uint256",
						"name": "",
						"type": "uint256"
					}
				],
				"stateMutability": "view",
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
				"type": "function"
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
				"type": "function"
			},
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
				"type": "function"
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
				"type": "function"
			},
			{
				"inputs": [
					{
						"internalType": "uint256",
						"name": "_dummy",
						"type": "uint256"
					}
				],
				"name": "setDummy",
				"outputs": [],
				"stateMutability": "nonpayable",
				"type": "function"
			}
		]

contract_address = "0x6B54d6f5FF4B06218689A620DE881b8885776909"

# Create smart contract instance
contract = web3.eth.contract(address=contract_address, abi=abi)


dummy = contract.functions.getDummy().call()  # read the coin total supply - call means we are reading from the blockchain
print("dummy: ", dummy) 

# initialize the chain id, we need it to build the transaction for replay protection
Chain_id = web3.eth.chain_id

# Call your function
call_function = contract.functions.addElement("giannis").build_transaction({"chainId": Chain_id, "from": caller, "nonce": nonce})

# Sign transaction
signed_tx = web3.eth.account.sign_transaction(call_function, private_key=private_key)

# Send transaction
send_tx = web3.eth.send_raw_transaction(signed_tx.rawTransaction)

# Wait for transaction receipt
tx_receipt = web3.eth.wait_for_transaction_receipt(send_tx)
print(tx_receipt) # Optional
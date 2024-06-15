from web3 import Web3, HTTPProvider
from solcx import compile_source, install_solc, set_solc_version, get_solc_version

from web3 import Web3

import json

abi_json_path = './truffle/build/contracts/Boquila.json'

with open(abi_json_path) as f:
    data = json.load(f)
    abi = data['abi']

# Initialize endpoint URL
node_url = 'http://localhost:7545'

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
caller = "0xAf35db5d191E6b32E2aE00d3859a9Fb477C1EB24"
private_key = "fbe94e4ba71170b2541414600409336e088b79809a151ad86f878bf8fb950e3d"  # To sign the transaction

# Initialize address nonce
nonce = web3.eth.get_transaction_count(caller)

# Initialize contract ABI and address

contract_address = "0x53F36358fE83E280ba34BaE451DE3260b3d665Da"

# Create smart contract instance
contract = web3.eth.contract(address=contract_address, abi=abi)


# dummy = contract.functions.getDummy().call()  # read the coin total supply - call means we are reading from the blockchain
# print("dummy: ", dummy) 

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
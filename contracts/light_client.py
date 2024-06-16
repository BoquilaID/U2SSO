from web3 import Web3, HTTPProvider
from solcx import compile_source, install_solc, set_solc_version, get_solc_version

from web3 import Web3

import json

# Function to inicialize the connection to the node
def connect(provider_url):
    # Initialize endpoint URL
    node_url = provider_url #'http://localhost:7545'

    # Create the node connection
    provider = Web3(Web3.HTTPProvider(node_url))

    # Verify if the connection is successful
    if provider.is_connected():
        print("-" * 50)
        print("Connection Successful")
        print("-" * 50)
        return provider
        
    else:
        print("Connection Failed")
        return None

# Function to get the contract
def get_contract(provider, contract_address, abi_json_path):

    with open(abi_json_path) as f:
        data = json.load(f)
        abi = data['abi']

    # Create smart contract instance
    contract = provider.eth.contract(address=contract_address, abi=abi)
    return contract

def send_transaction_to_contract(provider, contract, caller, private_key, pk):
    # initialize the chain id, we need it to build the transaction for replay protection
    chain_id = provider.eth.chain_id
    # Initialize address nonce
    nonce = provider.eth.get_transaction_count(caller)
    # Call your function
    call_function = contract.functions.addElement(pk).build_transaction({"chainId": chain_id, "from": caller, "nonce": nonce})

    # Sign transaction
    signed_tx = provider.eth.account.sign_transaction(call_function, private_key=private_key)

    # Send transaction
    send_tx = provider.eth.send_raw_transaction(signed_tx.rawTransaction)

    return send_tx

def fetch_list(contract):
    # Read the list - call means we are reading from the blockchain
    list = contract.functions.getList().call()
    return list

def query_pk_to_index(contract, pk):
    # Read the index - call means we are reading from the blockchain
    index = contract.functions.pkToIndex(pk).call()
    return index
import light_client

# main function
def main():
    # Connect to the provider
    provider = light_client.connect('http://localhost:7545')

    if not provider:
        return

    # Get the contract
    contract_address = "0x53F36358fE83E280ba34BaE451DE3260b3d665Da"
    abi_json_path = './truffle/build/contracts/Boquila.json'

    contract = light_client.get_contract(provider, contract_address, abi_json_path)

    # Initialize the address calling the functions/signing transactions
    caller = "0xAf35db5d191E6b32E2aE00d3859a9Fb477C1EB24"
    private_key = "fbe94e4ba71170b2541414600409336e088b79809a151ad86f878bf8fb950e3d"  # To sign the transaction

    # Send transaction to the contract
    pk = "giannis"
    send_tx = light_client.send_transaction_to_contract(provider, contract, caller, private_key, pk)

    # Wait for transaction receipt
    tx_receipt = provider.eth.wait_for_transaction_receipt(send_tx)
    print(tx_receipt) # Optional

    # Fetch the list
    list = light_client.fetch_list(contract)
    print("List: ", list)

    # Query the index of a pk 
    index = light_client.query_pk_to_index(contract, pk)
    print("Index: ", index)


if __name__ == "__main__":
    main()
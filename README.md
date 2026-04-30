# Boquila
IC3 Hackaton Project

## Abstract
Identity management is a critical aspect of our digital lives. Federated identities, for instance, have revolutionized the way we access multiple platforms using the same credentials, thereby enhancing usability. However, this approach has its drawbacks, such as delegating security to a single federated identity provider, which becomes a potential single point of failure and a central data collector with complete access to user identity data and their relationships. In contrast, decentralized identities offer a promising solution, empowering users to manage their private data directly and control disclosures for each use case. Yet, managing multiple identities or claims can be overwhelming for the average user of standard online services like social media platforms, online banking, and e-commerce websites. This project delves into the challenges and potential solutions for enhancing decentralized identities to match the usability of federated identities, a crucial topic in the field of identity management and security.

## Structure of the repo 

The cryptographic library for the construction ASC-CRS is under `crypto-dbpoe/`. 
The cryptographic library for the construction ASC-SRS is under `crypto-snark/`.

To run the related tests follow the instructions under each directory.

At this level we collected in `.csv` files the measurements used to create the plot. 

The script `plot_comparison.py` regenerates the plot. 

The `./tests` under `crypto-dbpoe/` run also benchmarks for Boromean signatures (used in DID:RING). 

The data for DualDory was obtained by running their benchmarks at https://github.com/yacovm/DualDory/tree/main# . 


## Proof-of-concept
For a guided proof-of-concept follow the detailed roadmap under `proof-of-concept` directory.

export function mnemonicsToSeed(mnemonics) {
    const seedString = mnemonics.join('');
    return CryptoJS.SHA256(seedString).toString(CryptoJS.enc.Hex);
  }
  
  export function generateRsaKeyPair() {
    const keypair = forge.pki.rsa.generateKeyPair({ bits: 1024, e: 0x10001 });
  
    const privateKeyPem = forge.pki.privateKeyToPem(keypair.privateKey);
    const publicKeyPem = forge.pki.publicKeyToPem(keypair.publicKey);
  
    return { privateKeyPem, publicKeyPem };
  }
  
  export function generateChildRsaKeyPair(seed) {
    const seedBuffer = forge.util.hexToBytes(seed);
    const prng = createPrng(seedBuffer);
    const keypair = forge.pki.rsa.generateKeyPair({ bits: 1024, e: 0x10001, prng });
  
    const privateKeyPem = forge.pki.privateKeyToPem(keypair.privateKey);
    const publicKeyPem = forge.pki.publicKeyToPem(keypair.publicKey);
  
    return { privateKeyPem, publicKeyPem };
  }
  
  function createPrng(seed) {
    let counter = 0;
  
    function getBytes(n) {
      let randomBytes = '';
      while (randomBytes.length < n) {
        const counterBytes = forge.util.createBuffer();
        counterBytes.putInt32(counter++);
        randomBytes += forge.md.sha256.create().update(seed + counterBytes.getBytes()).digest().getBytes();
      }
      return randomBytes.slice(0, n);
    }
  
    return {
      getBytes,
      getBytesSync: getBytes
    };
  }
  
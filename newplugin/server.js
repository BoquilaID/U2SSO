const express = require('express');
const path = require('path');
const { exec } = require('child_process');

const app = express();
const goBinaryPath = path.join(__dirname, 'simulator'); // Path to your Go binary
const proofGenBinaryPath = path.join(__dirname, 'proof_gen'); // Path to your proof generation Go binary

app.use(express.json()); // To parse JSON request bodies

// Endpoint to generate and serve keys
app.get('/keys', (req, res) => {
  exec(goBinaryPath, (error, stdout, stderr) => {
    if (error) {
      console.error(`Error executing Go binary: ${error.message}`);
      return res.status(500).json({ error: 'Failed to generate keys' });
    }

    if (stderr) {
      console.error(`Go binary stderr: ${stderr}`);
      return res.status(500).json({ error: 'Error in Go binary execution' });
    }

    // Read the generated keys from the output (assuming they are printed in stdout)
    const mskMatch = stdout.match(/Returned MSK: ([0-9a-f]+)/);
    const mpkMatch = stdout.match(/Returned MPK: ([0-9a-f]+)/);

    if (mskMatch && mpkMatch) {
      const msk = mskMatch[1];
      const mpk = mpkMatch[1];
      return res.json({ msk, mpk });
    } else {
      console.error('Failed to parse keys from Go binary output');
      return res.status(500).json({ error: 'Failed to parse keys' });
    }
  });
});

// Endpoint to process a specific task using proof_gen
app.post('/proof_gen', (req, res) => {
  exec(proofGenBinaryPath, (error, stdout, stderr) => {
    if (error) {
      console.error(`Error executing proof_gen binary: ${error.message}`);
      return res.status(500).json({ error: 'Failed to process task' });
    }

    if (stderr) {
      console.error(`proof_gen binary stderr: ${stderr}`);
      return res.status(500).json({ error: 'Error in proof_gen binary execution' });
    }

    const result = stdout.trim();
    return res.json({ result });
  });
});

app.listen(3000, () => {
  console.log('Server is running on http://localhost:3000');
});

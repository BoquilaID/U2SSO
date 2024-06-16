import { execFile } from 'child_process';
import path from 'path';

// Path to the compiled Go binary
const __dirname = path.resolve();
const binaryPath = path.join(__dirname, 'simulator');

// Execute the Go binary
execFile(binaryPath, (error, stdout, stderr) => {
  if (error) {
    console.error(`Error executing file: ${error.message}`);
    return;
  }

  if (stderr) {
    console.error(`stderr: ${stderr}`);
    return;
  }

  // Output from the Go program
  console.log(`stdout: ${stdout}`);
});

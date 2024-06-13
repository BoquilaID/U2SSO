const { app, BrowserWindow, ipcMain, Menu } = require('electron/main')
const path = require('node:path')

let registerWindow;

function createRegisterWindow () {
  const registerWindow = new BrowserWindow({
    webPreferences: {
      preload: path.join(__dirname, 'preload.js')
    }
  })

  registerWindow.loadFile('index.html')

    registerWindow.on('closed', () => {
        registerWindow = null;
    });

  // Open the DevTools.
  registerWindow.webContents.openDevTools()
}

app.whenReady().then(() => {
    createRegisterWindow()
})

app.on('window-all-closed', function () {
  if (process.platform !== 'darwin') app.quit()
})

export function saveToLocalStorage(key, value) {
    chrome.storage.local.set({ [key]: value }, () => {
      if (chrome.runtime.lastError) {
        console.error('Error saving to local storage:', chrome.runtime.lastError);
      } else {
        console.log(`Saved ${key} to local storage.`);
      }
    });
  }
  
  export function getFromLocalStorage(key) {
    return new Promise((resolve, reject) => {
      chrome.storage.local.get(key, (result) => {
        if (chrome.runtime.lastError) {
          reject(chrome.runtime.lastError);
        } else {
          resolve(result[key]);
        }
      });
    });
  }
  
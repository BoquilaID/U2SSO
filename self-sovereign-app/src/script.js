// script.js
// import { getDefaultAccaunt, addElement } from './light_client.js';

document.getElementById('registerButton')?.addEventListener('click', function() {
    window.location.href = 'second.html';
});

// document.getElementById('nextButton')?.addEventListener('click', function() {
//     window.location.href = 'index.html';
// });

document.getElementById('ethButton')?.addEventListener('click', function() {
    document.getElementById('contractResponse').style.display = 'flex';   
});

// async function loadToEthContract() {
//     const defaultAccount = await getDefaultAccaunt();
//     const element = document.getElementById('dynamicText1').textContent;
//     await addElement(element, defaultAccount);
//     document.getElementById('contractResponse').textContent = "Element added to the contract";

// }


window.addEventListener('DOMContentLoaded', () => {
    const loadingElement = document.getElementById('loading');
    const linesElement = document.getElementById('info-lines');
    const ethButton = document.getElementById('ethButton');



    // TODO: substitute with KeyGen() from crypto library
    async function generateRandomString(length) {
        return new Promise((resolve, reject) => {
            try {
                const array = new Uint8Array(length);
                window.crypto.getRandomValues(array);
                const randomString = Array.from(array, byte => byte.toString(16).padStart(2, '0')).join('');
                resolve(randomString);
            } catch (error) {
                reject(error);
            }
        });
    }

    async function updateContent() {
        if (loadingElement) {
            try {
                // Generate random strings
                const randomString1 = await generateRandomString(32);
                const randomString2 = await generateRandomString(32);

                console.log('Random strings:', randomString1, randomString2);

                // Update dynamic text with random strings
                document.getElementById('dynamicText1').textContent = randomString1;
                document.getElementById('dynamicText2').textContent = randomString2;

                setTimeout(()=>{
                    // Hide the loading spinner
                    loadingElement.style.display = 'none';

                    // Show the content
                    linesElement.style.display = 'flex';
                    ethButton.style.display = 'flex';
                    
                
                },3000);

            } catch (error) {
                console.error('Error generating random strings:', error);
            }
        }
    }

    // Call updateContent immediately to generate strings and update the content
    updateContent();
});


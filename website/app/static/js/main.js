// app/static/js/main.mjs
document.addEventListener('DOMContentLoaded', () => {
  document.getElementById('loginButton').addEventListener('click', () => {
    const event = new CustomEvent('boquilla-login');
    window.dispatchEvent(event);
  });

  document.getElementById('registerButton').addEventListener('click', () => {
    const registerEvent = new CustomEvent('boquilla-register');
    window.dispatchEvent(registerEvent);
  });
});

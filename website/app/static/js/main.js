document.addEventListener('DOMContentLoaded', () => {
    document.getElementById('loginButton').addEventListener('click', () => {
      const event = new CustomEvent('boquilla-login');
      window.dispatchEvent(event);
    });
  });
  
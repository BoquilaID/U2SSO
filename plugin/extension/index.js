document.addEventListener('DOMContentLoaded', function() {
    function adjustPopupSize() {
        // Ensure that the popup size adjusts based on the content's height and width
        document.documentElement.style.height = 'auto';
        document.documentElement.style.width = 'auto';
        document.body.style.height = 'auto';
        document.body.style.width = 'auto';
    
        // Set the body height and width to the actual content's height and width
        document.body.style.height = document.documentElement.scrollHeight + 'px';
        document.body.style.width = document.documentElement.scrollWidth + 'px';
      }

  adjustPopupSize();
  window.addEventListener('resize', adjustPopupSize);
});

document.getElementById("enterButton").addEventListener('click', () => {
    window.location.href = 'popup.html';
});
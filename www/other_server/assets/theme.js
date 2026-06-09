(function() {
    var t = localStorage.getItem('theme');
    if (t) document.documentElement.setAttribute('data-theme', t);
})();

function toggleTheme(btn) {
    var d = document.documentElement;
    var t = d.getAttribute('data-theme') === 'light' ? 'dark' : 'light';
    d.setAttribute('data-theme', t);
    localStorage.setItem('theme', t);
    btn.textContent = t === 'light' ? 'Into the darkness ✨' : 'Make it wild 💖';
}

document.addEventListener('DOMContentLoaded', function() {
    var btn = document.querySelector('.toggle');
    if (btn && document.documentElement.getAttribute('data-theme') === 'light') {
        btn.textContent = 'Into the darkness ✨';
    }
});

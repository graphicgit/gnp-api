document.addEventListener('DOMContentLoaded', () => {
    // Navbar scroll effect
    const navbar = document.getElementById('navbar');
    window.addEventListener('scroll', () => {
        if (window.scrollY > 50) {
            navbar.classList.add('scrolled');
        } else {
            navbar.classList.remove('scrolled');
        }
    });

    // Smooth scrolling for anchor links
    document.querySelectorAll('a[href^="#"]').forEach(anchor => {
        anchor.addEventListener('click', function (e) {
            e.preventDefault();
            const targetId = this.getAttribute('href').substring(1);
            const targetElement = document.getElementById(targetId);

            if (targetElement) {
                window.scrollTo({
                    top: targetElement.offsetTop - 80,
                    behavior: 'smooth'
                });
            }
        });
    });

    // Dynamic Status simulation
    const statusText = document.querySelector('.status-info p');
    const latencyValue = document.querySelector('.metric:last-child .value');

    function simulateActivity() {
        const randomLatency = Math.floor(Math.random() * 9) + 1; // 1-9ms
        latencyValue.textContent = `${randomLatency}ms`;
    }

    setInterval(simulateActivity, 3000);

    // Fade-in animations on scroll
    const observerOptions = {
        threshold: 0.1
    };

    const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.classList.add('fade-in-visible');
            }
        });
    }, observerOptions);

    document.querySelectorAll('.feature-card, .status-card').forEach(el => {
        el.style.opacity = '0';
        el.style.transform = 'translateY(20px)';
        el.style.transition = 'all 0.6s ease-out';
        observer.observe(el);
    });

    // Custom helper for fade-in
    window.addEventListener('scroll', () => {
        document.querySelectorAll('.feature-card, .status-card').forEach(el => {
            const rect = el.getBoundingClientRect();
            if (rect.top < window.innerHeight * 0.9) {
                el.style.opacity = '1';
                el.style.transform = 'translateY(0)';
            }
        });
    });
});

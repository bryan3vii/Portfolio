document.addEventListener('DOMContentLoaded', (event) => {
    console.log('Document is fully loaded and parsed');

    // Initialize the Bootstrap carousel with specific options
    const myCarouselElement = document.querySelector('#myCarousel');
    const carousel = new bootstrap.Carousel(myCarouselElement, {
        interval: 2000,
        touch: true,
        ride: 'carousel',
        pause: 'hover',
        keyboard: true,
        wrap: true
    });

    // Add event listener for slide events
    myCarouselElement.addEventListener('slide.bs.carousel', (event) => {
        console.log(`Slide event triggered. Direction: ${event.direction}, From: ${event.from}, To: ${event.to}`);
    });

    // Manually control the carousel
    document.querySelector('#nextSlide').addEventListener('click', () => {
        carousel.next();
    });

    document.querySelector('#prevSlide').addEventListener('click', () => {
        carousel.prev();
    });

    document.querySelector('#pauseCarousel').addEventListener('click', () => {
        carousel.pause();
    });

    document.querySelector('#cycleCarousel').addEventListener('click', () => {
        carousel.cycle();
    });
});

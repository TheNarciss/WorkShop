function updateGauge(value) {
    const gaugeFill = document.getElementById('simple-gauge-fill');
    const slider = document.getElementById('simple-gauge-slider');
    
    // Calculer la largeur en pourcentage
    const percentage = (value / 4095) * 100; // Remplace 4095 par la valeur max si besoin
    gaugeFill.style.width = percentage + '%'; // Mettre à jour la largeur du remplissage

    // Mettre à jour la position du curseur
    const sliderPosition = percentage; // position du curseur en pourcentage
    slider.style.left = `calc(${sliderPosition}% + 5px)`; // Ajuste pour que le curseur soit centré
}

// Appeler cette fonction avec la nouvelle valeur pour tester
updateGauge(4095); // Exemple pour mettre à 100%

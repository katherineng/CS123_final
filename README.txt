           _                 _     _     
  __ _ ___| |_ ___ _ __ ___ (_) __| |___ 
 / _` / __| __/ _ \ '__/ _ \| |/ _` / __|
| (_| \__ \ ||  __/ | | (_) | | (_| \__ \  Katherine Ng (kwng) + Winnie Wang (ww15)
 \__,_|___/\__\___|_|  \___/|_|\__,_|___/  17 December 2012


Asteroids fly in random directions in space. Initially there are 40 or less asteroids, though the user can add more by pressing the "A" key. The scene can re-render itself with ~40 asteroids when the user presses "R". Displacement mapping is toggled with "D" and collision detection is toggled with "C."

To render the asteroid, the vertices are displaced along the normals using perlin noise. Vertex displacement occurs on the GPU via the shader (in the file displacement.vert). The displacement value is determined by simulating perlin noise, which indexes into a random array of 512 integers (ranging from 0 to 255) that are randomly generated for every instance of an Asteroid. Because every asteroid indexes into an unique random array of 512 integers, every asteroid's set of vertices will remain the same, but the vertices will still be (psuedo)randomly generated. The texture is mapped onto the asteroid on displacement.vert.

Collision between asteroids is detected by simply iterating through the list of asteroids and calculating the distance between one asteroid and the others. If the distance is below a certain point, the two asteroids will be deleted from the m_asteroids list and the explosion will render. 

The explosion is rendered via a particle emitter. The particles travel outward from the center, with their lifespan slowly decaying over time. This decay translates into a lower alpha value, so that the particles gradually fade out as they radiate from the center. The color also changes as a function of the particle's distance from the center. The color is determined by linearly interpolating between two colors such that a gradient could be created. The actual particle is generated using a texture of a puff of smoke. This texture is 2D, but is billboarded such that the texture is always facing the user. This is accomplished by rotating the texture according to the amount by which the camera has rotated from its starting point (270 degree angle). 

Credits to:
Ken Perlin for perlin noise algorithm
Explosion photoshop brushes made by DeviantArt user maView

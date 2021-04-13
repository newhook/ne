/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
varying vec3 pos_eye;
varying vec3 n_eye;

uniform samplerCube imgTexture0;
uniform mat4 viewMatrix;

void main() {
  vec3 incident_eye = normalize (pos_eye);
  vec3 normal = normalize (n_eye);

  vec3 reflected = reflect (incident_eye, normal);
  // convert from eye to world space
  reflected = vec3 (inverse (viewMatrix) * vec4 (reflected, 0.0));

  gl_FragColor = textureCube(imgTexture0, reflected);
}
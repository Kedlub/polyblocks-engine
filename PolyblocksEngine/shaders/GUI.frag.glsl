#version 330 core

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform vec4 colorIN;

void main() {

    color = colorIN;

}
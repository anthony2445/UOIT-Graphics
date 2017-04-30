/*
 *  Simple vertex shader for example four
 */

in vec3 vPosition;
in vec3 vNormal;
uniform mat4 model;
uniform mat3 normalMat;
out vec3 normal;

void main() {

    gl_Position = model * vec4(vPosition.xyz, 1.0);
    normal = normalMat * vNormal;
}

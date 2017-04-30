/*
 *  Simple vertex shader for example six
 *out vec3 normal;
 *out vec4 position;
 *position = vPosition;
 *normal = vNormal;
 */

in vec4 vPosition;
in vec3 vNormal;
uniform mat4 modelView;
uniform mat4 projection;
uniform mat3 normalMat;
varying vec3 normal;
varying vec3 position;

void main() {
	gl_Position = projection * modelView * vPosition;
	position = vec3(modelView * vPosition);    
    normal = normalize(normalMat * vNormal);
} 
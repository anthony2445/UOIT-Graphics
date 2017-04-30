#version 330 core
in vec3 texCoord;
in vec3 dir;
uniform sampler3D tex;

void main(void) {
	int slices = 10;
	float step = 0.1;
	//vec4 average;
	int i;
	vec4 max;
	vec4 average;
	vec3 coord = texCoord;
	max = vec4(0.0,0.0,0.0,1.0);

	//average = vec4(0.0, 0.0, 0.0, 1.0);
	for(i=0; i<slices; i++) {
		if(length(texture(tex, coord).xyz) > length(max.xyz)){
			max = texture(tex, coord);
		}
		
		coord += step*dir;
	}
	average = vec4(1.0, 0.0, 1.0, 1.0);

	for(i=0; i<slices; i++) {
		average += max-texture(tex,coord);
	}

	//average /= slices;
	gl_FragColor = average;
	//gl_FragColor = max;
}
#version 330 core
 
// Ouput data
out float fragmentDepth;

void main()
{
	// Writes depth values
	fragmentDepth = gl_FragCoord.z;
}
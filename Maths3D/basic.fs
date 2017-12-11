varying vec3 v_Position;
varying vec2 v_TexCoords;
varying vec4 v_Normal;
varying vec4 v_Color;


uniform sampler2D u_Texture;

const vec3 directionalLight = normalize(-vec3(0.0, -1.0, 0.0));

// composantes ambiantes
const vec3 lightAmbiantColor = vec3(0.1, 0.1, 0.1);
const vec3 materialAmbiantColor = vec3(1.0, 1.0, 1.0);
const vec3 lightSpecularColor = vec3(0.5, 0.5,0.5);
// composantes diffuses
const vec3 lightDiffuseColor = vec3(0.5,0.5, 0.5);
const vec3 materialDiffuseColor = vec3(0.8, 0.8, 0.8);
const vec3 materialSpecularColor = vec3(1.0, 1.0, 1.0);
const float u_Shininess=10;


void main(void)
{
	vec3 diffuseColor = lightDiffuseColor * materialDiffuseColor;
    vec2 uv = vec2(v_TexCoords.x, 1.0 - v_TexCoords.y);
	vec3 normal = normalize(v_Normal.xyz);
	float diffuseFactor = max(dot(normal, directionalLight), 0.0);
	
	vec4 diffuse = vec4(diffuseFactor * diffuseColor, 1.0);
	
	

	vec3 P = v_Position;
	vec3 L = vec3(directionalLight);
	vec3 R = normalize(-reflect(L,normal));
	vec3 E = normalize(- P);
	
	vec3 specularColor =  vec3(materialSpecularColor)*lightSpecularColor;
	vec3 ambiantColor = lightAmbiantColor * vec3(materialAmbiantColor);
	vec4 specular = vec4(pow(max(dot(R,E),0.0),u_Shininess)*specularColor,1.0) ;
	gl_FragColor = diffuse*v_Color+ vec4(ambiantColor, 1.0) *v_Color+specular;
}

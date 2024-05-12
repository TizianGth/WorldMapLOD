#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

precision highp float;
uniform mat4 u_MVP;
uniform vec3 u_CameraPosition;
uniform vec2 u_MapSize;

out float v_Distance;
out vec2 v_TexCoord;

void main() {

	gl_Position = u_MVP * position;
	v_TexCoord = vec2(position.x / u_MapSize.x, position.z / u_MapSize.y);
	
	float deltaX = abs(position.x - u_CameraPosition.x);
	float deltaY = abs(position.y - u_CameraPosition.y);
	float deltaZ = abs(position.z - u_CameraPosition.z);
	v_Distance = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;
};

#shader fragment
#version 330 core
precision highp float;
layout(location = 0) out vec4 color;

uniform sampler2D u_Mask;
uniform sampler2D u_Temperature;
uniform sampler2D u_WaterNormalBig;
uniform sampler2D u_WaterNormalSmall;
uniform sampler2D u_WaterColorMask;

uniform vec3 u_LigthWaterColor;
uniform vec3 u_DeepWaterColor;
uniform vec3 u_WaterSpeed;

uniform vec3 u_WaterOffset;

in float v_Distance;
in vec2 v_TexCoord;

void main() {
	float waterScaleBig = 20.0f;
	float waterScaleSmall = 100.0f;

	vec4 temperature = texture(u_Temperature, v_TexCoord);
	float mask = (texture(u_Mask, v_TexCoord)).r;
	vec4 waterNormal = vec4(56 / 255.0, 166 / 255.0, 251 / 255.0, 1);
	vec4 waterColorMask = texture(u_WaterColorMask, v_TexCoord);


	if (v_Distance < 20000.0) {
		vec2 wTexCoordB = vec2(v_TexCoord.x + u_WaterOffset.x, v_TexCoord.y + u_WaterOffset.x);
		vec4 waterNormalBig = texture(u_WaterNormalBig, wTexCoordB * vec2(waterScaleBig, waterScaleBig * 0.5));
		waterNormal = mix(waterNormal, waterNormalBig, (0.7 / -20000.0 * v_Distance + 0.7));


		if (v_Distance < 400.0) {
			vec2 wTexCoordS = vec2(v_TexCoord.x + u_WaterOffset.y, v_TexCoord.y + u_WaterOffset.z);
			vec4 waterNormalSmall = texture(u_WaterNormalSmall, wTexCoordS * vec2(waterScaleSmall, waterScaleSmall * 0.5));
			waterNormal = mix(waterNormalBig, waterNormalSmall, (0.55 / -400.0 * v_Distance + 0.55));
		}
	}

	vec4 waterColor = mix(vec4(u_LigthWaterColor, 1.0), vec4(u_DeepWaterColor, 1.0), waterColorMask);
	vec4 waterColorNormal = vec4(vec3(waterNormal.x, waterNormal.y, waterNormal.z), 1.0) * waterColor;

	
	color = mix(temperature, waterColorNormal, mask);
	
};
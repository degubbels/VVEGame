#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_EXT_nonuniform_qualifier : enable

 #define RESOURCEARRAYLENGTH 16

#include "../common_defines.glsl"
#include "../light.glsl"

// layout(early_fragment_tests) in;

// // layout(location = 0) in vec3 fragPosW;
// // layout(location = 1) in vec3 fragNormalW;
// // layout(location = 2) in vec3 fragTangentW;
// // layout(location = 3) in vec2 fragTexCoord;
// layout (location = 0) in vec2 inUV;

// layout(location = 0) out vec4 outColor;

// // for glow effect
// //layout(location = 1) out vec4 glowColor;

// layout(set = 0, binding = 0) uniform cameraUBO_t {
//     cameraData_t data;
// } cameraUBO;

// layout(set = 1, binding = 0) uniform lightUBO_t {
//     lightData_t data;
// } lightUBO;

// layout(set = 2, binding = 0) uniform sampler2D shadowMap[NUM_SHADOW_CASCADE];

// layout(set = 3, binding = 0) uniform objectUBO_t {
//     objectData_t data;
// } objectUBO;

// layout(set = 4, binding = 0) uniform sampler2D texSamplerArray[RESOURCEARRAYLENGTH];
// layout(set = 4, binding = 1) uniform sampler2D normalSamplerArray[RESOURCEARRAYLENGTH];


// void main() {

//     // //parameters
//     // int  lightType  = lightUBO.data.itype[0];
//     // vec3 camPosW   = cameraUBO.data.camModel[3].xyz;
//     // vec3 lightPosW = lightUBO.data.lightModel[3].xyz;
//     // vec3 lightDirW = normalize( lightUBO.data.lightModel[2].xyz );
//     // float nfac = dot( fragNormalW, -lightDirW)<0? 0.5:1;
//     // vec4 lightParam = lightUBO.data.param;
//     // vec4 texParam   = objectUBO.data.param;
//     // vec2 texCoord   = (fragTexCoord + texParam.zw)*texParam.xy;
//     // ivec4 iparam    = objectUBO.data.iparam;
//     // uint resIdx     = iparam.x % RESOURCEARRAYLENGTH;

//     // //TBN matrix
//     // vec3 N        = normalize( fragNormalW );
//     // vec3 T        = normalize( fragTangentW );
//     // T             = normalize( T - dot(T, N)*N );
//     // vec3 B        = normalize( cross( T, N ) );
//     // mat3 TBN      = mat3(T,B,N);
//     // vec3 mapnorm  = normalize( texture(normalSamplerArray[resIdx], texCoord).xyz*2.0 - 1.0 );
//     // vec3 normalW  = normalize( TBN * mapnorm );

//     // //colors
//     // vec3 ambcol  = lightUBO.data.col_ambient.xyz;
//     // vec3 diffcol = lightUBO.data.col_diffuse.xyz;
//     // vec3 speccol = lightUBO.data.col_specular.xyz;
//     // vec3 fragColor = texture(texSamplerArray[resIdx], texCoord).xyz;

//     // vec3 result = vec3(0,0,0);
//     // int sIdx = 0;
//     // cameraData_t s = lightUBO.data.shadowCameras[0];
//     // float shadowFactor = 1.0;

//     // if( lightType == LIGHT_DIR ) {
//     //     sIdx = shadowIdxDirectional(cameraUBO.data.param,
//     //                                 gl_FragCoord,
//     //                                 lightUBO.data.shadowCameras[0].param[3],
//     //                                 lightUBO.data.shadowCameras[1].param[3],
//     //                                 lightUBO.data.shadowCameras[2].param[3]);

//     //     s = lightUBO.data.shadowCameras[sIdx];
//     //     shadowFactor = shadowFunc(fragPosW, s.camView, s.camProj, shadowMap[sIdx] );

//     //     result +=   dirlight( lightType, camPosW,
//     //                           lightDirW, lightParam, shadowFactor,
//     //                           ambcol, diffcol, speccol,
//     //                           fragPosW, normalW, fragColor);
//     // }


//     // if( lightType == LIGHT_POINT ) {

//     //     sIdx = shadowIdxPoint( lightPosW, fragPosW );
//     //     s = lightUBO.data.shadowCameras[sIdx];
//     //     shadowFactor = shadowFunc(fragPosW, s.camView, s.camProj, shadowMap[sIdx] );

//     //     result +=   pointlight( lightType, camPosW,
//     //                             lightPosW, lightParam, shadowFactor,
//     //                             ambcol, diffcol, speccol,
//     //                             fragPosW, normalW, fragColor);
//     // }

//     // if( lightType == LIGHT_SPOT ) {

//     //     shadowFactor = shadowFunc(fragPosW, s.camView, s.camProj, shadowMap[sIdx] );

//     //     result +=  spotlight( lightType, camPosW,
//     //                           lightPosW, lightDirW, lightParam, shadowFactor,
//     //                           ambcol, diffcol, speccol,
//     //                           fragPosW, normalW, fragColor);
//     // }

//     // if( lightType == LIGHT_AMBIENT ) {
//     //     result += fragColor * ambcol;
//     // }

//     // outColor = vec4( result, 1.0 );

//     // // Glow effect
//     // vec2 coord = gl_FragCoord.xy;



//     // // New params
//     // float glow_size = 10;
//     // vec3 glow_colour = vec3(0.8, 0, 0);
//     // float glow_intensity = 10;

//     // // ivec2 size = textureSize(texSamplerArray[resIdx], 0);

//     // // float uv_x = coord.x * size.x;
//     // // float uv_y = coord.y * size.y;

//     // float uv_x = coord.x;
//     // float uv_y = coord.y;


//     // float sum = 0.0;
//     // for (int n = 0; n < 9; ++n) {
//     //     uv_y = (coord.y) + (glow_size * float(n - 4.5));
//     //     float h_sum = 0.0;
//     //     h_sum += texelFetch(texSamplerArray[resIdx], ivec2(uv_x - (4.0 * glow_size), uv_y), 0).a;
//     //     h_sum += texelFetch(texSamplerArray[resIdx], ivec2(uv_x - (3.0 * glow_size), uv_y), 0).a;
//     //     h_sum += texelFetch(texSamplerArray[resIdx], ivec2(uv_x - (2.0 * glow_size), uv_y), 0).a;
//     //     h_sum += texelFetch(texSamplerArray[resIdx], ivec2(uv_x - glow_size, uv_y), 0).a;
//     //     h_sum += texelFetch(texSamplerArray[resIdx], ivec2(uv_x, uv_y), 0).a;
//     //     h_sum += texelFetch(texSamplerArray[resIdx], ivec2(uv_x + glow_size, uv_y), 0).a;
//     //     h_sum += texelFetch(texSamplerArray[resIdx], ivec2(uv_x + (2.0 * glow_size), uv_y), 0).a;
//     //     h_sum += texelFetch(texSamplerArray[resIdx], ivec2(uv_x + (3.0 * glow_size), uv_y), 0).a;
//     //     h_sum += texelFetch(texSamplerArray[resIdx], ivec2(uv_x + (4.0 * glow_size), uv_y), 0).a;
//     //     sum += h_sum / 9.0;
//     // }

//     //outColor = vec4(glow_colour, (sum / 9.0) * glow_intensity);
//     outColor = vec4(0.8);
// }


layout (location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;

void main() {
    //vec3 cam = cameraUBO.data.camModel[3].xyz;
    //ivec4 iparam    = objectUBO.data.iparam;
    // uint resIdx     = iparam.x % RESOURCEARRAYLENGTH;
    // vec4 texel = texelFetch(texSamplerArray[resIdx], ivec2(inUV.x, inUV.y), 0);
    //vec4 texel = texelFetch(shadowMap[0], ivec2(inUV.x, inUV.y), 0);

    //outColor = vec4(inUV.x, inUV.y, 0.0, 1.0);

    float threshold = 0.2;
    float mult = 2;

    if (inUV.x < threshold) {
        //outColor = vec4(0.4);
        float val = threshold - inUV.x;
        outColor = vec4(val * mult);

    } else if (inUV.x > (1-threshold)) {
        float val = threshold - (1-inUV.x);
        outColor = vec4(val * mult);

    } else {
        outColor = vec4(0.0);
    }
    
}
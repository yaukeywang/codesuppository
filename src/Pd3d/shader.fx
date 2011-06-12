
float4 AmbientColor  = { 0.3, 0.3, 0.3, 1 };
float4 DiffuseColor  = { 1, 1, 1, 1 };

float4 SpecularColor = { 1, 1, 1, 1 };

float  SpecularPower = 100.0;

float ClampLow       = 0;
float ClampHigh      = 10000;
float ClampScale     = 1.0;

float4 EyePos        = {0,0,0,0};
float4 lightpos      = { .707*40.0, 0.707*40.0, -.707*40.0, 1};	//vector pointing away from light

float2 gOneOverHalfScreenSize : INVERSESCREENSIZE = { 1.0/512.0, 1.0/348.0 };

float4x4 wvp;
float4x4 wm;
float4x4 vp;
float4x4 wv;
float4x4 proj;

texture DiffuseMap    : DIFFUSEMAP;
texture EnvironmentMap: ENVIRONMENTMAP;
texture LightMap : LIGHTMAP;
texture DetailMap : DETAILMAP;

struct VS_INPUT_SCREENQUAD
{
  float4 Pos          : POSITION;
  float3 TexCoordHack : NORMAL;
  float4 Diffuse  : COLOR;
};

struct VS_INPUT
{
  float4 Pos      : POSITION;
  float3 Normal   : NORMAL;
  float2 TexCoord : TEXCOORD0;
  float2 TexCoord2 : TEXCOORD1;
  float2 TexCoord3 : TEXCOORD2;
  float4 Diffuse  : COLOR;
};


struct VS_OUTPUT
{
  float4 Pos       : POSITION;
  float3 TexCoord  : TEXCOORD0;
  float4 TexCoord1 : TEXCOORD1;
  float4 TexCoord2 : TEXCOORD2;
  float3 TexCoord3 : TEXCOORD3;
  float3 TexCoord4 : TEXCOORD4;
  float3 TexCoord5 : TEXCOORD5;
  float4 Diffuse   : COLOR;
};

sampler2D Sampler1 = sampler_state
{
  Texture   = (DiffuseMap);
  MipFilter = LINEAR;
  MinFilter = LINEAR;
  MagFilter = LINEAR;
  AddressU  = WRAP;
  AddressV  = WRAP;
};

sampler2D SamplerLightMap = sampler_state
{
  Texture   = (LightMap);
  MipFilter = LINEAR;
  MinFilter = LINEAR;
  MagFilter = LINEAR;
  AddressU  = WRAP;
  AddressV  = WRAP;
};

sampler2D SamplerDetailMap = sampler_state
{
  Texture   = (DetailMap);
  MipFilter = LINEAR;
  MinFilter = LINEAR;
  MagFilter = LINEAR;
  AddressU  = WRAP;
  AddressV  = WRAP;
};


samplerCUBE SamplerEnvironmentMap = sampler_state
{
  Texture = (EnvironmentMap);
  MipFilter = LINEAR;
  MinFilter = LINEAR;
  MagFilter = LINEAR;
  AddressU  = WRAP;
  AddressV  = WRAP;
};

sampler3D Sampler3D = sampler_state
{
  Texture   = (EnvironmentMap);
  MipFilter = LINEAR;
  MinFilter = LINEAR;
  MagFilter = LINEAR;
};

float4 PixelShaderScreenQuad(VS_OUTPUT In) : COLOR
{
  float4 outColor = tex2D(Sampler1,In.TexCoord);
  return (outColor * In.Diffuse);
}


VS_OUTPUT SoftBodyVertexShader(VS_INPUT In,uniform float3 ldir)
{
  VS_OUTPUT Out = (VS_OUTPUT)0;

  float4 opos = mul(In.Pos,wm);
  Out.Pos     = mul(opos,vp);

  Out.TexCoord = float3(In.TexCoord,0);

  float3 normal = normalize(In.Normal);

  float3 newnorm = mul(normal,(float3x3)wm);

  float NdotL = dot(newnorm,ldir);

  Out.Diffuse = (DiffuseColor*clamp(NdotL,0,1)) + AmbientColor;

  // ok.compute environment map texture coords
  //
  // Compute normal in camera space
  //
  float3 vN = mul(normal, wv );
  vN = normalize( vN );

	Out.TexCoord1.xyz = vN;

  //
  // Obtain the reverse eye vector
  //
  float3 vEyeR = -normalize( opos );

  //
  // Compute the reflection vector
  //
  float3 vRef = 2 * dot( vEyeR, vN ) * vN - vEyeR;

  //
  // Store the reflection vector in texcoord1
  //
  Out.TexCoord2.xyz = vRef;

  return Out;
}


VS_OUTPUT LightMapVertexShader(VS_INPUT In,uniform float3 ldir)
{
  VS_OUTPUT Out = (VS_OUTPUT)0;

  float4 opos = mul(In.Pos,wm);
  Out.Pos     = mul(opos,vp);

  Out.TexCoord = float3(In.TexCoord,0);
  Out.TexCoord2.xyz = float3(In.TexCoord2,0);
  Out.TexCoord3.xyz = float3(In.TexCoord3,0)*0.1;

  return Out;
}


VS_OUTPUT FractalVertexShader(VS_INPUT In,uniform float3 ldir)
{
  VS_OUTPUT Out = (VS_OUTPUT)0;

  In.Pos.y = (clamp(In.Pos.y,ClampLow,ClampHigh)-ClampLow)*ClampScale+1;

  float4 opos = mul(In.Pos,wm);
  Out.Pos     = mul(opos,vp);

  Out.TexCoord = float3(In.TexCoord,0);

  float3 normal = normalize(In.Normal);

  float3 newnorm = mul(normal,(float3x3)wm);

  float NdotL = dot(newnorm,ldir);

  Out.Diffuse = (DiffuseColor*clamp(NdotL,0,1)) + AmbientColor;

  // ok.compute environment map texture coords
  //
  // Compute normal in camera space
  //
  float3 vN = mul(normal, wv );
  vN = normalize( vN );

	Out.TexCoord1.xyz = vN;

  //
  // Obtain the reverse eye vector
  //
  float3 vEyeR = -normalize( opos );

  //
  // Compute the reflection vector
  //
  float3 vRef = 2 * dot( vEyeR, vN ) * vN - vEyeR;

  //
  // Store the reflection vector in texcoord1
  //
  Out.TexCoord2.xyz = vRef;

  return Out;
}



VS_OUTPUT SoftBodyVertexShaderSolid(VS_INPUT In,uniform float3 ldir)
{
  VS_OUTPUT Out = (VS_OUTPUT)0;

  float4 opos = mul(In.Pos,wm);
  Out.Pos     = mul(opos,vp);

  Out.TexCoord = float3(In.TexCoord,0);

  float3 normal = normalize(In.Normal);

  float3 newnorm = mul(normal,(float3x3)wm);

  float NdotL = dot(newnorm,ldir);

  Out.Diffuse = In.Diffuse*((DiffuseColor*clamp(NdotL,0,1)) + AmbientColor);

  // ok.compute environment map texture coords
  //
  // Compute normal in camera space
  //
  float3 vN = mul(normal, wv );
  vN = normalize( vN );

	Out.TexCoord1.xyz = vN;

  //
  // Obtain the reverse eye vector
  //
  float3 vEyeR = -normalize( opos );

  //
  // Compute the reflection vector
  //
  float3 vRef = 2 * dot( vEyeR, vN ) * vN - vEyeR;

  //
  // Store the reflection vector in texcoord1
  //
  Out.TexCoord2.xyz = vRef;

  return Out;
}



VS_OUTPUT SoftBodyVertexShaderWireFrame(VS_INPUT In)
{
  VS_OUTPUT Out = (VS_OUTPUT)0;

  float4 opos = mul(In.Pos,wm);
  Out.Pos     = mul(opos,vp);

  Out.Diffuse = In.Diffuse;
  return Out;
}

VS_OUTPUT VertexShaderScreenQuad(VS_INPUT_SCREENQUAD In)
{

  VS_OUTPUT Out = (VS_OUTPUT)0;

  Out.Pos.x= +( ((In.Pos.x-0.5)*gOneOverHalfScreenSize.x)-1.0 );
  Out.Pos.y= -( ((In.Pos.y-0.5)*gOneOverHalfScreenSize.y)-1.0 );

  Out.Pos.z = In.Pos.z;

  Out.Pos.w=1;

  Out.TexCoord.x = In.TexCoordHack.x;
  Out.TexCoord.y = In.TexCoordHack.y;
  Out.Diffuse    = In.Diffuse;

  return Out;

}

float4 SoftBodyPixelShader(VS_OUTPUT In,uniform float sPower) : COLOR
{
  float4 outColor = tex2D(Sampler1,In.TexCoord);

  // Compute normal dot half for specular light
  float3 vHalf = normalize( normalize( -EyePos ) + normalize( lightpos - EyePos ) );
  float4 fSpecular = pow( saturate( dot( vHalf, normalize( In.TexCoord1.xyz ) ) ) * SpecularColor, sPower );

  float4 envMap = texCUBE(SamplerEnvironmentMap,In.TexCoord2);

//  return envMap;

//  return (outColor*In.Diffuse)+fSpecular;

  return (outColor*In.Diffuse);

}

float4 LightMapPixelShader(VS_OUTPUT In,uniform float sPower) : COLOR
{
  float4 diffuse  = tex2D(Sampler1,In.TexCoord);
  float4 lightmap = tex2D(SamplerLightMap,In.TexCoord2);
  float4 detail = tex2D(SamplerDetailMap,In.TexCoord3);
  return (diffuse*lightmap*2*detail);
}

float4 FractalPixelShader(VS_OUTPUT In,uniform float sPower) : COLOR
{
  float4 outColor = tex2D(Sampler1,In.TexCoord);

  // Compute normal dot half for specular light
  float3 vHalf = normalize( normalize( -EyePos ) + normalize( lightpos - EyePos ) );
  float4 fSpecular = pow( saturate( dot( vHalf, normalize( In.TexCoord1.xyz ) ) ) * SpecularColor, sPower );
  float4 envMap = texCUBE(SamplerEnvironmentMap,In.TexCoord2);

//  return envMap;

  return (outColor*In.Diffuse)+fSpecular;

//  return (outColor*In.Diffuse);
}

float4 SoftBodyPixelShaderSolid(VS_OUTPUT In,uniform float sPower) : COLOR
{
  // Compute normal dot half for specular light
  float3 vHalf = normalize( normalize( -EyePos ) + normalize( lightpos - EyePos ) );
  float4 fSpecular = pow( saturate( dot( vHalf, normalize( In.TexCoord1.xyz ) ) ) * SpecularColor, sPower );
  return In.Diffuse+fSpecular;
}

float4 SoftBodyPixelShaderWireFrame(VS_OUTPUT In,uniform float4 wcolor) : COLOR
{
  return wcolor*In.Diffuse;
}

float4 SoftBodyPixelShaderWireFrameWhite(VS_OUTPUT In,uniform float4 wcolor) : COLOR
{
  return 1;
}

//******************************************** SKY BOX *************

VS_OUTPUT VertexShaderSkyBox(VS_INPUT In)
{

  VS_OUTPUT Out = (VS_OUTPUT)0;

  // Strip any translation off of the view matrix
  // Use only rotations & the projection matrix
  float4x4 matViewNoTrans =
  {
    wv[0],
    wv[1],
    wv[2],
    float4( 0.f, 0.f, 0.f, 1.f )
  };

  // Output the position
	Out.Pos = mul( In.Pos, mul( matViewNoTrans, proj ) );


  Out.TexCoord = normalize(In.Pos.xyz);

  return Out;

}

float4 PixelShaderSkyBox(VS_OUTPUT In) : COLOR
{
  float4 outColor = tex3D(Sampler3D,In.TexCoord);
  return outColor;
}


float3 softLdir       = { .707, .707, -.707 };	//vector pointing away from light

technique SoftBodyShader
{
  pass P0
  {
    AlphaBlendEnable = FALSE;
    ZWriteEnable     = TRUE;
    ZEnable          = TRUE;
    VertexShader = compile vs_2_0 SoftBodyVertexShader(softLdir);
    PixelShader  = compile ps_2_0 SoftBodyPixelShader(SpecularPower);
  }
}

technique SoftBodyWireFrameShader
{
  pass P0
  {
    FillMode = WIREFRAME;
    AlphaBlendEnable = FALSE;
    ZWriteEnable     = TRUE;
    ZEnable          = TRUE;
    VertexShader = compile vs_2_0 SoftBodyVertexShaderWireFrame();
    PixelShader  = compile ps_2_0 SoftBodyPixelShaderWireFrame(DiffuseColor);
  }
}

technique SoftBodyWireFrameWhite
{
  pass P0
  {
    FillMode = WIREFRAME;
    AlphaBlendEnable = FALSE;
    ZWriteEnable     = TRUE;
    ZEnable          = TRUE;
    VertexShader = compile vs_2_0 SoftBodyVertexShaderWireFrame();
    PixelShader  = compile ps_2_0 SoftBodyPixelShaderWireFrameWhite(DiffuseColor);
  }
}

technique SoftBodySolidShader
{
  pass P0
  {
    AlphaBlendEnable = FALSE;
    ZWriteEnable     = TRUE;
    ZEnable          = TRUE;

    VertexShader = compile vs_2_0 SoftBodyVertexShaderSolid(softLdir);
    PixelShader  = compile ps_2_0 SoftBodyPixelShaderSolid(SpecularPower);
  }
}

technique SCREENQUAD
{

  pass P0
  {

    CullMode = NONE;
    AlphaTestEnable = TRUE;
    AlphaBlendEnable = TRUE;
    SrcBlend         = SrcAlpha;
    DestBlend        = InvSrcAlpha;
    ZWriteEnable     = TRUE;
    ZEnable          = TRUE;

    VertexShader = compile vs_2_0 VertexShaderScreenQuad();
    PixelShader  = compile ps_2_0 PixelShaderScreenQuad();

  }
}

technique FractalShader
{
  pass P0
  {
    AlphaBlendEnable = FALSE;
    ZWriteEnable     = TRUE;
    ZEnable          = TRUE;
    VertexShader = compile vs_2_0 FractalVertexShader(softLdir);
    PixelShader  = compile ps_2_0 FractalPixelShader(SpecularPower);
  }
}

technique SkyBox
{
  pass P0
  {
    AlphaBlendEnable = FALSE;
    ZWriteEnable     = FALSE;
    ZEnable          = FALSE;
    VertexShader = compile vs_2_0 VertexShaderSkyBox();
    PixelShader  = compile ps_2_0 PixelShaderSkyBox();
  }
}

technique LightMapShader
{
  pass P0
  {
    AlphaBlendEnable = FALSE;
    ZWriteEnable     = TRUE;
    ZEnable          = TRUE;
    VertexShader = compile vs_2_0 LightMapVertexShader(softLdir);
    PixelShader  = compile ps_2_0 LightMapPixelShader(SpecularPower);
  }
}


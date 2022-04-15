#define USE_CUSTOM 0
#define USE_CUSTOM_SELECT 0
#define USE_ANTI_ALIASING 1
#define USE_PRE_SHARPEN 1
#define USE_BLOOM 1
#define USE_HDR 1
#define USE_TECHNICOLOR 0
#define USE_TONEMAP 0
#define USE_SEPIA 1
#define USE_NOISE 0
#define USE_RADIAL 0
#define USE_SCANLINES 0
#define USE_ANAGLYPH 0
#define USE_VIGNETTE 0
#define USE_POST_SHARPEN 0
#define USE_FINAL_LIMITER 0
#define USE_SPLITSCREEN 0
#define FXAA_QUALITY__PRESET 9
float fxaaQualitySubpix = 0.3885;
float fxaaQualityEdgeThreshold = 0.1921;
float fxaaQualityEdgeThresholdMin = 0.0545;
bool highQualitySharpen = 0;
float Sharpen = 0.056;
float AverageBlur = 0.35;
float CoefficientsBlur = 1.4332;
float CoefficientsOriginal = 2.4332;
float SharpenEdge = 0.28;
float SharpenContour = 0.07;
float BloomThreshold = 30.0;
float BloomPower = 8.0;
float BloomWidth = 0.5;
float HDRPower = 1.20;
float radius2 = 0.80;
#define TechniAmount 0.11
#define TechniPower 2.4
#define redNegativeAmount 0.99
#define greenNegativeAmount 0.99
#define blueNegativeAmount 0.99
#define Gamma 1.2
#define Exposure 0
#define Saturation 0.15
#define Bleach 0.25
#define Defog -0.04
#define FogColor float4(2.55, 2.55, 2.55, 2.55)
#define BlueShift 0
#define ColorTone float3(0.00, 0.00, 0.00)
float SepiaPower = 0.25;
float GreyPower = 1.20;
#define NoiseStrength 0.101
#define NoiseIntensity 0.11
#define LineIntensity 0.11
#define LineAmount 512
#define NoiseMode 1
#define RadialStrength 0.15
#define RadialWidth -0.05
#define ScanLineWidth 1
#define ScanLineColor 1
#define ScanLinePower 0.01
float AnaglyphSeperation = 0.0015;
float AnaglyphRed = 0;
float AnaglyphGreen = 0;
float AnaglyphBlue = 0;
#define VignetteRadius 0.985
#define VignetteAmount -2
#define VignetteStrength 0.05
float4 LimiterMinColor= float4(0.01,0.01,0.01,0.0f);
float4 LimiterMaxColor= float4(0.99,0.99,0.99,1.0f);
#define SplitScreenMode 1
#define Check1 USE_HDR
#define Shader1 HDRPass
#define Check2 USE_BLOOM
#define Shader2 BloomPass
#define Check3 USE_TECHNICOLOR
#define Shader3 TechnicolorPass
#define Check4 USE_TONEMAP
#define Shader4 TonemapPass
#define Check5 USE_SEPIA
#define Shader5 SepiaPass
#define Check6 USE_NOISE
#define Shader6 NoisePass
#define Check7 USE_RADIAL
#define Shader7 RadialPass
#define Check8 USE_SCANLINES
#define Shader8 ScanlinePass
#define Check9 USE_CUSTOM
#define Shader9 CustomPass
#define CustomValue1 1
#define CustomValue2 1
#define CustomValue3 1
#define CustomValue4 1
#define CustomValue5 1
#define CustomValue6 1


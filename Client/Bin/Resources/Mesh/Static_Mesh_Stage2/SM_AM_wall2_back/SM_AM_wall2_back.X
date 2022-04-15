xof 0303txt 0032
template ColorRGBA {
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <d3e16e81-7835-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template Material {
 <3d82ab4d-62da-11cf-ab39-0020af71e433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template TextureFilename {
 <a42790e1-7810-11cf-8f52-0040333594a3>
 STRING filename;
}

template Frame {
 <3d82ab46-62da-11cf-ab39-0020af71e433>
 [...]
}

template Matrix4x4 {
 <f6f23f45-7686-11cf-8f52-0040333594a3>
 array FLOAT matrix[16];
}

template FrameTransformMatrix {
 <f6f23f41-7686-11cf-8f52-0040333594a3>
 Matrix4x4 frameMatrix;
}

template Vector {
 <3d82ab5e-62da-11cf-ab39-0020af71e433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template MeshFace {
 <3d82ab5f-62da-11cf-ab39-0020af71e433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template Mesh {
 <3d82ab44-62da-11cf-ab39-0020af71e433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template MeshMaterialList {
 <f6f23f42-7686-11cf-8f52-0040333594a3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]
}

template VertexElement {
 <f752461c-1e23-48f6-b9f8-8350850f336f>
 DWORD Type;
 DWORD Method;
 DWORD Usage;
 DWORD UsageIndex;
}

template DeclData {
 <bf22e553-292c-4781-9fea-62bd554bdd93>
 DWORD nElements;
 array VertexElement Elements[nElements];
 DWORD nDWords;
 array DWORD data[nDWords];
}


Material material_0 {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "M_AM_wall_2_back";
 }
}

Frame SM_AM_wall2_back {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh SM_AM_wall2_back {
  6;
  183.377293;383.999974;105.766330;,
  183.377318;-0.000006;105.766099;,
  -183.377293;-0.000018;105.766080;,
  -183.377293;-0.000018;105.766080;,
  -183.377293;384.000026;105.766310;,
  183.377293;383.999974;105.766330;;
  2;
  3;0,1,2;,
  3;3,4,5;;

  MeshMaterialList {
   1;
   2;
   0,
   0;
   { material_0 }
  }

  DeclData {
   5;
   2;0;3;0;,
   2;0;6;0;,
   2;0;7;0;,
   1;0;5;0;,
   1;0;5;1;;
   78;
   858960158,
   891289600,
   3212836864,
   1065353216,
   869123308,
   858960174,
   869123312,
   3212836864,
   3038773248,
   1065328640,
   1040531456,
   1065328640,
   1040531456,
   858960141,
   891289601,
   3212836864,
   1065353216,
   856398423,
   858960146,
   856398430,
   3212836864,
   3038773249,
   1065328640,
   1065328640,
   1065328640,
   1065328640,
   858960160,
   891289600,
   3212836864,
   1065353216,
   856398423,
   858960165,
   856398430,
   3212836864,
   3038773248,
   1040539648,
   1065328640,
   1040539648,
   1065328640,
   858960160,
   891289600,
   3212836864,
   1065353216,
   3024995564,
   858960128,
   3024995562,
   3212836864,
   3038773248,
   1040539648,
   1065328640,
   1040539648,
   1065328640,
   858960177,
   891289600,
   3212836864,
   1065353216,
   3023221012,
   858960149,
   3023221010,
   3212836864,
   3038773248,
   1040539648,
   1040531456,
   1040539648,
   1040531456,
   858960158,
   891289600,
   3212836864,
   1065353216,
   3023221012,
   858960130,
   3023221010,
   3212836864,
   3038773248,
   1065328640,
   1040531456,
   1065328640,
   1040531456;
  }
 }
}
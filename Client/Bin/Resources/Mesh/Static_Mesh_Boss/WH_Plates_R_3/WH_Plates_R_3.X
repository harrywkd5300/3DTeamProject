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
  "WH_Plates_1";
 }
}

Frame WH_Plates_R_3 {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh WH_Plates_R_3 {
  8;
  -114.225536;-0.000000;-9.810293;,
  112.923008;-183.580966;-9.846020;,
  -113.104832;-183.580966;-9.846020;,
  -114.225536;-0.000000;-9.810293;,
  111.802291;-0.000000;-9.810293;,
  -111.802291;183.580966;0.004489;,
  111.802291;-0.000000;-9.810293;,
  114.225536;183.580966;0.004489;;
  4;
  3;0,1,2;,
  3;1,3,4;,
  3;5,4,0;,
  3;6,5,7;;

  MeshMaterialList {
   1;
   4;
   0,
   0,
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
   104;
   0,
   1024182167,
   3212827092,
   1065353216,
   0,
   0,
   0,
   3212827092,
   3171665814,
   1050279936,
   1057398784,
   1023541248,
   1056964608,
   0,
   961286148,
   3212836864,
   1065353216,
   0,
   0,
   0,
   3212836864,
   3108769796,
   1057325056,
   1060413440,
   1060487168,
   1065082880,
   0,
   961286148,
   3212836864,
   1065353216,
   0,
   0,
   0,
   3212836864,
   3108769796,
   1050279936,
   1060413440,
   1024442368,
   1065082880,
   0,
   961286148,
   3212836864,
   1065353216,
   0,
   0,
   0,
   3212836864,
   3108769796,
   1050279936,
   1057398784,
   1023541248,
   1056964608,
   0,
   1015415857,
   3212834619,
   1065353216,
   0,
   0,
   0,
   3212834619,
   3162899505,
   1057325056,
   1057398784,
   1060429824,
   1056964608,
   0,
   1029352455,
   3212812938,
   1065353216,
   0,
   0,
   0,
   3212812938,
   3176836104,
   1050279936,
   1051828224,
   1025490944,
   1015152640,
   0,
   1029352455,
   3212812938,
   1065353216,
   0,
   0,
   0,
   3212812938,
   3176836104,
   1057325056,
   1057398784,
   1060429824,
   1056964608,
   0,
   1029352455,
   3212812938,
   1065353216,
   0,
   0,
   0,
   3212812938,
   3176836104,
   1057325056,
   1051828224,
   1060552704,
   1015152640;
  }
 }
}
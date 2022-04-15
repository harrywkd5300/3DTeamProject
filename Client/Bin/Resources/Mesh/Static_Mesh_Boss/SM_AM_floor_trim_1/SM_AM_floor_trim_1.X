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
  "M_AM_floor_trim_1";
 }
}

Frame SM_AM_floor_trim_1 {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh SM_AM_floor_trim_1 {
  8;
  -192.000000;-0.000000;-80.774477;,
  191.999987;3.208815;-79.124922;,
  191.999872;-0.000000;-80.774477;,
  -191.999539;3.208815;-79.124922;,
  -192.000000;3.208815;0.000000;,
  191.999987;3.208815;-79.124922;,
  -191.999539;3.208815;-79.124922;,
  191.999987;3.208815;0.000000;;
  4;
  3;0,1,2;,
  3;1,0,3;,
  3;4,5,6;,
  3;5,4,7;;

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
   1055528325,
   3210980738,
   1065353216,
   0,
   0,
   0,
   3210980738,
   3203011973,
   939524096,
   1046601728,
   939524096,
   1065230336,
   0,
   1055528324,
   3210980738,
   1065353216,
   0,
   0,
   0,
   3210980738,
   3203011972,
   1065353216,
   1045954560,
   1065353216,
   1064501248,
   0,
   1055528324,
   3210980738,
   1065353216,
   0,
   0,
   0,
   3210980738,
   3203011972,
   1065353216,
   1046601728,
   1065353216,
   1065230336,
   0,
   1055528324,
   3210980738,
   1065353216,
   0,
   0,
   0,
   3210980738,
   3203011972,
   939524096,
   1045954560,
   939524096,
   1064501248,
   0,
   1065353216,
   0,
   1065353216,
   0,
   0,
   0,
   0,
   3212836864,
   939524096,
   939524096,
   939524096,
   939524096,
   0,
   1065353216,
   0,
   1065353216,
   0,
   0,
   0,
   0,
   3212836864,
   1065353216,
   1045954560,
   1065353216,
   1064501248,
   0,
   1065353216,
   0,
   1065353216,
   0,
   0,
   0,
   0,
   3212836864,
   939524096,
   1045954560,
   939524096,
   1064501248,
   0,
   1065353216,
   0,
   1065353216,
   0,
   0,
   0,
   0,
   3212836864,
   1065353216,
   939524096,
   1065353216,
   939524096;
  }
 }
}
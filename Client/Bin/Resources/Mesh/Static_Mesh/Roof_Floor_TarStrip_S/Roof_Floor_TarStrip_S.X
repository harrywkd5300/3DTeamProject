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
  "Roof_Floor_TarStrip";
 }
}

Frame Roof_Floor_TarStrip_S {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh Roof_Floor_TarStrip_S {
  8;
  -28.831635;0.462006;192.000000;,
  0.000000;2.001486;192.000000;,
  -28.831635;0.462008;-192.000000;,
  0.000000;2.001486;192.000000;,
  0.000000;2.001492;-192.000000;,
  28.831635;0.462006;192.000000;,
  28.831635;0.462008;-192.000000;,
  0.000000;2.001492;-192.000000;;
  4;
  3;0,1,2;,
  3;3,4,2;,
  3;1,5,4;,
  3;5,6,7;;

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
   3176818078,
   1065329350,
   833252484,
   0,
   2980752043,
   1065353216,
   1065329350,
   1029334430,
   798071060,
   1082122240,
   999178240,
   1065091072,
   1011146752,
   984303496,
   1065353202,
   841793857,
   680058201,
   2989277514,
   1065353216,
   1065353202,
   3131787144,
   2909237043,
   1082122240,
   1056997376,
   1065091072,
   1056899072,
   3176818078,
   1065329350,
   833252484,
   2953539814,
   2980813036,
   1065353216,
   1065329350,
   1029334430,
   810840137,
   939524096,
   999178240,
   1013080064,
   1011146752,
   3176818114,
   1065329350,
   847129684,
   2896148726,
   2994637334,
   1065353216,
   1065329350,
   1029334466,
   811230658,
   1082122240,
   1056997376,
   1065091072,
   1056899072,
   3175516031,
   1065333497,
   847138179,
   2894764860,
   2994641654,
   1065353216,
   1065333497,
   1028032383,
   809928575,
   939524096,
   1056997376,
   1013080064,
   1056899072,
   1029334431,
   1065329350,
   847225542,
   2914889114,
   2994731759,
   1065353216,
   1065329350,
   3176818079,
   2958325257,
   1082122240,
   1065345024,
   1065091072,
   1065091072,
   1029334466,
   1065329350,
   832930323,
   755381034,
   2980430340,
   1065353216,
   1065329350,
   3176818114,
   2945554732,
   939524096,
   1065345024,
   1013080064,
   1065091072,
   1029334465,
   1065329350,
   832930323,
   756877111,
   2980430496,
   1065353216,
   1065329350,
   3176818113,
   2945601551,
   939524096,
   1056997376,
   1013080064,
   1056899072;
  }
 }
}
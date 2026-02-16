#include "common.hlsli"

[maxvertexcount(6)]

void geo(in triangle PS_IN input[3], in uint Id : SV_PrimitiveID, inout TriangleStream<PS_IN> Out) {

    matrix vp = mul(View, Projection);
    
    for (int i = 0; i < 3; i++)
    {
        input[i].Diffuse.gb = 0.0f;
        input[i].Position = mul(input[i].Position, vp);
        Out.Append(input[i]);
    
    }
    
    Out.RestartStrip();
    
    //PS_IN copy[3];
    //matrix vp;
    
    //vp = mul(View, Projection);
    
    //for (int i = 0;i<3;i++) {
    //    copy[i] = input[i];
        
    //    input[i].Diffuse.gb = 0.0f;
    //    input[i].Position=mul(input[i].Position,vp);
    //    Out.Append(input[i]);
    //}
    
    //Out.RestartStrip();
    
    //PS_IN nv;
    //nv=copy[0];
    //nv.Diffuse.rg=0.0f;
    //nv.Diffuse.a=1.0f;
    
    //nv.Normal.xyz=cross(copy[1].Position.xyz-copy[0].Position.xyz,copy[2].Position.xyz-copy[1].Position.xyz);
    
    //nv.Diffuse.rgb = saturate(nv.Normal.xyz);
    
    //nv.Position.x = (copy[0].Position.x + copy[1].Position.x + copy[2].Position.x) / 3.0f;
    //nv.Position.y = (copy[0].Position.y + copy[1].Position.y + copy[2].Position.y) / 3.0f;
    //nv.Position.z = (copy[0].Position.z + copy[1].Position.z + copy[2].Position.z) / 3.0f;
    //nv.Position.w = 1.0f;
    
    //nv.Position.xyz += nv.Normal.xyz * 0.14f;
    
    //nv.Position=mul(nv.Position,vp);
    //copy[0].Position = mul(copy[0].Position,vp);
    //copy[1].Position = mul(copy[1].Position,vp);
    //copy[2].Position = mul(copy[2].Position,vp);
    
    //Out.Append(copy[0]);
    //Out.Append(nv);
    //Out.Append(copy[2]);
    //Out.RestartStrip();
    
    //Out.Append(copy[1]);
    //Out.Append(nv);
    //Out.Append(copy[0]);
    //Out.RestartStrip();
    
    //Out.Append(copy[2]);
    //Out.Append(nv);
    //Out.Append(copy[1]);
    //Out.RestartStrip();
}
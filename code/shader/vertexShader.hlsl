#include "common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    matrix World; //���[���h�s����쐬
    World = mul(mul(Scale, Angle), Translation);
	//���_�ϊ��s������ World * View * Projection
    matrix wvp;
    wvp = mul(mul(World, View), Projection);

	//�|���S���̒��_��ϊ��s��ŕϊ����ďo��
    Out.Position = mul(float4(In.Position, 1.0f), wvp); //Position * wvp
	
//==========================================================================
// �@���̉�]����
//==========================================================================

    float4 normal = float4(In.Normal.xyz, 0.0f); //w������0�ɂ���
    float4 worldNormal = mul(normal, World); //�@�������[���h���W�ɕϊ�
    worldNormal = normalize(worldNormal); //�@���x�N�g���͐��K������
    
    Out.Normal = worldNormal; //�@���f�[�^���o��
    
//==========================================================================
// �����v�Z����
//==========================================================================

    float light = -dot(worldNormal.xyz, Light.DirectionalLight.direction.xyz); //�����̕����x�N�g���Ɩ@���x�N�g���̓��ς��v�Z
    light = saturate(light); //0.0f~1.0f�͈̔͂Ɏ��߂�
    
    Out.TexCoord = In.TexCoord; //�e�N�X�`�����W
    Out.Diffuse.rgb = In.Diffuse * diffuse; //�P�x��Diffuse��RGB�����Ɋi�[
    Out.Diffuse.a = In.Diffuse.a; //���_�F�̃A���t�@���������̂܂܎g�p
    
    Out.WorldPosition = mul(float4(In.Position, 1.0f), World).xyz;
}
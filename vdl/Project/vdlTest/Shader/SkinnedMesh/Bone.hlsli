struct Bone
{
  float3 Position;
  float3 Normal;
};

Bone Calc(float4 _Position, float4 _Normal, row_major float4x4 _BoneTransform, float _BoneWeight)
{
  Bone Bone;

  Bone.Position = (_BoneWeight * mul(_Position, _BoneTransform)).xyz;
  Bone.Normal = (_BoneWeight * mul(float4(_Normal.xyz, 0.0f), _BoneTransform)).xyz;

  return Bone;
}

float Diffuse(float3 _Direction, float3 _Normal)
{
  return max(dot(normalize(_Direction), _Normal), 0.0f);
}

float Attenuation(float _Range, float _Distance)
{
  return 1.0f - smoothstep(_Range * 0.75f, _Range, _Distance);
}

class DirectionalLight
{
  float3 Direction;
  float Itensity;
  float4 Color;

  float4 Calc(float3 _Normal)
  {
    return Diffuse(-Direction, _Normal) * Itensity * Color;
  }
};

class PointLight
{
  float3 Position;
  float Itensity;
  float4 Color;
  float Range;
  float3 Padding;

  float4 Calc(float3 _Position, float3 _Normal)
  {
    const float3 Light = Position - _Position;
    const float Length = length(Light);

    return Diffuse(Light, _Normal) * Attenuation(Range, Length) * Itensity * Color;
  }
};

struct SpotLight
{
  float3 Position;
  float Itensity;
  float3 Direction;
  float Angle;
  float4 Color;
  float Range;
  float3 Padding;

  float4 Calc(float3 _Position, float3 _Normal)
  {
    const float3 Light = Position - _Position;
    const float Length = length(Light);

    const float MinCos = cos(Angle);
    const float MaxCos = lerp(MinCos, 1.0f, 0.5f);

    const float cosAngle = dot(-Direction, normalize(Light));

    const float Cone = smoothstep(MinCos, MaxCos, cosAngle);

    return Diffuse(-Direction, _Normal) * Attenuation(Range, Length) * Cone * Itensity * Color;
  }
};

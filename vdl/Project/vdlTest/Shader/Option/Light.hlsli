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
};
float4 Calc(DirectionalLight _Light, float3 _Normal)
{
  return Diffuse(-_Light.Direction, _Normal) * _Light.Itensity * _Light.Color;
}

class PointLight
{
  float3 Position;
  float Itensity;
  float4 Color;
  float3 Padding;
  float Range;
};
float4 Calc(PointLight _Light, float3 _Position, float3 _Normal)
{
  const float3 Direction = _Light.Position - _Position;
  const float Length = length(Direction);

  return Diffuse(Direction, _Normal) * Attenuation(_Light.Range, Length) * _Light.Itensity * _Light.Color;
}

struct SpotLight
{
  float3 Position;
  float Itensity;
  float3 Direction;
  float Angle;
  float4 Color;
  float3 Padding;
  float Range;
};
float4 Calc(SpotLight _Light, float3 _Position, float3 _Normal)
{
  const float3 Direction = _Light.Position - _Position;
  const float Length = length(Direction);

  const float MinCos = cos(_Light.Angle);
  const float MaxCos = lerp(MinCos, 1.0f, 0.5f);

  const float cosAngle = dot(-_Light.Direction, normalize(Direction));

  const float Cone = smoothstep(MinCos, MaxCos, cosAngle);

  return Diffuse(-_Light.Direction, _Normal) * Attenuation(_Light.Range, Length) * Cone * _Light.Itensity * _Light.Color;
}

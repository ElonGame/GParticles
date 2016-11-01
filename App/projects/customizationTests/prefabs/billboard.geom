vec4[4] transform()
{
  @colorsG = @colorsV[0];
  @lifetimesG = @lifetimesV[0];

  vec4 quad[4] = getStretchedQuad(0.1, 0.1, @velocitiesV[0].xyz, false, @deltaTime);
  return billboardDirection(quad,
                            (view*model*@velocitiesV[0]).xyz);
}
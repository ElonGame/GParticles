////////////////////////////////////////////////////////////////////////////////
float randInRange(vec2 seed, float minVal, float maxVal)
{
	float val = snoise(seed);
	float percentage = val * 0.5 + 0.5;
	return mix(minVal, maxVal, percentage);
}
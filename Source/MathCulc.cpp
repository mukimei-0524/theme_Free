#include "MathCulc.h"

// ���`��ԁi�P��̒l�j
float MathCulc::Lerp(float a, float b, float t)
{
	return a * (1.0f - t) + (b * t);
}
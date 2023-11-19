#pragma once
#include "UI.h"

namespace Game
{
	namespace Math
	{
		inline ImVec2 VecAdd(ImVec2 a, ImVec2 b)
		{
			return ImVec2{ a.x + b.x, a.y + b.y };
		}

		inline ImVec2 VecSub(ImVec2 a, ImVec2 b)
		{
			return ImVec2{ a.x - b.x, a.y - b.y };
		}

		inline ImVec2 VecMul(ImVec2 a, ImVec2 b)
		{
			return ImVec2{ a.x * b.x, a.y * b.y };
		}

		inline ImVec2 VecMul(ImVec2 a, float b)
		{
			return ImVec2{ a.x * b, a.y * b };
		}

		inline ImVec2 VecCross(ImVec2 a)
		{
			return ImVec2{a.y, -a.x};
		}

		inline float VecDot(ImVec2 a, ImVec2 b)
		{
			return a.x * b.x + a.y * b.y;
		}

		inline float VecLength(ImVec2 a)
		{
			float x = abs(a.x);
			float y = abs(a.y);

			return sqrtf(x * x + y * y);
		}

		inline void VecNormalize(ImVec2& a)
		{
			float len = VecLength(a);
			if (len == 0)
				return;

			a.x = a.x / len;
			a.y = a.y / len;
		}

		inline ImVec2 VecClamp(ImVec2 a)
		{
			return ImVec2{ fmaxf(0, fminf(WIDTH, a.x)), fmaxf(0, fminf(HEIGHT, a.y)) };
		}

		inline int RandomInt(int from, int to)
		{
			if (from > to)
			{
				int temp = from;
				from = to;
				to = temp;
			}

			return rand() % (to - from + 1) + from;
		}

		inline int RandomInt(int to)
		{
			return RandomInt(0, to);
		}

		inline float RandomFloat(float from, float to)
		{
			return from + rand() / (RAND_MAX / (to - from));
		}

		inline float RandomFloat(float max)
		{
			return (rand() / RAND_MAX) * max;
		}

		inline float Lerp(float delta, float from, float to)
		{
			return from + (to - from) * delta;
		}

		inline float Approach(float delta, float from, float to)
		{
			delta = fabsf(delta);

			if (from < to)
			{

				return fminf(from + delta, to);
			}
			else if (from > to)
			{
				return fmaxf(from - delta, to);
			}

			return to;
		}

		inline ImVec2 VecLerp(float delta, ImVec2 from, ImVec2 to)
		{
			return ImVec2{ Lerp(delta, from.x, to.x), Lerp(delta, from.y, to.y) };
		}

		inline ImVec2 VecApproach(float delta, ImVec2 from, ImVec2 to)
		{
			return ImVec2{ Approach(delta, from.x, to.x), Approach(delta , from.y, to.y) };
		}
	}
}
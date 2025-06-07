#pragma once
#include <DX3D/Core/Core.h>
#include <cmath>

namespace dx3d
{
	class Vec3
	{
	public:
		Vec3() = default;
		Vec3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

		void normalize()
		{
			const f32 length = std::sqrt(x * x + y * y + z * z);
			if (length > 0.0f) // Avoid division by zero
			{
				x /= length;
				y /= length;
				z /= length;
			}
		}

	public:
		f32 x{}, y{}, z{};
	};
}
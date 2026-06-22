#pragma once

class Collider
{
public:
	// 상대 좌표 (Entity 기준)
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	float width = 0.0f;
	float height = 0.0f;

	// AABB 충돌 체크
	bool IsColliding(float myX, float myY, const Collider& other, float otherX, float otherY) const
	{
		float leftA = myX + offsetX;
		float rightA = leftA + width;
		float topA = myY + offsetY;
		float bottomA = topA + height;
		
		float leftB = otherX + other.offsetX;
		float rightB = leftB + other.width;
		float topB = otherY + other.offsetY;
		float bottomB = topB + other.height;
		
		if (rightA < leftB || leftA > rightB || bottomA < topB || topA > bottomB)
			return false;
			
		return true;
	}
};

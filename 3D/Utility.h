#ifndef UTILITY_H
#define UTILITY_H
//CHECKS IF VALUE IS BETWEEN MIN AND MAX IF IT IS RETURN TRUE OTHERWISE RETURN FALSE
class Utility
{
	private:


	public:
		inline bool checkBounds(float min, float max, float value)
		{
			if (value > min && value < max)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		inline bool oppositeDirection(float marginX, float marginY, glm::vec2 direction, glm::vec2 direction2)
		{
			if (direction.x < (-direction2.x + marginX) && direction.x > (-direction2.x - marginX))
			{
				if (direction.y < (-direction2.y + marginY) && direction.y >(-direction2.y - marginY))
				{
					return true;
				}
			}
			return false;
		}
};
#endif
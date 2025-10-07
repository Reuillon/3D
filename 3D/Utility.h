#ifndef UTILITY_H
#define UTILITY_H
//CHECKS IF VALUE IS BETWEEN MIN AND MAX IF IT IS RETURN TRUE OTHERWISE RETURN FALSE
class Utility
{
	private:


	public:
		bool checkBounds(float min, float max, float value)
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
};
#endif
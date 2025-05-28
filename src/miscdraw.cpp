#include "miscdraw.h"

char seq_by_orient[4][4] = {
	{0,1,2,3},{1,3,0,2},{3,2,1,0},{2,0,3,1}
};

char p_by_orient(unsigned int orientation)
{
	char orient[4] = {0,1,3,2};
	return orient[orientation];
}

char arrow_by_orient(unsigned int orientation)
{
	if(orientation == 0) return 0x1a;
	if(orientation == 1) return 0x19;
	if(orientation == 2) return 0x1b;
	if(orientation == 3) return 0x18;
}

char* draw_seq_by_orient(unsigned int orientation)
{
	return seq_by_orient[orientation];
}
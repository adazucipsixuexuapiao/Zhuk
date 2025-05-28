#pragma once

extern char seq_by_orient[4][4];

char p_by_orient(unsigned int orientation);
char arrow_by_orient(unsigned int orientation);
char* draw_seq_by_orient(unsigned int orientation);
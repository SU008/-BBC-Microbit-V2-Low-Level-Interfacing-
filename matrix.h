#ifndef __MATRIX_H
#define __MATRIX_H
int matrix_begin();
void matrix_all_off();
void matrix_put_pattern(uint8_t rows, uint8_t cols);
void matrix_put_pattern_smile();            //to show smile in LED matrix eg :)
void matrix_put_pattern_patternUnhappy(); //to show unhappy in LED matrix eg :(

#endif

#pragma once

#define min(x,y) ({ \
const typeof(x) _x = (x);	\
const typeof(y) _y = (y);	\
(void) (&_x == &_y);		\
_x < _y ? _x : _y; })

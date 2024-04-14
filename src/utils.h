#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#define DEBUG
#define HEADER(level) std::cout << level << ": " << __FILE__ << ":" << __func__ << ":" << __LINE__ << ": "
#define DEBUG_START 

#define DEBUGLN
#define d
#define dret
#define lblock

#ifdef DEBUG

#undef DEBUGLN
#undef d
#undef dret
#undef lblock

#define DEBUGLN(s) { HEADER("DEBUG")<< s << std::endl; }
#define d(s) DEBUGLN(s)
#define lblock std::cout << "====================================================================================================" << std::endl;
// static int bp = 0;
#define dret return 0;
#endif
#endif
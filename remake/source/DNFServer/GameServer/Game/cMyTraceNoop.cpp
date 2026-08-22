// df_game_r 还原 —— cMyTraceNoop（ORIG ctor 0x822ad5a / operator() 0x822ad60，空实现）。
#include "cMyTraceNoop.h"

cMyTraceNoop::cMyTraceNoop() {}

void cMyTraceNoop::operator()(const char*, ...) {}
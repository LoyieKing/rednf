#ifndef DNF_GAME_TAIWAN_INTERNAL_H_
#define DNF_GAME_TAIWAN_INTERNAL_H_
#include "CStreamGuard.h"
#include "MoreTypes.h"
namespace Taiwan {
void internal_stream(CStreamGuard& guard, TaiwanInternalPack::T packet, int uid);
}
#endif

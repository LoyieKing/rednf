// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG 0x08ab0996 (C1 W), nm-only
// STWorldmapPattern2ND：ORIG C1 W 0x08ab0996（+0x00=-1，+0x04 起 memset 0x18）；
// GetWorldMapCoordFromTable2ND T 0x088e5f44 写 7 个 int（+0x00..+0x18），sizeof 0x1c。
#ifndef DNF_GAME_STWORLDMAPPATTERN2ND_H_
#define DNF_GAME_STWORLDMAPPATTERN2ND_H_

#include <string.h>   // memset

class STWorldmapPattern2ND
{
public:
    STWorldmapPattern2ND()   // W 0x08ab0996：base=-1，coord 区清零
    {
        m_base = -1;
        memset(m_coord, 0, sizeof(m_coord));
    }

    int m_base;          // +0x00
    int m_coord[3][2];   // +0x04
};  // sizeof 0x1c

#endif  // DNF_GAME_STWORLDMAPPATTERN2ND_H_

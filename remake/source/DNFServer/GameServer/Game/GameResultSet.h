#ifndef DNF_GAME_RESULT_SET_H_
#define DNF_GAME_RESULT_SET_H_

#include "GameResultType.h"

class GameResultSet : public GameResultType
{
public:
    GameResultSet();
    ~GameResultSet();
    void Clear();
};

#endif

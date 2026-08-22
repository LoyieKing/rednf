// df_game_r Game/ EventClassify::CEventScriptMng（批3：从 GameStubs.cpp 迁移）。
#ifndef DNF_CEVENTSCRIPTMNG_H
#define DNF_CEVENTSCRIPTMNG_H

class CUser;

namespace EventClassify
{
class CEventScriptMng
{
public:
    void send_event_init_data(CUser* user, bool flag);
};
}

#endif  // DNF_CEVENTSCRIPTMNG_H
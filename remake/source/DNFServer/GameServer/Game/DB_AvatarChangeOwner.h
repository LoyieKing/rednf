#ifndef GAME_DB_AVATARCHANGEOWNER_H_
#define GAME_DB_AVATARCHANGEOWNER_H_

#include "DBDispatcher.h"

// df_game_r 还原 —— DB_AvatarChangeOwner（球员换主 DB 请求，DBThread 派发）。
// 唯一声明点（原定义于 DB_AvatarChangeOwner.cpp，本头拆出供多 TU include）。
// 语义：向 DB 队列发一条 SIG_AVATAR_CHANGE_OWNER 请求。参考
// docs/class_func_reports/DB_AvatarChangeOwner/makeRequest.md，ORIG 0x0840055c。
// dispatch 报告 docs/class_func_reports/DB_AvatarChangeOwner/dispatch.md，
// ORIG 0x084004bc（GoalAuditDB7；W8LT_DBTask.cpp 提供定义）。
class DB_AvatarChangeOwner : public DBDispatcher
{
public:
    DB_AvatarChangeOwner() {}   // ORIG C1 0x844d7f2 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x084004bc
    static void makeRequest(unsigned int a, unsigned int b, unsigned int c,
                            unsigned char d);   // ORIG T 0x0840055c
};

#endif  // GAME_DB_AVATARCHANGEOWNER_H_

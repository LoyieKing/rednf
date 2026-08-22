// DB 同步 stub 类实现（batch-5 迁移）。
// 保留原 GameStubs.cpp 的空体行为；签名与 ORIG df_game_r 符号一致。

#include "DB_AccountCargoSync.h"

void DB_CreateAccountCargo::makeRequest(int, unsigned int, unsigned int) {}

void DB_DeleteAccountCargo::makeRequest(int, unsigned int) {}

void DB_UpgradeAccountCargo::makeRequest(int, unsigned int, unsigned int) {}

void DB_GuildExpBookDelete::makeRequest(unsigned int, int, ENUM_SERVER_GROUP) {}

void DB_InsertRandomOptionItemInform::makeRequest(int, unsigned int,
                                                  unsigned char)
{
}

void DB_PassPadUpdateFailCnt::makeRequest(unsigned int, unsigned int, char*)
{
}  // TODO(G2)

namespace APSystem
{
void DB_UpdateActionPoint::makeRequest(int, const _SIG_LOAD_ACTION_POINT&, bool)
{
}  // TODO(G2)
}
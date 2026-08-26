#ifndef GAME_DB_UPDATEAVATARHISTORY_H_
#define GAME_DB_UPDATEAVATARHISTORY_H_

// df_game_r 还原 —— DB_UpdateAvatarHistory（头像历史更新 DB 请求，DBThread 派发）。
// 唯一声明点；定义于 CompoundDeathLCB.cpp（WongWork::CCompoundAvatar::_ProcCompoundCore
// 合成成功后为被消耗的两件材料各发一条）。
// 参考 docs/class_func_reports/DB_UpdateAvatarHistory/makeRequest.md，ORIG T 0x0844536a。
class DB_UpdateAvatarHistory
{
public:
    // ORIG mangled _ZN22DB_UpdateAvatarHistory11makeRequestEjiPc：
    // 流头 0x2b0 + -1，正文 characNo(uint)、avatarUid(int)、ipgNo 二进制 0x18 字节，
    // 入 MsgQueue 队列 2。
    static void makeRequest(unsigned int characNo, int avatarUid, char* ipgNo);
};

#endif  // GAME_DB_UPDATEAVATARHISTORY_H_

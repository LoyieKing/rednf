#ifndef GAME_DB_EVENTACCOUNTCHARACTERLEVELUPONCEGIFT_H_
#define GAME_DB_EVENTACCOUNTCHARACTERLEVELUPONCEGIFT_H_

// df_game_r Game/ DB_EventAccountCharacterLevelUpOnceGift（DB 请求封装；独立叶子头）。
//
// ORIG（df_game_r, elf32-i386）：
//   _ZN39DB_EventAccountCharacterLevelUpOnceGift11makeRequestEjsiss  T 0x08449f66
//   静态成员。函数体经 StreamPool::Acquire(0x940bd6c, "DBThread.cpp", 0xb9d9)
//   + CStreamGuard 构造，顺序写 int 0x319、int -1、uint 实参1、short 实参2、
//   int 实参3、short 实参4、short 实参5，最后 MsgQueueMgr::put(DB_Q, guard)。
class DB_EventAccountCharacterLevelUpOnceGift
{
public:
    static void makeRequest(unsigned int accId, short serverGroup, int characNo,
                            short job, short level);
};

#endif  // GAME_DB_EVENTACCOUNTCHARACTERLEVELUPONCEGIFT_H_

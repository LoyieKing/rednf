#ifndef GAME_DB_LOADCHARACHOTKEYOPTION_H_
#define GAME_DB_LOADCHARACHOTKEYOPTION_H_

// df_game_r Game/ DB_LoadCharacHotKeyOption（DB 请求封装；独立叶子头）。
//
// ORIG（df_game_r, elf32-i386）：
//   _ZN25DB_LoadCharacHotKeyOption11makeRequestEiji  T 0x0842f710
//   静态成员（调用点 CUser::LoadCharacTypeHotKey 0x08692d2a +0x14f 处以
//   (GetUID, get_acc_id, GetCharacHotKeyType) 三实参直调，无 this）。
//   函数体经 StreamPool::Acquire(0x940bd6c, "dnt", 0x749a) + CStreamGuard
//   构造/析构入 DB 请求队列（0x0842f710 反汇编实证）。
class DB_LoadCharacHotKeyOption
{
public:
    static void makeRequest(int uid, unsigned int accId, int hotkeyType);
};

#endif  // GAME_DB_LOADCHARACHOTKEYOPTION_H_

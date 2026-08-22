#ifndef DNF_GAME_CUSER_GLOBAL_INFO_HANDLE_H_
#define DNF_GAME_CUSER_GLOBAL_INFO_HANDLE_H_

// df_game_r 还原 —— CUserGlobalInfoHandle（G2-4 批次，用户全局信息句柄）。
// 原类定义位于 CUserGlobalInfoHandle.cpp 内部，因 CGameManager.cpp 需真实调用
// get_uniqueid（原 asm 桥 _ZN21CUserGlobalInfoHandle12get_uniqueidEv），
// 拆到本头作为唯一声明点（CUserGlobalInfoHandle.cpp 亦 include）。
// 布局：+0x00 char m_used[0x10000] | +0x10000 int m_next（总尺寸 0x10004）。

class CUserGlobalInfoHandle
{
public:
    CUserGlobalInfoHandle();
    ~CUserGlobalInfoHandle();

    int find_uniqueid();
    int get_uniqueid();
    void reset_uniqueid_flag(unsigned short uniqueid);

    bool m_used[0x10000];  // +0x00
    int m_next;            // +0x10000
};

CUserGlobalInfoHandle* CUserGlobalInfoHandleInstance();

#endif  // DNF_GAME_CUSER_GLOBAL_INFO_HANDLE_H_

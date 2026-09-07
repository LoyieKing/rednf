#ifndef DNF_GAME_CSCHOOL_MGR_H_
#define DNF_GAME_CSCHOOL_MGR_H_

// df_game_r 还原 —— CSchoolMgr（学院 PvP 频道用户/学院分区管理器，G2-4 批次）。
// 原类定义位于 CSchoolMgr.cpp 内部；因 CGameManager.cpp 需真实调用 AddUser
// （原 asm 桥 _ZN10CSchoolMgr7AddUserEP5CUser）且 Village.cpp 需
// SetSchoolArea，拆到本头作为唯一声明点（CSchoolMgr.cpp / Village.cpp 亦 include）。
// 布局（ORIG ctor 0x856f206 / dtor 0x856f0ba 推导）：
//   CSchoolMgr ：+0x00 std::map<uint,uint> m_schoolUserCount
//                +0x18 CSchoolArea m_schoolArea（sizeof 0x28）
//   CSchoolArea：+0x00 std::map<uint,int> m_areaMap
//                +0x18 std::stack<int> m_areaStack

#include <map>
#include <stack>

class CUser;

// ---------------------------------------------------------------------------
// CBelong（send_group 归属对象抽象基类）与 CSchoolMember
// （CBelong(+0 vptr) + CUser*，sizeof 8；IsRight 覆写纯虚。0x085690b6 W）
// ---------------------------------------------------------------------------
class CBelong
{
public:
    CBelong();
    virtual bool IsRight(CUser* user) = 0;
};

class CSchoolMember : public CBelong
{
public:
    CSchoolMember(CUser* user);             // 0x085690b6 W
    virtual bool IsRight(CUser* user);      // 0x085690dc W

    CUser* m_user;   // +0x04
};

// ---------------------------------------------------------------------------
// CSchoolArea（学院分区：area 号映射 + 空闲区号栈）
// ---------------------------------------------------------------------------
class CSchoolArea
{
public:
    CSchoolArea();                 // 0x0856f18c W
    ~CSchoolArea();                // 0x0856f074 W

    int GetAreaIndex();                       // 0x085689a6 W
    void SetAreaMapping(unsigned int schoolNo, int areaIdx);  // 0x085689ec W
    void AddSchoolArea(unsigned int schoolNo);                // 0x08568a3e W
    void DelSchoolArea(unsigned int schoolNo);                // 0x08568a74 W
    void SetSchoolArea(int start, int end);                   // 0x086d23bc W
    int GetAreaIndex(unsigned int schoolNo);                  // 0x086d23f8 W

    std::map<unsigned int, int> m_areaMap;   // +0x00
    std::stack<int> m_areaStack;             // +0x18
};

// ---------------------------------------------------------------------------
// CSchoolMgr
// ---------------------------------------------------------------------------
class CSchoolMgr
{
public:
    CSchoolMgr();                 // 0x0856f206 W
    ~CSchoolMgr();                // 0x0856f0ba W

    void AddUser(CUser* user);                // 0x085683a0 T
    void DelUser(CUser* user);                // 0x085684bc T
    void SendBroadcastUserIn(CUser* user);    // 0x08567fe8 T
    void SendBroadcastUserOut(CUser* user);   // 0x085681ce T
    unsigned int GetUserCount(unsigned int schoolNo);  // 0x084ed394 W（cpp 访问 m_schoolUserCount）
    void SetSchoolArea(int start, int end);   // 0x086d245c W
    int GetSchoolArea(unsigned int schoolNo); // 0x086d2480 W

    std::map<unsigned int, unsigned int> m_schoolUserCount;  // +0x00
    CSchoolArea m_schoolArea;                                 // +0x18
};

// 全局单例（定义在 CGameManager.cpp）
// ORIG nm -S：g_schoolMgr = BSS @0x0943e080 size 0x58（对象本体）；
// InterDispatcher.cpp 调用点 ORIG 0x084d8279 `movl $0x943e080,(%esp)` 取符号
// 地址立即数作 this ⇒ 对象声明（指针声明反产 `mov <A>,%eax` 装载形）。
extern CSchoolMgr g_schoolMgr;

#endif  // DNF_GAME_CSCHOOL_MGR_H_

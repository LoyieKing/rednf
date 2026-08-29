#ifndef DNF_GAME_CREATEDNFEVENTSCIPRT_H_
#define DNF_GAME_CREATEDNFEVENTSCIPRT_H_

// df_game_r Game/ CreateDnfEventSciprt（ARAD EventCreateDnf 脚本服务，全局命名空间）。
//
// ORIG（df_game_r, elf32-i386, nm -S 实证）：
//   class CreateDnfEventSciprtServer      vtable _ZTV26CreateDnfEventSciprtServer @0x8b7cba0
//     （8 字节 offset-to-top/typeinfo + 两槽 [Load 0x0816451e, Destroy 0x08164590]，
//      无虚析构槽——与 ARAD::ScriptInterface 契约一致）
//     C1 _ZN26CreateDnfEventSciprtServerC1Ev W @0x081648ac（0x35）
//     D1 _ZN26CreateDnfEventSciprtServerD1Ev W @0x081648e2（0x1f）
//     Load _ZN26CreateDnfEventSciprtServer4LoadEv T @0x0816451e（0x71）
//     Destroy _ZN26CreateDnfEventSciprtServer7DestroyEv T @0x08164590（0xa）
//   class CreateDnfEventSciprt            typeinfo @0x8b7ccc0
//     C1/C2 T @0x08ac000c（构造 map<int,vector<ItemInfo>>）
//     D1/D2 T @0x08ac0020
//     loadScript T @0x08ac0034（0x2f5）
//     clearScript T @0x08ac032a
//     GetItemInfo(vector<pair<int,int>>&, int) T @0x08ac03a6
//     GetItemInfo(vector<pair<int,int>>&) T @0x08ac04a4
//   class CreateDnfEventSciprt::ItemInfo
//     C1(int,int) W @0x08ac063e / D1(D2) W @0x08ac1884
//
// Server 布局（C1 0x081648ac 反汇编）：
//   +0x00 ScriptInterface vptr
//   +0x04 CreateDnfEventSciprt m_script（C1 先调 _ZN20CreateDnfEventSciprtC1Ev(this+4)，
//        再 Singleton 基 C1、ScriptInterface 基 C1，最后写 vptr 0x8b7cba8；
//        D1 仅写 vptr 后调 _ZN20CreateDnfEventSciprtD1Ev(this+4)，无基析构调用）
//
// 类名 Spiprt 为 ORIG 拼写原样（勿"纠正"为 Script，否则 mangled 全部失配）。

#include <map>
#include <vector>
#include <string>

#include "Arad_DataManager.h"   // ARAD::Singleton<T> 模板、ARAD::ScriptInterface

class CreateDnfEventSciprtServer;

namespace ARAD { template <class T> class Singleton; }  // Arad_DataManager.h 已含

// ----------------------------------------------------------------------------
// CreateDnfEventSciprt —— 脚本数据子对象（Server @+0x04）。
// ----------------------------------------------------------------------------
class CreateDnfEventSciprt
{
public:
    // ORIG ItemInfo C1(i,i) 0x08ac063e：arg1 写 +0x00（32 位）、arg2 低 16 位写 +0x04。
    // GetItemInfo 经 make_pair<unsigned long&, short&> 转发两字段（_ZSt4pairImsEC1IRmRsE
    // 链实证 first 为 unsigned long、second 为 short），故字段类型照此声明。
    struct ItemInfo
    {
        unsigned long m_itemIndex;   // +0x00（ctor int 入参截断写入；【推断】=物品索引）
        short m_count;               // +0x04（ctor int 入参取低 16 位符号写；【推断】=数量）

        inline ItemInfo(int itemIndex, int count)
            : m_itemIndex(itemIndex)
            , m_count(static_cast<short>(count))
        {
        }
    };

    CreateDnfEventSciprt();          // ORIG C1 0x08ac000c：map 默认构造
    ~CreateDnfEventSciprt();         // ORIG D1 0x08ac0020：map 析构（元素 vector 自清）

    // ORIG 0x08ac0034 loadScript：loadRDARScriptFile("Event/", "tw_CreateDnf.evt") 失败返 -1；
    // 逐 token 解析 [step]/[day]/[reward]/[/step] 段；重复 [level] printf 后返 -1。
    int loadScript();

    // ORIG 0x08ac032a clearScript：【ORIG 原样】把每个 mapped value 的首 4 字节当堆指针
    // 显式析构 + operator delete，且循环体内无迭代器自增、末尾 map.clear()——逐指令转写，
    // 不做安全化修改。
    void clearScript();

    void GetItemInfo(std::vector<std::pair<int, int> >& out);                 // ORIG 0x08ac04a4
    void GetItemInfo(std::vector<std::pair<int, int> >& out, int dayLevel);   // ORIG 0x08ac03a6

    std::map<int, std::vector<ItemInfo> > m_itemMap;   // +0x00（脚本按 day 级分组）
};

// ----------------------------------------------------------------------------
// CreateDnfEventSciprtServer —— Singleton + ScriptInterface 双基服务包装。
// ----------------------------------------------------------------------------
class CreateDnfEventSciprtServer : public ARAD::Singleton<CreateDnfEventSciprtServer>,
                                   public ARAD::ScriptInterface
{
public:
    CreateDnfEventSciprtServer();    // ORIG C1 0x081648ac
    ~CreateDnfEventSciprtServer();   // ORIG D1 0x081648e2（非虚）

    virtual bool Load();             // vtable 槽0（ORIG 0x0816451e）
    virtual int Destroy();          // vtable 槽1（ORIG 0x08164590 返 1；接口 void 化同
                                     // AvatarFixedHiddenOptionServer 先例）

    CreateDnfEventSciprt m_script;   // +0x04
};

#endif  // DNF_GAME_CREATEDNFEVENTSCIPRT_H_

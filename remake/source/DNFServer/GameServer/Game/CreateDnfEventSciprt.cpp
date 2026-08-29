// df_game_r Game/ CreateDnfEventSciprt 实现（独立新 TU，全局命名空间类）。
// 依据 docs/class_func_reports/CreateDnfEventSciprtServer/{Load,Destroy}.md、
// docs/class_func_reports/CreateDnfEventSciprt/{loadScript,clearScript,
// GetItemInfo}.md 与 ORIG 反汇编逐段转写。
//
// 注册链（ARAD.cpp Init #8，待 ARAD 域批次接入）：
//   RegistScript(Singleton<CreateDnfEventSciprtServer>::Get(), 0x13)
#include "CreateDnfEventSciprt.h"

#include <cstdio>

#include "DNFLexWrapper.h"    // loadRDARScriptFile / unloadRDARScriptFile /
                              // ScanType / ScanInt / ScanTypeOrInt
#include "GameTypes.h"        // cMyTrace

namespace
{
typedef std::vector<CreateDnfEventSciprt::ItemInfo> ItemInfoVector;
}  // namespace

// ============================================================================
// CreateDnfEventSciprt
// ============================================================================

// ORIG _ZN20CreateDnfEventSciprtC1Ev @0x08ac000c：
// this+4 处 m_itemMap 默认构造（map Rb_tree impl 初始化）。
CreateDnfEventSciprt::CreateDnfEventSciprt()
{
}

// ORIG _ZN20CreateDnfEventSciprtD1Ev @0x08ac0020：m_itemMap 逆序析构。
CreateDnfEventSciprt::~CreateDnfEventSciprt()
{
}

// ORIG loadScript @0x08ac0034（0x2f5 字节）。token 循环；
// 各段失败路径 → unloadRDARScriptFile + return 0；
// duplicate [level] → printf 后直接 return -1（不 unload，ORIG 同）。
int CreateDnfEventSciprt::loadScript()
{
    if (!loadRDARScriptFile("Event/", "tw_CreateDnf.evt"))   // "tw_CreateDnf.evt" / "Event/"
        return -1;

    std::string token;              // -0x44
    bool isType = false;            // -0x45（ScanTypeOrInt 出参）
    bool isInt = false;             // -0x46
    int curDayLevel = 0;            // -0x4c（[day] 段记录）
    std::vector<ItemInfo> items;    // -0x58

    while (ScanType(token, true))                       // loop top 08ac008d
    {
        if (token != "[step]")                          // 08ac00ab
            continue;

        if (!ScanType(token, true))                     // 08ac00c2
            break;
        if (token == "[day]")                           // 08ac00e0
            curDayLevel = ScanInt((bool*)0);            // 08ac00f7
        if (!ScanType(token, true))                     // 08ac0106
            break;

        if (token == "[reward]")                        // 08ac0124
        {
            for (;;)                                    // reward 体 08ac013b..
            {
                int rewardIdx = ScanTypeOrInt(isType, token, isInt, true);
                if (isType)                             // 08ac015f：出参为真 → 结束 [reward]
                    break;
                int count = ScanInt((bool*)0);          // 08ac0167
                items.push_back(ItemInfo(rewardIdx, count));   // ItemInfo C1 + push_back(move)
            }
        }

        if (!ScanType(token, true))                     // 08ac01a4
            break;
        if (token != "[/step]")                         // 08ac01c2
            continue;

        // [/step] 收口（08ac01dd..）：查重后经 new 空向量 swap(items) 再以引用拷贝入 map。
        if (m_itemMap.find(curDayLevel) != m_itemMap.end())
        {
            printf("duplicate [level] %d", curDayLevel);       // 08ac022b
            return -1;                                  // 不调 unloadRDARScriptFile（ORIG 同）
        }
        std::vector<ItemInfo>* staged = new std::vector<ItemInfo>();  // operator new(0xc)
        staged->swap(items);
        m_itemMap.insert(std::make_pair(curDayLevel, *staged));
        // 注【ORIG 原样】：staged 本体未 delete——ORIG 经 pair(const int&, vector&)
        // 引用把内容拷入节点后即离开作用域；staged 由 clearScript 的指针式回收承接。
    }

    unloadRDARScriptFile();                             // 08ac02c8 收尾路径
    return 0;
}

// ORIG clearScript @0x08ac032a（0x7b 字节）。逐指令转写【ORIG 原样，含缺陷】：
// 循环体内无迭代器自增，且把 mapped value 首 4 字节当堆指针做显式析构 + delete。
void CreateDnfEventSciprt::clearScript()
{
    std::map<int, std::vector<ItemInfo> >::iterator it = m_itemMap.begin();
    for (; it != m_itemMap.end() /* 无 ++it，ORIG 原样 */; )
    {
        std::vector<ItemInfo>* victim =
            reinterpret_cast<std::vector<ItemInfo>*>(
                *reinterpret_cast<void**>(&it->second));        // 读 second 前 4 字节
        if (victim != NULL)                                     // 08ac036b
        {
            victim->~ItemInfoVector();                          // 显式 ~vector（08ac06fa）
            ::operator delete(victim);                          // _ZdlPv（087244f0）
        }
    }
    m_itemMap.clear();                                          // 08ac08e2
}

// ORIG GetItemInfo(vector&) @0x08ac04a4（0x13d 字节）：合并所有 day 组的全部元素。
void CreateDnfEventSciprt::GetItemInfo(std::vector<std::pair<int, int> >& out)
{
    out.clear();
    std::map<int, std::vector<ItemInfo> >::iterator it = m_itemMap.begin();
    for (; it != m_itemMap.end(); ++it)
    {
        std::vector<ItemInfo>& vec = it->second;
        for (unsigned int i = 0; i < vec.size(); ++i)   // unsigned long/short → pair<int,int>
        {
            out.push_back(std::make_pair(vec.at(i).m_itemIndex, vec.at(i).m_count));
        }
    }
}

// ORIG GetItemInfo(vector&, int) @0x08ac03a6（0xfe 字节）：仅指定 day 组；
// find 未命中时不动 out（ORIG 同）。
void CreateDnfEventSciprt::GetItemInfo(std::vector<std::pair<int, int> >& out, int dayLevel)
{
    std::map<int, std::vector<ItemInfo> >::iterator it = m_itemMap.find(dayLevel);
    if (it == m_itemMap.end())
        return;
    out.clear();
    std::vector<ItemInfo>& vec = it->second;
    for (unsigned int i = 0; i < vec.size(); ++i)
    {
        out.push_back(std::make_pair(vec.at(i).m_itemIndex, vec.at(i).m_count));
    }
}

// ============================================================================
// CreateDnfEventSciprtServer
// ============================================================================

// ORIG C1 @0x081648ac：先 m_script(this+4) ctor → ARAD::Singleton<T> C1（空）→
// ScriptInterface C1（vptr 0x8b2db78）→ 写自身 vptr 0x8b7cba8。
CreateDnfEventSciprtServer::CreateDnfEventSciprtServer()
{
}

// ORIG D1 @0x081648e2：写 vptr 0x8b7cba8 后析构 m_script(this+4)；无基析构调用。
CreateDnfEventSciprtServer::~CreateDnfEventSciprtServer()
{
}

// ORIG Load @0x0816451e：PRETTY 串 "virtual bool CreateDnfEventSciprtServer::Load()"
// （.rodata 0x8b7c9a0），行号 0xd0 / 计数 9 / true,false → cMyTrace C1(PKc,iibb)；
// 打点 "- Loading EventCreateDnf script - "；return m_script.loadScript()==0。
bool CreateDnfEventSciprtServer::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0xd0, 9, true, false);
    trace("\t- Loading EventCreateDnf script - ");
    if (m_script.loadScript())
        return false;
    return true;
}

// ORIG Destroy @0x08164590：mov $0x1,%eax; ret（undefined4 返 1；接口 void 化沿用
// AvatarFixedHiddenOptionServer 先例，见 ScriptInterface vtable 两槽契约）。
int CreateDnfEventSciprtServer::Destroy()
{
    return 1;
}

// ============================================================================
// ORIG 单例链完整类族：_ZN4ARAD9SingletonI26CreateDnfEventSciprtServerEC1Ev（W
// 0x08164902）、Get（W 0x08164907）、guard var/obj（0x09404630/0x09404638）、
// GetPtr。本 TU 经下列可执行引用完成隐式实例化（inline 模板各 TU COMDAT 合并，
// 与 ARAD.cpp 将来的 #8 RegistScript 调用不冲突）；勿在他处重复显式实例化。
// ============================================================================
namespace
{
ARAD::ScriptInterface* instantiateCreateDnfEventSciprtServerSingleton()
{
    return ARAD::Singleton<CreateDnfEventSciprtServer>::Get();
}
}  // namespace

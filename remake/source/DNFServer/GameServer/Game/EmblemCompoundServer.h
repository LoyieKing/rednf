#ifndef DNF_GAME_EMBLEMCOMPOUNDSERVER_H_
#define DNF_GAME_EMBLEMCOMPOUNDSERVER_H_

// df_game_r Game/ EmblemCompoundServer + ARAD::SCRIPT::EmblemCompound。
//
// ORIG（elf32-i386）符号（nm 实证，均为 T）：
//   ARAD::SCRIPT::EmblemCompound
//     ctor  _ZN4ARAD6SCRIPT14EmblemCompoundC1Ev  0x088b1408
//     dtor  _ZN4ARAD6SCRIPT14EmblemCompoundD1Ev  0x088b14c2
//     loadScript   _ZN4ARAD6SCRIPT14EmblemCompound10loadScriptEv 0x088b154a
//     clearScript  _ZN4ARAD6SCRIPT14EmblemCompound11clearScriptEv 0x088b1926
//     getCost      _ZN4ARAD6SCRIPT14EmblemCompound7getCostEi      0x088b19b0
//     getResult    _ZN4ARAD6SCRIPT14EmblemCompound9getResultEi    0x088b1a14
//     getList      _ZN4ARAD6SCRIPT14EmblemCompound7getListEi      0x088b1a7e
//   EmblemCompoundServer（全局命名空间）
//     ctor C1 0x0819155e / dtor D1 0x08191594 / Test 0x081915b4 /
//     Load 0x081915ba / Destroy 0x0819162c
//     （CheckCondition 0x08191636 / GetEmblemInfo 0x08191818 /
//       GetEmblemRandomIndex 0x081919e8 / IsResultSelectable 0x08191ad6 /
//       Process 0x08191afa 尚未还原，见 docs/class_func_reports/EmblemCompoundServer/）
//
// 类形态（ORIG C1/D1 反汇编实证）：C1 先构造 +4 处脚本子对象、再调 ScriptInterface
// 基（写 vptr）、末写 vtable——即 SCRIPT 子对象是【第一个声明的非多态基类】（g++
// 多态基 ScriptInterface 作主基占 +0，非多态基按声明序排 +4）；D1 写 vtable 后析构
// +4 子对象。布局：+0 vptr；+4 EmblemCompound(0x84)；合计 0x88（Singleton obj bss
// 邻接间距 0x9405060→0x94050e8 实证）。

#include <map>
#include <vector>

#include "Arad_DataManager.h"   // ARAD::ScriptInterface / ARAD::Singleton<T>

namespace ARAD
{
namespace SCRIPT
{

class EmblemCompound
{
public:
    // ORIG loadScript 内 operator new(0xa) 裸分配的 POD（无用户构造函数，
    // 字段由 ScanInt 直接写入；+0x09 为对齐 pad）：
    //   +0x00 short 列表索引（getList 用其索引 m_list[7]）
    //   +0x02 short 物品 index
    //   +0x04 short 数量
    //   +0x06 short 保留字段
    //   +0x08 bool  （ScanInt()==1）
    struct Result
    {
        short m_listIdx;     // +0x00
        short m_itemIndex;   // +0x02
        short m_itemCount;   // +0x04
        short m_field6;      // +0x06
        bool m_selectable;   // +0x08
    };

    EmblemCompound();   // ORIG 0x088b1408：map×2 ctor + 7×vector ctor
    ~EmblemCompound();  // ORIG 0x088b14c2：逆序析构 7 vector → map(+0x18) → map(+0x00)

    // 返回：0=成功；-1=loadRDARScriptFile 失败；-2=grade 重复（ORIG printf 后返回）。
    int loadScript();       // ORIG 0x088b154a "Etc/EmblemRenewal/" emblem_compound_jpn.etc
    void clearScript();     // ORIG 0x088b1926：清 cost map、delete Result*、清 result map

    int getCost(int grade);             // ORIG 0x88b19b0：未命中 -1
    Result* getResult(int grade);       // ORIG 0x88b1a14：未命中 0
    std::vector<unsigned long>* getList(int grade);  // ORIG 0x88b1a7e：&m_list[ret->m_listIdx]，未命中 0

protected:
    // ORIG 原为 private；LinkClosureB.AradServers 改 protected 以便 Server 派生类
    // 按 ORIG 语义直接 map::operator[] 访问（签名/布局不变）。
    std::map<int, int> m_cost;          // +0x00
    std::map<int, Result*> m_result;    // +0x18（本工具链 std::map=0x18，与 ORIG 一致）
    std::vector<unsigned long> m_list[7];   // +0x30（size 合计 0x84）
};

}  // namespace SCRIPT

}  // namespace ARAD

namespace arad { struct SigAradEmblemCompound; }
class CUser;

class EmblemCompoundServer : public ARAD::SCRIPT::EmblemCompound,
                            public ARAD::Singleton<EmblemCompoundServer>,
                            public ARAD::ScriptInterface
{
public:
    EmblemCompoundServer();   // ORIG 0x0819155e：脚本基(+4) → Singleton(+0) →
                              //   → ScriptInterface(+0) → vtable（[b22] 补 Singleton 基调用）
    ~EmblemCompoundServer();  // ORIG 0x08191594：vtable + 脚本基 dtor

    virtual bool Load();      // ORIG 0x081915ba：cMyTrace + return loadScript()==0
    virtual int Destroy();   // ORIG 0x0819162c：return 1（基类约定 void）
    void Test();              // ORIG 0x081915b4：空（非虚，vtable 仅 Load/Destroy 两槽）

    // ORIG 0x08191636 CheckCondition / 0x08191afa Process
    // （Arad_INTER_kEmblemCompound 派发链调用）。仅声明，定义由 EmblemCompound
    // Server TU 批次提供。
    bool CheckCondition(CUser* user, arad::SigAradEmblemCompound* sig);
    bool Process(CUser* user, arad::SigAradEmblemCompound* sig);
    // ---- ORIG T（CheckCondition/Process 依赖；LinkClosureB.AradServers 定义归
    //      EmblemCompoundServer.cpp）----
    // ORIG _ZN20EmblemCompoundServer13GetEmblemInfoEP5CUserPN4arad21SigAradEmblemCompoundEi
    // @0x08191818：校验第 idx 条目槽位物品（isEmpty/lock/GetItemType()==0x14），
    // 通过则把 grade/itemIdx 写回 sig[idx*8]。
    bool GetEmblemInfo(CUser* user, arad::SigAradEmblemCompound* sig, int idx);
    // ORIG _ZN20EmblemCompoundServer20GetEmblemRandomIndexEit @0x081919e8
    int GetEmblemRandomIndex(int grade, unsigned short selectIdx);
    // ORIG _ZN20EmblemCompoundServer18IsResultSelectableEi @0x08191ad6
    bool IsResultSelectable(int grade);

private:
    EmblemCompoundServer(const EmblemCompoundServer&);
    EmblemCompoundServer& operator=(const EmblemCompoundServer&);
};

#endif  // DNF_GAME_EMBLEMCOMPOUNDSERVER_H_

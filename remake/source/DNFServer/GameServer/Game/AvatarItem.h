#ifndef DNF_GAME_AVATARITEM_H_
#define DNF_GAME_AVATARITEM_H_

// df_game_r Game/ WongWork::Avatar_Item（独立 TU，从 GameStubs.cpp 拆出）。
// 布局：m_expireDate(+0x00) / m_registTime(+0x04) / m_ipgAgencyNo(+0x08, 25B)
//       / m_emblemInfo(+0x21, 0x1eB) / m_color1(+0x3f) / m_color2(+0x41) / pad(+0x43)。
// 语义字段：m_emblemInfo 替代原 &m_ipgAgencyNo[0x19] 偏移访问。

#include "GamePODTypes.h"
#include "GameTypes.h"   // DynamicPool<T> 主模板：文件末尾 CNode 特化需先见主模板

namespace WongWork
{

// ORIG 布局为 packed：m_color1 @ +0x3f、m_color2 @ +0x41（setColor 写 0x3f/0x41；
// init 清 +0x3f 4 字节；getExpansionInfo 返 this+0x3f）。DynamicPool<Avatar_Item>
// MoreAlloc 节点步长 0x4c 实证 → sizeof = 0x44。
#pragma pack(push, 1)
class Avatar_Item
{
public:
    void init();
    int GetExpireDate() const;
    int GetRegistTime() const;
    int GetRemainDate(int tick) const;
    const char* GetIPGAgencyNo() const;
    const char* getJewelSocketData() const;
    void* getExpansionInfo() const;
    int* getExnansionInfoRef() const;
    // [b28d-Main·检查点18] 删除发明名 setColorEss（ORIG 无此符号）——语义等同 setColor。
    void setExpansionInfo(const ::stAvatarExpansionInfo_t& info);
    void SetData(int expireDate, int registTime, const char* ipgAgency);
    void setEmblemSocketData(const stAvatarEmblemInfo_t& emblem);
    // [b28d-Main·检查点18] 删除 `(int, unsigned long)` 重载声明（发明签名）——
    //   ORIG 仅 `(unsigned int, unsigned int)`（0x082fa2a6），下方已有声明与定义。
    // ---- G7 miscB 追加（ORIG 真实符号，定义 WongWorkMiscB_A.cpp）----
    void setColor(short color1, short color2);  // _ZN8WongWork11Avatar_Item8setColorEss W 0x082fa2fa（语义同 setColorEss）
    int updateEmblemSocket(unsigned int grade, unsigned int itemId);
    //   _ZN8WongWork11Avatar_Item18updateEmblemSocketEjj W 0x082fa2a6
    Avatar_Item() throw();                      // C1/C2 W 0x082fa34e（ORIG 空体，nm 实证重建遗漏，GmPool 探针）
    ~Avatar_Item();                             // D1/D2 W 0x082fa354（ORIG 空体）

    int m_expireDate;                          // +0x00
    int m_registTime;                          // +0x04
    char m_ipgAgencyNo[25];                    // +0x08（0x19 字节）
    stAvatarEmblemInfo_t m_emblemInfo;         // +0x21（0x1e 字节）
    short m_color1;                            // +0x3f（P211 同型化：ORIG setColor(ss)
    short m_color2;                            // +0x41  store 为 LHS-first，字段与形参
                                              //  同型翻正；pack(1) 偏移/尺寸不变）
    char m_pad43[1];                           // +0x43
};
#pragma pack(pop)

}  // namespace WongWork

// ---- DynamicPool<WongWork::Avatar_Item>::CNode 整类特化（须在首次隐式实例化前声明）----
// ORIG 实证（DynamicPool<WongWork::Avatar_Item>::MoreAlloc @0x082fac4e，报告
// /tmp/r_morealloc.md）：`sub $0x3c,%esp` + 寄存器循环（esi/ebx）+ **无**
// _Unwind_Resume/析构清理；主模板 CNode ctor 无空 EH 规范时 -O0 对
// `new CNode[n]` 生成 26 条 EH 清理（栈帧 0x4c、NEW size 0x19a vs ORIG 0x14b）。
// ⇒ 该实例的 CNode 构造带空 EH 规范（throw()；探针 /tmp/exp4/av 实证：仅 ctor
// 加 throw() 即逐指令同形）。throw() 不入 mangled ⇒ 符号名/ABI/sizeof(0x4c)/
// 偏移全不变；其余池实例（Stream/PacketBuf/SmallStream/BigStream/CEgg/
// CCreature）仍走主模板——ORIG 实证它们有清理且 NEW 已 identical/ae。
template <>
struct DynamicPool<WongWork::Avatar_Item>::CNode : public WongWork::Avatar_Item
{
    int m_type;    // +0x44（池魔数 0x8f21）
    char m_inUse;  // +0x48（占用计数）
    CNode() throw();
    ~CNode();
};

inline DynamicPool<WongWork::Avatar_Item>::CNode::CNode() throw()
{
    m_type = 0x8f21;
    m_inUse = 0;
}

inline DynamicPool<WongWork::Avatar_Item>::CNode::~CNode()
{
}

#endif  // DNF_GAME_AVATARITEM_H_

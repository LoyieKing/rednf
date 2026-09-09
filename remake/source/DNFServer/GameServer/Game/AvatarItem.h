#ifndef DNF_GAME_AVATARITEM_H_
#define DNF_GAME_AVATARITEM_H_

// df_game_r Game/ WongWork::Avatar_Item（独立 TU，从 GameStubs.cpp 拆出）。
// 布局：m_expireDate(+0x00) / m_registTime(+0x04) / m_ipgAgencyNo(+0x08, 25B)
//       / m_emblemInfo(+0x21, 0x1eB) / m_color1(+0x3f) / m_color2(+0x41) / pad(+0x43)。
// 语义字段：m_emblemInfo 替代原 &m_ipgAgencyNo[0x19] 偏移访问。

#include "GamePODTypes.h"

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

#endif  // DNF_GAME_AVATARITEM_H_

#ifndef GAME_CITEMMAKINGSKILL_H_
#define GAME_CITEMMAKINGSKILL_H_

// CItemMakingSkill 权威声明（StubS3A：charac_expand::CDataMgr slot9 @+0x28 依赖）。
// ORIG（df_game_r nm -S）：T 0x0854571c UpSkillLevel / 0x08545b7e GetPrevExpGap /
// 0x08545cf4 GetItemMakingSkillLevel / 0x08545d24 SetItemMakingSkillPoint /
// 0x08545a62 Send_ItemMakingSkillList；ctor 由 CDataMgr slot9 operator new(0x20) 构造。
#include "CBoosterGage.h"     // charac_expand::CData（pack(1) 基类）

#include "HistoryLogEnums.h"   // ENUM_ITEM_MAKING_TYPE（CUser.h:2031 同源）

class CMTRand;
class CStackableItem;
class CUser;

class CItemMakingSkill : public charac_expand::CData
{
public:
    CItemMakingSkill();                    // ORIG T 0x08545108（new CMTRand(time(0))）
    virtual ~CItemMakingSkill();           // ORIG D1 0x08545192（delete m_rand）

    // charac_expand::CData 纯虚覆盖（StubS3A 批；定义 StubS3A_deps.cpp；
    // getData 定义 CItemMakingSkill.cpp）。
    virtual bool loadData(CUser* pUser, char* pData);   // ORIG T 0x08545202
    virtual void getData(char* pData) const;            // ORIG T 0x08545b42
    virtual bool _saveData(CUser* pUser);               // ORIG T 0x08545276
    virtual void _reset();                              // ORIG T 0x085453ba
    int Send_ItemMakingSkillList(CUser* user) const;                       // ORIG T 0x08545a62
    int GetItemMakingSkillLevel(const CStackableItem* item, CUser* user);  // ORIG T 0x08545cf4
    int GetPrevExpGap(int level);                                          // ORIG T 0x08545b7e
    int GetSkillLevel(ENUM_ITEM_MAKING_TYPE type, CUser* user) const;
    void UpSkillLevel(ENUM_ITEM_MAKING_TYPE type, int point, CUser* user); // ORIG T 0x0854571c
    void GainItemMakingSkillPoint(const CStackableItem* item, CUser* user);
    void SetItemMakingSkillPoint(int skillIdx, int point, CUser* user);    // ORIG T 0x08545d24

    char m_pad5;                        // +0x05（1 字节；语义未还原 [推断]）
    unsigned short m_skillLevels[11];   // +0x06..+0x1b（11 项等级表）
    CMTRand* m_rand;                    // +0x1c（ctor new CMTRand(time(0))，dtor delete）
};

#endif  // GAME_CITEMMAKINGSKILL_H_

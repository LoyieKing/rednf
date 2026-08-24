// df_game_r Game/ expert_job 核心类型（按 ORIG 反汇编重建布局）。
// 证据：
//   CExpertJob::C1          0x849f2b4（仅置 vptr 0x8c7cb88）
//   CExpertJob::D1/D0       0x849f1fa / 0x849f220（虚析构）
//   ExpertJobMaterial::C1   0x849f23e -> Reset 0x849f252（清零 +0x00..+0x08、+0x0c/+0x0e）
//   CDisjointMachine::C1    0x85d3d7e（IObject 基类 + Rectangle@+4 + int@+0x14 + string@+0x18）
//   CDisjointMachine::D1    0x85d3eb6
//   collision_detection::Rectangle::C1 0x85cb45a（Point(0,0)@+0 + int@+8 + int@+0xc）
//   CDisjointer::C1/D1      0x85d1d3c / 0x85d1d8a（vptr 0x8cc0828；CDisjointMachine@+0x8）
//   CEnchanter::C1/D1       0x849f2c2 / 0x849fd86（vptr 0x8c7cb68；ExpertJobMaterial[2]@+0x8 步长 0x10）
#ifndef DNF_CEXPERTJOB_H
#define DNF_CEXPERTJOB_H

#include <string>

#include "CBoosterGage.h"   // charac_expand::CData（CCharacExpertJob 基类）
#include "CVillageObjectMgr.h"
#include "STExpertJobScript.h"  // STExpertJobScript（CCharacExpertJob::learnRecipe 依赖）
#include "STItemScript.h"   // ENUM_EXPERT_JOB_TYPE

class CUser;
class CStackableItem;
class PacketGuard;

namespace collision_detection
{

struct Point
{
    Point(int x, int y) : m_x(x), m_y(y) {}

    int m_x;
    int m_y;
};

class Rectangle
{
public:
    Rectangle();

    // 字段具名依据 ORIG private_store::CPrivateStorePos::CPrivateStorePos(store,x,y)
    // （0x85cb7fc）：rect.x = x-0x50、rect.y = y-0x32、+8 = 0xa0、+0xc = 0x64，
    // 即 {位置, 宽, 高}；成员公开与 ORIG 各 ctor 直写一致。
    Point m_pos;    // +0x00
    int m_width;    // +0x08【推断：宽】
    int m_height;   // +0x0c【推断：高】

    bool include(Rectangle const& other) const;   // ORIG W 0x85cb492
};

}  // namespace collision_detection

namespace expert_job
{

class ExpertJobMaterial
{
public:
    ExpertJobMaterial();
    void Reset();   // ORIG 0x849f252

private:
    unsigned int m_field00;
    unsigned int m_field04;
    unsigned int m_field08;
    unsigned short m_field0c;
    unsigned short m_field0e;
};  // sizeof == 0x10

class CExpertJob
{
public:
    CExpertJob();
    virtual ~CExpertJob();
    virtual int GetType();          // ORIG 0x8234796：return +0x04
    void SetType(ENUM_EXPERT_JOB_TYPE type);   // ORIG 0x849f148（写 +0x04）

    // ORIG 静态工具（this 不作为实例状态，实为 CUser* 参数）：
    //   IsSuccess       0x0849b462 T
    //   GetSuccessRate  0x0849b2e8 T
    //   IsLearnRecipe   0x0849b590 T
    //   IncreaseExpertJobExp 0x0849ae40 T
    static bool IsSuccess(CUser* user, CStackableItem const* item, int& resultExp);
    static stCompoundRate* GetSuccessRate(CUser* user, CStackableItem const* item,
                                          int& variation);
    static bool IsLearnRecipe(CUser* user, CStackableItem const* item);
    static void IncreaseExpertJobExp(CUser* user, int exp);

protected:
    int m_type;         // +0x04（职业类型；派生类成员起于 +0x8）
};

// CCharacExpertJob：专家职业角色扩展数据（charac_expand 槽 0）。基类
// charac_expand::CData（CBoosterGage.h）提供 vptr/+0x04，成员起于 +0x06。
// 布局（ORIG 反汇编）：
//   +0x06 unsigned short m_giveupCount
//   +0x08 int m_recipeInfo        （InitScript 由 script+0x274 写入）
//   +0x0c int m_field0c
//   +0x10 unsigned char m_recipeFlag[0x1e]（240 位 recipe bitmap；_reset 清 30 字节）
class CCharacExpertJob : public charac_expand::CData
{
public:
    CCharacExpertJob();                    // ORIG W 0x0832af52（基类 CData 构造）
    virtual ~CCharacExpertJob();           // ORIG W 0x0832bd52 / 0x0832bd22

    bool InitScript(STExpertJobScript* script);       // ORIG 0x0832b36c
    int get_recipe(int index) const;                  // ORIG 0x0832b306
    int set_recipe(int index);                        // ORIG 0x0832b264
    int learnRecipe(unsigned long recipeIndex, STExpertJobScript* script, int param); // ORIG 0x0832b3c0

    // charac_expand::CData 纯虚实现
    virtual bool loadData(CUser* pUser, char* pData);  // ORIG 0x0832b1fe
    virtual bool _saveData(CUser* pUser);              // ORIG 0x0832b0ac
    virtual void _reset();                             // ORIG 0x0832b064

    unsigned short get_giveup_count() const;           // ORIG W 0x0849f5a0
    void set_giveup_count(unsigned short c);           // ORIG W 0x0849f5ac

private:
    unsigned short m_giveupCount;    // +0x06
    int m_recipeInfo;                // +0x08
    int m_field0c;                   // +0x0c
    unsigned char m_recipeFlag[0x1e];// +0x10
};

class CDisjointMachine : public village_object::IObject
{
public:
    CDisjointMachine();                     // ORIG 0x85d3d7e
    ~CDisjointMachine();                    // ORIG 0x85d3eb6
    void reset();                           // ORIG 0x85d3e1e（village_object::IObject）
    bool put_object_info(PacketGuard& p);   // ORIG 0x85d1cc2（village_object::IObject）
    void put_disjoint_machine_info(PacketGuard& p);  // ORIG 0x85d1bc0

private:
    collision_detection::Rectangle m_rect;  // +0x04
    CUser* m_user;                          // +0x14（构造置空）
    std::string m_name;                     // +0x18
    int m_field1c;                          // +0x1c
    char m_field20;                         // +0x20
    unsigned short m_field24;               // +0x24
    unsigned short m_field28;               // +0x28
};

class CDisjointer : public CExpertJob
{
public:
    CDisjointer();
    ~CDisjointer();

private:
    CDisjointMachine m_machine;     // +0x08
};

class CEnchanter : public CExpertJob
{
public:
    CEnchanter();
    ~CEnchanter();

private:
    ExpertJobMaterial m_materials[2];   // +0x08，步长 0x10
};

}  // namespace expert_job

#endif  // DNF_CEXPERTJOB_H

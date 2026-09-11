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

// ENUM_PEER_REQUEST_TYPE 守卫定义（完整值表权威在 CUser.h，两处一致；
// CExpertJobMgr::On*Peer 签名需此类型可见）。
#ifndef DNF_ENUM_PEER_REQUEST_TYPE_DEFINED
#define DNF_ENUM_PEER_REQUEST_TYPE_DEFINED
enum ENUM_PEER_REQUEST_TYPE { ENUM_PEER_REQUEST_TYPE_0 = 0 };
#endif

#include <string>
#include <map>
#include "GameTypes.h"   // [Q3PrettSweepA] IntPairVector（ORIG prett 拼写直证）

// ---- OnlyOrig_CreatureJob/EJEnchanter 批：全局 POD（ORIG mangled 无 namespace
//      包裹，P11stItemPos_t / P18MSG_STATIC_COMMAND；布局按使用点反汇编实证）----
#ifndef DNF_GAME_STITEMPOS_T_DEFINED
#define DNF_GAME_STITEMPOS_T_DEFINED
#pragma pack(push, 1)
struct stItemPos_t
{
    unsigned char m_itemSpace;   // +0x00
    unsigned short m_slot;       // +0x01（ORIG _TryEnchantMine movzwl 0x1(%eax) 实证）
};                               // sizeof 0x3（pack(1)）
#pragma pack(pop)
#endif
#ifndef DNF_GAME_MSG_STATIC_COMMAND_DEFINED
#define DNF_GAME_MSG_STATIC_COMMAND_DEFINED
#pragma pack(push, 1)
struct MSG_STATIC_COMMAND
{
    unsigned char m_pad00[0xd];  // +0x00..+0x0c
    unsigned char m_type0d;      // +0x0d（类别 byte）
    // +0x0e（find_item 参数 = 内联 stItemPos_t 起点；ORIG OnPutItemForEnchant 访问
    // +0xe 实证）。匿名 union 双视角（P235/P254）：OnPutItemForEnchant 走 int 视角
    // （与 _PutItemForEnchant(int) 同型 ⇒ 单段 `mov 0xe(%eax),%edx`）；
    // OnTryEnchant 走 unsigned 视角（与 find_item(int) 异型 ⇒ 4.4 复现 ORIG 两段
    // 升格 `mov 0xe(%eax),%eax; mov %eax,%ebx`，探针 /tmp/probe_p254d.cpp f1/f2 实证）。
    // sizeof 0x12 / 偏移 +0xe / mangled 均不变。
    union
    {
        int m_value0e;
        unsigned int m_value0eU;
    };
};                               // sizeof 0x12（pack(1)）
#pragma pack(pop)
#endif
// ORIG _Z11EnableWorldPK5CItemS1_ T 0x0849d768（全局自由函数；Enchanter 附魔
// 可用性判定调用；定义归属待定批，链接缺口见批次报告）。
class CItem;       // EnableWorld / ExpertJobMaterial 返回类型（前置即可）
struct DisJointResult;
bool EnableWorld(const CItem* a, const CItem* b);

#include "CBoosterGage.h"   // charac_expand::CData（CCharacExpertJob 基类）
#include "CVillageObjectMgr.h"
#include "STExpertJobScript.h"  // STExpertJobScript（CCharacExpertJob::learnRecipe 依赖）
#include "STItemScript.h"   // ENUM_EXPERT_JOB_TYPE
#include "StaticPool.h"     // CExpertJobFactory 双池
#include "CExpertJobList.h"  // STExpertJobEtcScript 完整类型

// STExpertJobEtcScript 完整类型经 CExpertJobList.h（expert_job 域内）

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
// ---- EJEnchanter 批枚举（占位命名风格同 CLOSE_MISSION；数值由报告分支实证：
//      TRY 0=自家材料/1=对家材料/2=关闭；AGREE 0=取消/1=执行/2=拒绝）----
enum ENUM_TRY_ENCHANT_TYPE
{
    ENUM_TRY_ENCHANT_TYPE_0 = 0,
    ENUM_TRY_ENCHANT_TYPE_1 = 1,
    ENUM_TRY_ENCHANT_TYPE_2 = 2
};

enum ENUM_AGREE_ENCHANT_TYPE
{
    ENUM_AGREE_ENCHANT_TYPE_0 = 0,
    ENUM_AGREE_ENCHANT_TYPE_1 = 1,
    ENUM_AGREE_ENCHANT_TYPE_2 = 2
};

namespace expert_job
{

// ORIG mangled _ZN10expert_job20ENUM_CLOSE_MISSION_TYPE（OnCloseMission 第 2 参）；
// 数值语义仅 1/2 在 OnCloseMission 中被比较（发送 0x9e 包），其余未证，占位命名。
enum ENUM_CLOSE_MISSION_TYPE
{
    ENUM_CLOSE_MISSION_TYPE_0 = 0,
    ENUM_CLOSE_MISSION_TYPE_1 = 1,
    ENUM_CLOSE_MISSION_TYPE_2 = 2
};



class ExpertJobMaterial
{
public:
    ExpertJobMaterial();
    void Reset();   // ORIG 0x849f252
    bool Empty();   // 【objdump】ORIG W 0x0849f286
    int SetData(CUser* user, unsigned short area, unsigned short slot);   // ORIG T 0x0849bf08
    int IsRecipe();   // ORIG T 0x0849c114
    int CheckType(bool checkStackable, int itemType);   // ORIG T 0x0849bfd0
    int IsRight();    // ORIG T 0x0849c06e
    CItem* FindMonsterCard(const std::vector<std::pair<int, int> >& materials);   // ORIG T 0x0849c612
    CItem* GetEnchantItem();                       // ORIG T 0x0849c6ca
    int IsExistMaterialOfRecipe();                 // ORIG T 0x0849c166
    // [Q3PrettSweepA 2026-09-11] 拼写对齐 ORIG prett（同底层类型，mangled 不变）：
    // `bool expert_job::ExpertJobMaterial::DelMaterialOfRecipe(CUser*, const IntPairVector&,
    // int, PacketGuard*, bool&)` size 0x112。
    bool DelMaterialOfRecipe(CUser* user, const IntPairVector& materials,
                             int slot, PacketGuard* packet, bool& changed);   // ORIG T 0x0849c204
    int UseMaterialOfRecipe(PacketGuard* packet);  // ORIG T 0x0849c536

private:
    CUser* m_user;       // +0x00（RawOffsetCleanupB：SetData 0x0849bf08 实证。
                         //   b28 指针字段化：ORIG 3 段 LHS-first 直存形=同型指针赋值）
    CItem* m_item;       // +0x04（同上：ORIG `mov 0x8(%ebp),%edx; mov %eax,0x4(%edx)`
                         //   指针域直存 find_item 返回值）
    unsigned int m_itemIndex;   // +0x08
    unsigned short m_slot;      // +0x0c
    unsigned short m_field0e;   // +0x0e
    friend class CEnchanter;   // ORIG _DoEnchant/_TryEnchantMine 直读 private 字段
};  // sizeof == 0x10

// ---------------------------------------------------------------------------
// CExpertJob —— 共享虚表基类。
// ORIG vtable 实证（0x8cc0820 CDisjointer / 0x8c7cb60 CEnchanter /
// 0x8cbfa40 CAlchemist 三表槽序一致）：
//   slot0 OnStartMission(CUser*, CUser*)                    0x85d2a16 / 0x849c7b6 / pure
//   slot1 OnCloseMission(CUser*, ENUM_CLOSE_MISSION_TYPE)   0x85d2b74 / 0x849c822 / pure
//   slot2 IsAvailableJob()                                  0x849f596 / 0x849f30c / pure
//   slot3 Reset()                                           0x85d2cf2 / 0x849c71e / pure
//   slot4/5 D1/D0 居末。
// GetType（W 0x8234796）不在 vtable → 非虚成员。
// ---------------------------------------------------------------------------
class CExpertJob
{
public:
    CExpertJob();
    virtual void OnStartMission(CUser* targetUser, CUser* requestUser) = 0;
    virtual void OnCloseMission(CUser* user, ENUM_CLOSE_MISSION_TYPE type) = 0;
    virtual bool IsAvailableJob() = 0;
    virtual void Reset() = 0;
    virtual ~CExpertJob();

    int GetType();          // ORIG W 0x8234796：return +0x04（非虚）
    void SetType(ENUM_EXPERT_JOB_TYPE type);   // ORIG W 0x849f148（写 +0x04）

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
    ENUM_EXPERT_JOB_TYPE m_type;  // +0x04（职业类型；派生类成员起于 +0x8。
                                  //  b28：字段枚举化使 SetType store 序 = ORIG
                                  //  LHS-first（探针 /tmp/l3/p1.cpp fC））
};

// CCharacExpertJob：专家职业角色扩展数据（charac_expand 槽 0）。基类
// charac_expand::CData（CBoosterGage.h）提供 vptr/+0x04，成员起于 +0x06。
// 布局（ORIG 反汇编）：
//   +0x06 unsigned short m_giveupCount
//   +0x08 int m_endurance        （InitScript 由 script+0x274 写入）
//   +0x0c int m_enchantCount
//   +0x10 unsigned char m_recipeFlag[0x1e]（240 位 recipe bitmap；_reset 清 30 字节）
class CCharacExpertJob : public charac_expand::CData
{
public:
    CCharacExpertJob();                    // ORIG W 0x0832af52（基类 CData 构造）
    virtual ~CCharacExpertJob();           // ORIG W 0x0832bd52 / 0x0832bd22

    bool InitScript(STExpertJobScript* script);       // ORIG 0x0832b36c
    // [b28d-62] 返回类型 int→bool（P292/P248 档①）：ORIG 两处调用点均为
    // `call get_recipe; xor $0x1,%eax; test %al,%al; je`（IsLearnRecipe 0x0849b6b0）
    // 与 `xor $1; jne`（SendExpertJobInfo 0x0832b454）⇒ callee 实为 bool。
    // 返回类型不入 mangled，符号/ABI/布局零影响；宽度语义仍 0/1。
    bool get_recipe(int index) const;                 // ORIG 0x0832b306
    int set_recipe(int index);                        // ORIG 0x0832b264
    bool learnRecipe(unsigned long recipeIndex, STExpertJobScript* script, int param); // ORIG 0x0832b3c0 调用点 0x085d17d5 test%al 直测(S23C 取证,Main 代落)

    // charac_expand::CData 纯虚实现
    virtual bool loadData(CUser* pUser, char* pData);  // ORIG 0x0832b1fe
    // 分解机 getter（ORIG W 0x85d3e66/0x85d3e8e；定义 ExpertJobLink4.cpp）
    int get_disjoint_machine_endurance() const;
    int get_disjoint_machine_grade() const;
    void set_disjoint_machine_endurance(int value);   // ORIG W 0x85d3e72：alter()+写+0x08
    void set_disjoint_machine_grade(int value);       // ORIG W 0x85d3e9a：alter()+写+0x0c
    virtual bool _saveData(CUser* pUser);              // ORIG 0x0832b0ac
    virtual void _reset();                             // ORIG 0x0832b064

    unsigned short get_giveup_count() const;           // ORIG W 0x0849f5a0
    void set_giveup_count(unsigned short c);           // ORIG W 0x0849f5ac
    int SendExpertJobInfo(CUser* user);   // ORIG T 0x0832b454（定义 ExpertJobOnlyMgr.cpp）

private:
    unsigned short m_giveupCount;    // +0x06
    int m_endurance;                 // +0x08（RawOffsetCleanupB：SetData 0x0849bf08 实证；
                                     //   InitScript 由 script->m_enduranceInit 写入）
    int m_enchantCount;              // +0x0c（同上实证；分解机等级 getter 消费）
    unsigned char m_recipeFlag[0x1e];// +0x10
};

class CDisjointMachine : public village_object::IObject
{
public:
    CDisjointMachine();                     // ORIG 0x85d3d7e
    CDisjointMachine(CUser* user, const char* name, int level, char dir,
                     int x, int y);   // ORIG（定义 ExpertJobLink4.cpp）
    CDisjointMachine& operator=(CDisjointMachine const& other);  // ORIG W 0x085d3ed6
    ~CDisjointMachine();                    // ORIG 0x85d3eb6
    void reset();                           // ORIG 0x85d3e1e（非虚普通成员，不在 IObject vtable）
    bool put_object_info(PacketGuard& p);   // ORIG 0x85d1cc2（vtable slot1 覆写）
    void put_disjoint_machine_info(PacketGuard& p);  // ORIG 0x85d1bc0
    bool include(const village_object::IObject& other) const;   // vtable slot0 覆写
                                               // ORIG W 0x85d3de6：转调 m_rect.include


    // ---- getter（ORIG W 0x85d3e06 / 0x082347e0 / 0x85d3e12）----
    int get_cost() const;       // 读 +0x1c
    CUser* get_user() const;    // 读 +0x14
    char get_grade() const;     // 读 +0x20（movzbl）
private:
    collision_detection::Rectangle m_rect;  // +0x04
    CUser* m_user;                          // +0x14（构造置空）
    std::string m_name;                     // +0x18
    int m_field1c;                          // +0x1c
    char m_field20;                         // +0x20
    unsigned char m_pad21;                  // +0x21（ORIG 布局对齐填充）
    int m_field24;                          // +0x24（ORIG ctor/reset 以 movl 清零、put_short 取低16）
    int m_field28;                          // +0x28（同上）
};

// CAlchemist（炼金术士；dtor D1/D0 0x085d11d0/0x085d1200；定义 ExpertJobLink4.cpp）
class CAlchemist : public CExpertJob
{
public:
    CAlchemist();
    ~CAlchemist();
    // ORIG T 0x085d1424：反汇编参数自 0x8 起（无 this 槽），静态成员语义。
    // [b28d-Main·检查点14 P292] int→bool：ORIG 调用点 `call;xor $1;test %al`；mangled 不变。
    static bool OnTeleportCharacter(CUser* user, int vill, int area, int x, int y,
                                    bool flag);   // ORIG T 0x085d1424

    // ---- OnlyOrig_CreatureJob 批追加（定义 ExpertJobOnlyMgr.cpp）----
    static void UseDelilah(CUser* user);                     // ORIG T 0x085d18ae（体内无 this）
    static void UseRevivalItem(CUser* user);                 // ORIG T 0x085d1694（同上）
    static bool GetAssaultPrevent(CUser* user, bool* flag);  // ORIG T 0x085d137c（ORIG 无 this 槽）；[P354/P292] ORIG 调用点 `call; test %al,%al` ⇒ 8 位返回（原 int 误标）
    static int OnUseStackableItem(CUser* user, unsigned long uid,
                                  const CItem* item);        // ORIG T 0x085d1712（LinkClosureA-2 改 static：ORIG 调用点 0x865e42e 无 ecx 设置、mangled 无 this 编码实证）
    static int GetQuestItemDropRate(CUser* user, int itemIdx);   // ORIG T 0x085d12ee（ORIG 无 this 槽）
    // [IG1 2026-09-10 S2 授权] 返回类型 int→bool：ORIG 调用点 0x0865e177..0x0865e181
    // 为 `call; xor $0x1,%eax; test %al,%al; je`（P248 档①/P292）⇒ callee bool。
    // 返回类型不入 mangled、不改类布局 ⇒ 零 ABI 风险（定义侧机器码/尺寸 0x97 实测不变）。
    static bool CheckUsableStackableItem(CUser* user, const CItem* item);   // ORIG T 0x085d18e2 sz0x97（同上 static 实证）
    // ORIG T 0x085d121e——实现由 ItemUpgradeVtableB2.cpp 提供（同名局部类待收敛）
    static bool IsExistContinuousEffectItem(CUser* user, int itemIdx);
};

// CDisjointer（分解术士；vptr 0x8cc0828）。m_machine@+0x8；四虚槽全有 ORIG 实现。
class CDisjointer : public CExpertJob
{
public:
    CDisjointer();
    ~CDisjointer();

    // ---- CExpertJob 四虚槽 ----
    void OnStartMission(CUser* targetUser, CUser* requestUser);      // T 0x085d2a16
    void OnCloseMission(CUser* user, ENUM_CLOSE_MISSION_TYPE type);  // T 0x085d2b74
    bool IsAvailableJob();                                           // W 0x0849f596
    void Reset();                                                    // T 0x085d2cf2

    static STExpertJobScript* GetScript();      // ORIG T 0x085d1e0a（函数局部 static，无 this）
    static STExpertJobEtcScript* GetEtcScript();  // ORIG T 0x085d1e3a（同上）
    int OnCreateDisjointStore(CUser* user, const char* name, int level, int x,
                              int y);    // ORIG T 0x085d2d08
    // ORIG T 0x085d234a：体内首参读 0x8(%ebp)=item、全程无 this（0x8=item/
    // 0xc=invenItem/0x10=count/0x14=result/0x18=flag/0x1c=user），且
    // get_disjoint_result 调用点仅压 6 槽（0x0..0x14）⇒ static 语义实证
    // （P245/P281；static 不入 mangling，符号名不变）。
    static int get_disjoint_result_(CItem* item, Inven_Item* inventItem,
                                    int count, DisJointResult& result, bool flag,
                                    CUser* user);  // ORIG T 0x085d234a
    int complete_disjoint(CUser* requestUser, PacketGuard& packet);   // ORIG T 0x085d31a0
    int get_disjoint_result(CItem* item, Inven_Item* inventItem,
                            DisJointResult& result, CUser* user);     // ORIG W 0x082347ec
    bool isMine(CUser* user);            // ORIG W 0x08234854
    int get_disjoint_cost() const;       // ORIG W 0x085d3e50：m_machine.get_cost()
    std::vector<int>* GetPredictResult(CUser* user, CItem* item,
                                       Inven_Item* inventItem);       // ORIG T 0x085d1f68
    // ORIG 0x085d35ce/0x085d37e8：调用点无 this 压栈（Dispatcher_Repair/Upgrade
    // DisjointMachine 0x081cfec1/0x081d001f 仅压 user 一参），静态形态实证；
    // static 成员 mangled 与非静态单参一致（_ZN10expert_job11CDisjointer23…）。
    static int OnRepairDisjointMachine(CUser* user);     // ORIG T 0x085d35ce
    static int OnUpgradeDisjointMachine(CUser* user);    // ORIG T 0x085d37e8
    void send_error(CUser* user, int err);                            // ORIG T 0x085d1e62
    bool check_disjoint_endurance();                                  // ORIG T 0x085d1f24
    // ---- OnlyOrig_CreatureJob 批追加（定义 ExpertJobOnlyMgr.cpp）----
    int OnEnterDisjointStore(CUser* user);        // ORIG T 0x085d309a
    int get_disjoint_machine_grade();                                 // ORIG T 0x085d1f0a

private:
    CDisjointMachine m_machine;     // +0x08
};

// CDollController（人偶控制器；抽象基类派生，vtable @0x8cc08e0 四业务槽全
// __cxa_pure_virtual；ctor T 0x085d45b4 仅基类构造+vptr；sizeof==sizeof(CExpertJob)）。
class CDollController : public CExpertJob
{
public:
    CDollController();       // ORIG T 0x085d45b4
    virtual ~CDollController();   // ORIG D1/D0 0x085d45d0/0x085d4600（覆盖纯虚析构槽）
};
// CEnchanter（附魔师；vptr 0x8c7cb68）。布局（ORIG 反汇编实证）：
//   +0x08 ExpertJobMaterial[2] 步长 0x10
//   +0x28 CUser* m_user / +0x2c CUser* m_peer   （OnStartMission 写）
//   +0x30/+0x34 unsigned int uid/peerUid        （unique_id & 0xffff）
//   +0x38/+0x3c/+0x40 int 占位                  （Reset 清零；语义未证）
//   +0x44 int m_peerRequestType                 （SetPeerRequestType 写入）
class CEnchanter : public CExpertJob
{
public:
    CEnchanter();
    ~CEnchanter();

    // ---- CExpertJob 四虚槽 ----
    void OnStartMission(CUser* targetUser, CUser* requestUser);      // T 0x0849c7b6
    void OnCloseMission(CUser* user, ENUM_CLOSE_MISSION_TYPE type);  // T 0x0849c822
    bool IsAvailableJob();                                           // W 0x0849f30c
    void Reset();                                                    // T 0x0849c71e

    void SetPeerRequestType(ENUM_PEER_REQUEST_TYPE type);   // ORIG W 0x849f316：写 +0x44（enum 版，mangled 实证）
    // ---- EJEnchanter 批追加 14 方法（定义 ExpertJobOnlyEnchanter.cpp）----
    void OnTryEnchant(CUser* user, MSG_STATIC_COMMAND* msg);         // T 0x0849dfcc
    void OnAgreeEnchant(CUser* user, MSG_STATIC_COMMAND* msg);       // T 0x0849e07e
    void OnPutItemForEnchant(CUser* user, MSG_STATIC_COMMAND* msg);  // T 0x0849df5e
    bool IsUseOtherUserMonsterCard();                                // W 0x849f324【objdump】
    int _DoEnchant(ENUM_TRY_ENCHANT_TYPE type);                      // T 0x0849cb2c
    int _TryEnchant(CUser* user, stItemPos_t* pos, ENUM_TRY_ENCHANT_TYPE type);   // T 0x0849dcc4
    int _AgreeEnchant(CUser* user, ENUM_AGREE_ENCHANT_TYPE type);    // T 0x849dd94
    int _TryEnchantMine(CUser* user, stItemPos_t* pos);              // T 0x0849d7b6
    int _TryEnchantOther(CUser* user, stItemPos_t* pos);             // T 0x0849da3e
    int _PutItemForEnchant(CUser* user, int kind, stItemPos_t* pos, int money);   // T 0x849d344
    bool _DrawEnchant();                                             // T 0x849cac6
    bool IsRecipeRight();                                            // T 0x849ca06（bool：调用点 xor $1 实证 P22）
    int IsRightMaterial();                                           // T 0x849c9b2
        bool IsEqualMonsterCad();                                        // T 0x849ca6a（bool：调用点 xor $1 实证 P22）

private:
    ExpertJobMaterial m_materials[2];   // +0x08，步长 0x10
    CUser* m_user;                      // +0x28
    CUser* m_peer;                      // +0x2c
    unsigned int m_uid;                 // +0x30
    unsigned int m_peerUid;             // +0x34
    int m_field38;                      // +0x38【推断占位】
    CItem* m_field3c;                   // +0x3c（b28 指针字段化：= m_materials[1].m_item
                                        //  的 CItem* 快照，ORIG 直存指针）
    CItem* m_field40;                   // +0x40（b28 字段化：ORIG OnTryEnchant
                                        //  find_item 返回值直存 `mov %eax,0x40(%edx)`
                                        //  无 int 中转；消费点全为 CItem*/子类指针）
    ENUM_PEER_REQUEST_TYPE m_peerRequestType;  // +0x44（b28 枚举化：SetPeerRequestType
                                               //  store 序 LHS-first，同 p1.cpp fC）
};
// CExpertJobFactory（ORIG W 0x0849f68c）：两个池，sizeof 0x60。
// ctor/dtor 定义 GlobalData.cpp。
class CExpertJobFactory
{
public:
    // [b28d-solo-GlobalData] P299/P318：ORIG C1/C2 为 W（隐式合成）⇒ 删用户空 ctor
    // 声明恢复 CWG434 value-init 前置 rep stos（CExpertJobMgr::C1 0x0849e102 实证）。
    ~CExpertJobFactory();
    StaticPool<CEnchanter, 300> m_enchanterPool;    // +0x00
    StaticPool<CDisjointer, 600> m_disjointerPool;  // +0x30
    CExpertJob* AcquireExpertJob(ENUM_EXPERT_JOB_TYPE type);  // ORIG W 0x0849f5d2：按 type 分派池
    void FreeExpertJob(CExpertJob* job);   // ORIG W 0x0849f63e：按 GetType 分派池
};

// CExpertJobMgr（m_factory@+0；AcquireExpertJob 定义 ExpertJobLink4.cpp；
// ctor/dtor 定义 GlobalData.cpp）
class CExpertJobMgr
{
public:
    CExpertJobMgr();
    ~CExpertJobMgr();
    CExpertJob* AcquireExpertJob(ENUM_EXPERT_JOB_TYPE type);  // ORIG T 0x0849e1dc

    // ---- OnlyOrig_CreatureJob 批追加（定义 ExpertJobOnlyMgr.cpp）----
    unsigned int IsAvailableRequestPeer(CUser* requester, CUser* peer,
                                        ENUM_PEER_REQUEST_TYPE type,
                                        PacketGuard& packet);   // ORIG T 0x0849e55e
    void OnRequestPeer(CUser* requester, CUser* peer,
                       ENUM_PEER_REQUEST_TYPE type, PacketGuard& packet);   // ORIG T 0x0849e256
    void OnResponsePeer(CUser* a, CUser* b, CUser* c, CUser* d, int err,
                        ENUM_PEER_REQUEST_TYPE type, PacketGuard& packet);  // ORIG T 0x0849e362
    int OnEnchantByBead(CUser* user, int itemIdx, int slot, int beadIdx,
                        int option);          // ORIG T 0x0849ed1a
    void OnGiveup(CUser* user);               // ORIG T 0x0849e60c
    void OnLeaveUser(CUser* user);            // ORIG T 0x0849e214

    void FreeExpertJob(CExpertJob* job);   // ORIG T 0x0849e1f8：转发工厂

private:
    CExpertJobFactory* m_factory;  // +0x00
};

// ---- 自由函数（OnlyOrig_CreatureJob 批；定义 ExpertJobOnlyMaterial.cpp）----
CItem* FindMonsterCard(CItem* recipe);                                        // ORIG T 0x0849be32
void GetRecipeInputItems(CItem* recipe, std::vector<std::pair<int, int> >& inputs);   // ORIG T 0x0849b734
// ORIG T 0x0849b820。bool（P292/P248 档①）：ORIG 调用点 _DoEnchant 0x127
// `xor $1,%eax; test %al,%al; je`（无 setne）⇒ callee 单字节 bool 返回；
// 返回类型不入 mangled，定义体 return 0/1 不变（ExpertJobOnlyMaterial.cpp 待属主同步）。
bool IsExistRecipeItem(CUser* user, CItem* recipe, bool onlyInputs);
// [Q3PrettSweepA 2026-09-11] 同底层，mangled 不变；ORIG prett =
// `bool expert_job::DelMaterialOfRecipe(CUser*, IntPairVector&, int, PacketGuard*, bool&, int)` size 0x92。
bool DelMaterialOfRecipe(CUser* user, IntPairVector& materials,
                         int slot, PacketGuard* packet, bool& changed,
                         int bodyCount);                                      // ORIG T 0x0849b957
int UseMaterialOfRecipe(CUser* user, CItem* recipe, PacketGuard* packet,
                        bool onlyInputs);                                     // ORIG T 0x0849bcfb


}  // namespace expert_job

// ---- EJEnchanter 批：专家职业物品统计（全局作用域——ORIG mangled
//      _ZN19CExpertJobStatistic… 无命名空间前缀；定义 MouseRegisterEJ.cpp）----
class CExpertJobStatistic
{
public:
    // std::map 键（ORIG ctor W 0x08610720 / operator< W 0x086106da）：
    // operator< 先比 int 无符号化比较、相等再比 unsigned long。
    struct ExpertJobKey
    {
        ExpertJobKey(ENUM_EXPERT_JOB_TYPE type, unsigned long itemIdx);
        bool operator<(const ExpertJobKey& other) const;

        // [b28c-35] ORIG operator< 0x086106da 段1 保留 `mov (%eax),%eax; mov %eax,%edx`
        // 两段装载（4.1 对 enum 字段的中转）⇒ 字段类型为 enum（C28L36 JobKey 同法）；
        // int 字段 4.4 恒折 `mov (%eax),%edx` 单段。sizeof/mangled 不变。
        ENUM_EXPERT_JOB_TYPE m_field00;  // +0x00（job/type）
        unsigned long m_itemIdx04;   // +0x04（物品 idx）
    };

    CExpertJobStatistic();           // ORIG W 0x0861074a：map C1 + clear
    ~CExpertJobStatistic();          // ORIG W 0x086108ae：map D1

    void increaseItemCount(ENUM_EXPERT_JOB_TYPE type, unsigned long itemIdx,
                           int count);   // ORIG T 0x0860f9d4
    void UpdateExpertJob();              // ORIG T 0x0860faca（DB_Q 批量写 m_counts 后清空）

private:
    std::map<ExpertJobKey, int> m_counts;   // +0x00（ctor 即空 map）
};
CExpertJobStatistic* GetInstanceExpertJobStatistic();   // ORIG T 0x0860f940

#endif  // DNF_CEXPERTJOB_H

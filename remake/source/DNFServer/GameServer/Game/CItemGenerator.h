// ============================================================================
// CItemGenerator.h — WongWork 掉落生成器域权威声明（唯一声明点）
//
// 覆盖类型（全部经 df_game_r nm/objdump 核实；报告见
//   docs/class_func_reports/WongWork__CItemGenerator{,Mgr}/ 等）：
//   ::CLuckPoint           （UseLuckPoint T 0x08550a84，静态方法）
//   WongWork::CLuckPoint   （ctor 0x08550884 / dtor 0x085508ec）
//   stGenerateRefData_t    （ctor 0x0814a706 / dtor 0x0814aaf0）
//   stGenerateResult_t     （ctor 0x0814ab06 / dtor 0x0814ab4c）
//   CGenerateRandomNumber  （ctor 0x085334ea / dtor 0x085335d0 /
//                           generateNumber 0x085334a4）
//   GenerateRandomUpgrade  （ctor 0x085335ee / dtor 0x0853364e）
//   CItemGenerator         （ctor 0x08533b3e / dtor 0x08533b9a；
//                           vtable _ZTVN8WongWork14CItemGeneratorE @ 0x08c9a788）
//
// 布局证据（objdump 反汇编 + 本工具链实测）：
//   WongWork::CLuckPoint:  +0x00 vector<CUser*>（0xc）；
//                          +0x0c map<int,int>（本工具链 sizeof(std::map)==0x18 实测）
//                          => sizeof 0x24
//   stGenerateResult_t:    +0x00 vector<Inven_Item>（0xc，ctor 先调
//                          St6vectorI10Inven_ItemC1Ev）；+0x0c CLuckPoint（ctor
//                          add $0xc 后调 CLuckPointC1Ev）
//                          => sizeof 0x30（ORIG 同帧两个结果对象相距 0x30 实证；
//                          CLuckPoint 0x24 之后无尾随成员）
//   stGenerateRefData_t:   +0x00 int=100；+0x04 int=0；+0x08 vector<bool>
//                          （本工具链 sizeof==0x14 实测；ctor clear +
//                          push_back(false)×2）；+0x1c float=1.0f；
//                          +0x20 int=0（setPremiumDropRate 写，CBattle_Field
//                          0x0830b848 实证写 +0x20 / 读字节 +0x40）；
//                          +0x24/+0x25 byte=0；+0x28 int=11；+0x2c/+0x30 int=0；
//                          +0x34..+0x47 调用上下文 blob（WarField/
//                          generateRewardItem 写 level/roleType/mobIndex/dungeon
//                          等，见各调用方注释）
//                          => sizeof 0x48（ORIG 同帧两个 ref 对象相距 0x48 实证）
//   CGenerateRandomNumber: +0x00 CMTRand*（ctor new CMTRand[count]，元素即
//                          PvPTypes.h 的 CMTRand(0x9c8)，generateNumber 对元素
//                          调 CMTRand::randInt）；+0x04 int count => sizeof 8
//   GenerateRandomUpgrade: +0x00 CMTRand*（operator new 0x9c8）=> sizeof 4
//   CItemGenerator:        +0x00 vptr；+0x04 CGeneratorCommonData*（setCommonData
//                          0x0853a3e2 写 +0x04）；+0x08 CGenerateRandomNumber；
//                          +0x10 GenerateRandomUpgrade；+0x14 int generatorType
//                          （setGeneratorType/getGeneratorType 写读 +0x14）
//                          => sizeof 0x18
//
// vtable（4 槽，槽位由派生类 vtable 交叉证实：CMonsterDrop @0x08c9a740 四槽 =
//   setDropRatio/loadScript/generateRandomItem/generateSpecificItem 具名符号）：
//   +0x00 setDropRatio(float)
//   +0x04 loadScript(char const*)
//   +0x08 generateRandomItem(stGenerateRefData_t const&, stGenerateResult_t&)
//   +0x0c generateSpecificItem(stGenerateRefData_t const&, stGenerateResult_t&)
// 注意：无虚析构（vtable 仅 4 槽；dtor 为非虚 T 符号）。
// ============================================================================
#ifndef GAME_CITEMGENERATOR_H_
#define GAME_CITEMGENERATOR_H_

#include <map>
#include <vector>

#include "Inven_Item.h"
#include "PvPTypes.h"   // CMTRand（0x9c8 字节随机源）
#include "CItemAmplifier.h"   // upgrade_table_t（CItemUpgradeTable 基类完整类型）

// [b28d-56] P254：stGenerateRefData_t::m_field28 必须是枚举类型，才能复现 ORIG
// 0x8349b7d `mov 0x28(%eax),%eax; mov %eax,%edx; mov levelFilter,%eax; cmp` 的
// eax→edx 中转（int 字段时 4.4 折成 `mov 0x28(%eax),%edx` 单段）。探针
// /tmp/enum_probe3.cpp a(int) vs b(enum) 在 4.1.2 与 4.4.7 上双证。
// 以既有 guard 内联（与 JobKey.h / STCharacterScript.h / Quest.h /
// CCompoundAvatar.h / GlobalScriptTutorial.h / STEquipmentScript.h 幂等，
// 成员表一致）；本仓编译旗标无 -fstrict-enums（窄枚举存非 0 值安全）。
#ifndef DNF_ENUM_CHARACTERJOB_DEFINED
#define DNF_ENUM_CHARACTERJOB_DEFINED
enum ENUM_CHARACTERJOB
{
    ENUM_CHARACTERJOB_0 = 0
};
#endif

class CUser;  // 全局命名空间（ORIG vector<CUser*>）
class RegenerationROI;  // RandomRentalA2.cpp 的 RegenVer_* 签名引用
class CRandomOptionGive;  // set_random_option_give 参数（UserMiscLink 批）
class CDungeon;  // stGenerateRefData_t::m_dungeon（+0x40 指针，仅前置声明）
class CMonster;                    // _generateMonseterSpecificItem 参数（ORIG
                                   //   拼写如此；仅指针引用）
// （stGenerateRefClearReward_t 前向声明见 namespace WongWork 内 CClearReward
//  之后；ORIG mangled 属 WongWork::，勿在此全局区声明以免 ODR 分裂）
class GenuineDamageUpgradeTable;  // CItemUpgrade_Separate.h 完整类型（0x188）
// ORIG T 0x0891229d _Z22getGenuineUpgradeTablev：函数局部 static 惰性单例，
// operator new(0x188)→清零→构造→importGenuineUpgradeTable 失败返回 0。
// 定义 CItemGenerator.cpp（本头为唯一声明点）。
GenuineDamageUpgradeTable* getGenuineUpgradeTable();

// 全局可见别名（OnlyOrig_CreatureJob 修复插入）：CObjectDrop 等全局作用域类的
// generateRandomItem/generateSpecificItem 以非限定名引用两结构；权威定义在
// 下方 namespace WongWork（161/196 行）。using 声明不产生新类型，无 ODR 风险。
namespace WongWork { struct stGenerateRefData_t; struct stGenerateResult_t; }  // 前置（Wave5 APSystem 批补：定义在本文件下方 ns 内）
using WongWork::stGenerateRefData_t;
using WongWork::stGenerateResult_t;

class CLuckPoint
{
public:
    CLuckPoint();   // ORIG T 0x08550884（定义 CClearReward.cpp：m_users.clear()）
    static bool UseLuckPoint(CUser* user, int itemType, int rarity, int grade);
    void AddUser(CUser* user);   // ORIG T 0x08550932：user 非空时 push_back（
                                 //   CParty::GetLuckPoint 奖励收集；ORIG AddUser 反汇编实证）
    // ORIG T 0x08550be4：按稀有度权重表+幸运系数修正 roll 并返回映射稀有度
    // （定义 CClearReward.cpp）。
    int GetItemRarity(int* table, int roll, int dropType);
    // ORIG T 0x085509da：成员幸运点均值（sum(GetCurCharacLuckPoint)/size；空 0）。
    int GetLuckPoint();
    // ORIG T 0x08550e50 _ZN10CLuckPoint15GetUserGainItemEi：m_points(+0xc) 按
    // 序号取用户下标再回 m_users；越界/负序号返回 0（LinkKillChain 批次补声明）。
    int GetUserGainItem(int idx);
    // ORIG T 0x08550b14 _ZN10CLuckPoint12UseLuckPointEiiii：取 m_users 最大幸运
    // 点用户序号，调静态 UseLuckPoint(user,a,b,c) 成功则 m_points[p4]=序号。
    bool UseLuckPoint(int itemType, int rarity, int grade, int seq);
    // ORIG T 0x08550ef0 _ZN10CLuckPoint16sendItemDropRateEPKc：遍历 m_users，
    // 对 getCurCharacR 非空者经 GetGameMasterCharacter 虚发 sendServerState。
    void sendItemDropRate(char const* msg);
    // ORIG T 0x08550954：遍历 m_users 取 GetCurCharacLuckPoint 最大者序号
    // （空表返回 0；报告 docs/class_func_reports/CLuckPoint/_GetMaxLuckPointUserIndex.md）。
    unsigned int _GetMaxLuckPointUserIndex();
    std::vector<CUser*> m_users;   // +0x00（vector 0xc；ORIG AddUser 0x08550932
                                   //   push_back 到 this+0 实证）
    // +0x0c：ORIG GetUserGainItem 0x08550e50 以 map<int,int>::size/operator[]
    // 访问 +0xc（idx→用户序号映射；GetUserGainItem 反汇编实证）。
    std::map<int, int> m_points;   // +0x0c
};

// ---- ::WorldDropItemChunck / ::WorldDropInfo（全局命名空间；原 CDataManager.h
//      定义移入此处：WongWork::CWorldDrop 成员数组需要完整类型。生命周期唯一
//      owner：C1/D1 均在 DropLayoutDeps.cpp（ORIG 0x08374ec8 / 0x08374ede）；
//      ImportWorldDrop InitScriptProducers_1.cpp T 0x089022be。）----
// ORIG WorldDropItemChunck：WongWork::CWorldDrop::generateRandomItem 0x085383b8
// 读元素 +0x00（道具 idx）/+0x04（权重），元素步长 8。
struct WorldDropItemChunck
{
    int m_itemIdx;   // +0x00
    int m_rate;      // +0x04（权重；ImportWorldDrop 累加至槽位总量）
};

class WorldDropInfo
{
public:
    WorldDropInfo();
    ~WorldDropInfo();

    // ORIG 槽位步长 0x14（m_worldDrop1[201]@+0x6734 → m_worldDrop2@+0x76e8 自证）；
    // ImportWorldDrop 0x089022be 写 +0x00/+0x04(vector)/+0x10，装载后 m_first=m_total。
    int m_first;                        // +0x00（装载完成时 = m_total）
    std::vector<WorldDropItemChunck> m_chunks;  // +0x04
    int m_total;                        // +0x10（权重总和）
};  // sizeof == 0x14

namespace WongWork
{
    struct stGenerateRefClearReward_t;   // 前置：CClearReward 私有签名（573/576）
                                         //   先于下方完整声明处使用；定义在 CClearReward.cpp

// ---- CGeneratorCommonData（sizeof == 0x648，ORIG 实证）----
// 证据：getGoldGenTable ORIG T 0x08533424 以 this+level*8 步长取 8 字节表项
// （level>0xc8 时回退表项 0），并把 [基準値,随机浮动幅度] 拷入静态返回区
// 0x943ded0（CParty::get_item 0x4af 金币生成消费）=> 表为 201×8B。
// CItemGeneratorMgr ORIG operator new 0xa28、m_common 位于 +0x3e0 =>
// sizeof(common) = 0xa28-0x3e0 = 0x648 = 201*8 ✓。ctor/dtor 均空体
// （0x08374ebc/0x08374ec2 仅 push/mov/pop/ret）。loadScript T 0x08534456。
class CGeneratorCommonData
{
public:
    // [Main·2026-09-11 P216 全局 A/B] ctor 加 `throw()`：外层 `WongWork::CItemGeneratorMgr`
    // 的 ctor 经成员构造内联了本 ctor，我方声明无 throw() ⇒ 4.4 认为成员构造可抛 ⇒
    // `new CItemGeneratorMgr` 点生成 8 条 cleanup（`_ZdlPv` + jmp LP）。
    // ORIG 同点 0x0835cd26 之后紧接 0x0835cd2b `mov %ebx,%edx`（直接续正常路径、无 LP），
    // 且全函数 `_ZdlPv` ORIG 26 次 vs 我方 27 次 ⇒ 该 ctor 在 ORIG 为 nothrow。
    // 体空（0x08374ebc push/mov/pop/ret）⇒ 与 `PacketHeader` 反例同族，**以全局核账定稿**。
    CGeneratorCommonData() throw();                   // ORIG W 0x08374ebc（空体）
    ~CGeneratorCommonData();                         // ORIG W 0x08374ec2（空体）

    // ORIG T 0x08534456 _ZN8WongWork20CGeneratorCommonData10loadScriptEPKc
    // 返回错误行号（0=成功）；定义待脚本导入域批次。
    int loadScript(const char* path);

    int* getGoldGenTable(unsigned char level);       // ORIG T 0x08533424

    struct GoldGenEntry
    {
        int m_base;      // +0x00 基准值
        int m_randAmp;   // +0x04 随机浮动幅度【推断：语义命名，布局实证】
    };
    GoldGenEntry m_goldGenTable[201];  // +0x000（getGoldGenTable 按 level*8 索引，上界 0xc8）
};  // sizeof == 0x648

// [b28d-Main·检查点18] 删除本 namespace 内的重复 `WongWork::CLuckPoint`（ORIG 无
//   `_ZN8WongWork10CLuckPointC1Ev` 符号；stGenerateResult_t::m_luckPoint 已声明为
//   全局 `::CLuckPoint`）。需要该名时用下方 using 声明引用全局类。
using ::CLuckPoint;

// ---- stGenerateRefData_t（掉落生成引用；sizeof 0x48） ----
struct stGenerateRefData_t
{
    stGenerateRefData_t();
    ~stGenerateRefData_t();

    int m_dropRate;                 // +0x00（ctor=100）
    // b28d-53：ORIG generateRandomItem 0x08538773 `mov 0x4(%eax),%edx` 单条 disp 读
    // 该 4 字节并位重解释为 float 实参；4.4 对 `*(float*)&m_field04` / float 引用视图
    // 恒折 `add $0x4; mov (%eax)` 两步 ⇒ 权威头匿名 union 双视角（P235/P219，
    // sizeof/偏移/对齐/mangled 零影响）。
    union {
        int m_field04;              // +0x04（ctor=0）
        float m_field04f;           // +0x04（位重解释 float 视角）
    };
    std::vector<bool> m_forceTag;   // +0x08（ctor: clear + 2×push_back(false)；0x14 字节）
    float m_field1c;                // +0x1c（ctor=1.0f）
    int m_premiumDropRate;          // +0x20（setPremiumDropRate 写 0/-1/rate）
    char m_field24;                 // +0x24（ctor=0）
    char m_field25;                 // +0x25
    // [b28d-56] P254：enum 字段（ORIG 0x8349b7d `mov 0x28(%eax),%eax; mov %eax,%edx`
    // 两段装载）；sizeof/偏移/mangled 零变化。
    ENUM_CHARACTERJOB m_field28;    // +0x28（ctor=11）
    int m_field2c;                  // +0x2c
    int m_field30;                  // +0x30
    // ---- +0x34..+0x47：调用方上下文区（全部具名，2026-09-05 BossMission 批次）----
    // 独立掉落路径（generateIndependentItem 0x834989a /
    // independentItemRateControl 0x834972f）按字节读取本区；
    // 调用方（CBossPlay::generateRewardItem 0x08149914、WarField、CBossStage、
    // BattleMonsterDepsC2 等）写入宽度不一，故下列成员以 union 叠加
    // “整字 / 半字 / float” 视图，逐一对齐 ORIG 指令宽度
    // （movl / movw / float 位型）；旧基名全部保留 → 消费者零破坏。
	// b28d-solo-WongWorkDrop_A：CDungeonDrop/CObjectDrop 的 ORIG 以
	// `mov 0x34(%eax),%edx` 单条 disp 读本 4 字节为**配置指针**
	//（vector/list<pair<int,int>>）；`&m_field34` 取址在 4.4 恒产
	// `add $0x34; mov (%eax)` 两步 ⇒ 匿名 union 叠加指针/整字视图
	//（P235，sizeof/偏移/成员名/mangled 零影响）。
	union
	{
		void* m_field34p;           // +0x34 指针视图（掉落配置表指针）
		int   m_field34d;           // +0x34 整字视图
		std::vector<std::pair<int, int> > const* m_field34v;  // +0x34
		                            //   CDungeonDrop 配置表（vector<pair<int,int>>；
		                            //   读侧恒 const 视图 ⇒ ORIG K 版 begin/end）
		std::list<std::pair<int, int> > const* m_field34l;  // +0x34
		                            //   CObjectDrop 配置表（list<pair<int,int>>）
		struct
		{
			char m_field34;         // +0x34（基准值；与 +0x3f 组成等级差【推断】；
			                        //   CBossPlay 写掉落等级参数 a，WongWorkDrop 侧按 type 读）
			char m_field35;         // +0x35（CBossPlay 写难度 diff，WarField 写 mob level）
			char m_rateSlot;        // +0x36（m_rate[] 掉率档位下标【推断】；WarField 写 mob level）
			char m_pad37;           // +0x37
		};
	};
	union
	{
		int   m_rankBonusIndex;     // +0x38（eRankBonusIndex；_generateRandomItem 按 float 位型读）
		float m_rankBonusRate;      // +0x38（float 视图：ORIG CBossPlay::generateRewardItem
		                            //   特定分支 `mov $0x3f800000,%eax; mov %eax,-0xec(%ebp)`
		                            //   即写 1.0f；随机分支同槽写整型 1）
		struct                     // +0x38 低字节视图（WongWorkDrop 按字节读）
		{
			unsigned char m_field38;  // +0x38
			unsigned char m_field39;  // +0x39（循环次数上限 i8）
			unsigned char m_field3a;  // +0x3a
			unsigned char m_field3b;  // +0x3b
		};
	};
	union
	{
		int m_field3c;              // +0x3c（整字视图：ORIG 特定分支 `movl $0x0,-0xe8(%ebp)`）
		struct
		{
			unsigned short m_goldBase;    // +0x3c（金币公式基数分量；随机分支 movw $1）
			unsigned short m_goldFactor;  // +0x3e（金币公式加倍分量；随机分支 movw $1）
		};
		struct                     // +0x3c 低字节视图（WongWorkDrop_B 按字节读）
		{
			unsigned char m_field3c_byte;  // +0x3c
			unsigned char m_field3d;       // +0x3d
			unsigned char m_field3e_byte;  // +0x3e（m_goldFactor 低字节）
			unsigned char m_field3f;       // +0x3f
		};
	};
	union
	{
		CDungeon* m_dungeon;        // +0x40（_generateSpecificItem 取 clear_item 的目标副本；
		                            //   ORIG 特定分支 `mov %eax,-0xe4(%ebp)` 整字写）
		struct
		{
			unsigned short m_field40;     // +0x40（半字视图：ORIG 随机分支 movw $1）
			unsigned short m_field42;     // +0x42（半字视图：ORIG 随机分支 movw $1，
			                            //   两半合成指针位型 0x10001，随机路径不解引用）
		};
		struct                     // +0x40 低字节视图（WongWorkDrop 按字节读 hell 旗标等）
		{
			unsigned char m_field40_byte;  // +0x40
			unsigned char m_field41;       // +0x41
			unsigned char m_field42_byte;  // +0x42
			// b28d-solo-WongWorkDrop_A：ORIG generateSpecificItem 0x08537254
			// `movzbl 0x43; xor $0x1; test %al,%al; je` ⇒ bool 字段（P248 档②；
			// uchar 恒产 cmp $1 形）。1 字节、偏移/布局/mangled 零影响。
			// [N5A 2026-09-11] 追加同址字节视角：ORIG kill_monster/_MakeDropItems
			// `ref+0x43` 的 store 形（0x0830b0b5..0xc1 `mov $1,%eax/jmp/mov $0,%eax/
			// mov %al,-0x7d`）只能由「向**字节左值**写 int 三目」产生 —— 4.1.2 与 4.4.7 对
			// bool 目标恒发 `setne`（探针 /tmp/r5/n5/N5A_BattleField/e_probe.cpp E1..E5、
			// x_probe.cpp X1..X7 全形实测）；而消费者形 `movzbl;xor $1;test;je` 要求 bool
			// （h_probe.cpp：uchar 目标反产 `test;jne`）。union 双视角同时满足两侧
			// （u_probe.cpp 实证：store 复现 ORIG 4 条形、消费者仍为 `xor $1` 形）。
			// 1 字节 union，offsetof(+0x43)/sizeof(0x48)/mangled 零影响，写入值仅 0/1。
			union
			{
				bool m_field43;               // +0x43
				unsigned char m_field43Byte;  // +0x43（同址字节视角）
			};
		};
	};
	union
	{
		unsigned short m_field44;         // +0x44
		struct                     // +0x44 低字节视图（WongWorkDrop_B 按字节读）
		{
			bool m_field44_byte;           // +0x44（同上 bool 化，P248 档②）
			unsigned char m_field45;       // +0x45
		};
	};
	char m_pad46[2];                // +0x46..+0x47
};
// 布局自检（union 仅叠加视图，sizeof/成员偏移零漂移）。
static_assert(sizeof(stGenerateRefData_t) == 0x48, "stGenerateRefData_t size drift");
static_assert(offsetof(stGenerateRefData_t, m_field34) == 0x34, "ref m_field34 drift");
static_assert(offsetof(stGenerateRefData_t, m_rankBonusIndex) == 0x38, "ref +0x38 drift");
static_assert(offsetof(stGenerateRefData_t, m_field3c) == 0x3c, "ref +0x3c drift");
static_assert(offsetof(stGenerateRefData_t, m_dungeon) == 0x40, "ref +0x40 drift");
static_assert(offsetof(stGenerateRefData_t, m_field44) == 0x44, "ref +0x44 drift");

// ---- stGenerateResult_t（掉落生成结果；sizeof 0x30） ----
// 注意：ORIG 无 +0x0f「幸运」成员。+0x00 vector(0xc) + +0x0c CLuckPoint(0x24)
// 已完整覆盖 0x30；result+0x0f 落在 CLuckPoint::m_users._M_start 指针内部，
// 不可字段化。CBossStage::consistMap ORIG 写的"幸运字节"是对象外的独立栈
// 标量死存储（-0xb2f(%ebp)，写后无读取），已删除，勿再加成员/裸偏移写入。
struct stGenerateResult_t
{
    stGenerateResult_t();
    ~stGenerateResult_t();

    std::vector<Inven_Item> m_dropItems;  // +0x00
    ::CLuckPoint m_luckPoint;             // +0x0c（sizeof 0x24；ORIG 用全局 ::CLuckPoint）
};

// ---- CGenerateRandomNumber（加权随机表宿主；sizeof 8） ----
class CGenerateRandomNumber
{
public:
    CGenerateRandomNumber(int count);
    ~CGenerateRandomNumber();

    int generateNumber(int idx, int range);

    CMTRand* m_table;  // +0x00（new CMTRand[count]）
    int m_count;       // +0x04
};

// ---- GenerateRandomUpgrade（升级随机源；sizeof 4） ----
class GenerateRandomUpgrade
{
public:
    GenerateRandomUpgrade();
    ~GenerateRandomUpgrade();
    // ---- WongWorkDrop_B 批次补充（ORIG T 实证；定义 WongWorkDrop_B.cpp）----
    // ORIG T 0x08533932 _ZN8WongWork21GenerateRandomUpgrade18generateEquipGradeEi
    //   权重 {1000×5}、等级档 {0x5aa7,0xfa6,0x6a23,0x5df0,0x7270}；比率表 =
    //   G_CDataManager()+0x540 vector<pair<int,float>>（字段未建模，本批不改
    //   CDataManager.h），randInt(this->m_rand, &total) 按累计权重落槽。
    int generateEquipGrade(int levelDelta);
    // ORIG T 0x08533676 _ZN8WongWork21GenerateRandomUpgrade15generateUpgradeE11ENUM_RARITYi
    //   按 rarity 键查 G_CDataManager()+0x528 map<int,ItemFarmingData>；
    //   条目 active(+0xc)==0 或查无 → 0；w[0] 缩放后按累计权重取条目 first。
    int generateUpgrade(ENUM_RARITY rarity, int levelDelta);

    CMTRand* m_rand;  // +0x00（operator new 0x9c8；构造期额外初始化调用未建模，[INFERENCE]）
};

// ---- CItemGenerator（生成器基类；sizeof 0x18，无虚析构） ----
class CItemGenerator
{
public:
    CItemGenerator(int count);
    ~CItemGenerator();  // 非虚（ORIG vtable 仅 4 槽）

    virtual void setDropRatio(float ratio);                                        // 槽 +0x00
    virtual int loadScript(char const* path);                                      // 槽 +0x04（ORIG 基类 0x0853a3f6 ret0：返回错误行，0=成功）
    virtual void generateRandomItem(stGenerateRefData_t const& ref,
                                    stGenerateResult_t& result);                   // 槽 +0x08
    virtual void generateSpecificItem(stGenerateRefData_t const& ref,
                                      stGenerateResult_t& result);                 // 槽 +0x0c

    void setCommonData(CGeneratorCommonData* common);        // W inline，写 +0x04
    CGeneratorCommonData* getGeneratorCommonData();          // W inline，读 +0x04
    void setGeneratorType(int type);                         // W inline，写 +0x14
    int getGeneratorType();                                  // W inline，读 +0x14

    CGeneratorCommonData* m_common;      // +0x04（ORIG ctor 不初始化）
    CGenerateRandomNumber m_random;      // +0x08
    GenerateRandomUpgrade m_upgrade;     // +0x10
    int m_generatorType;                 // +0x14
};


// ============================================================================
// 派生生成器与生成率表（布局由 ORIG ctor/loadScript 偏移逐字段实证；实现
// 全部在 DropLayoutDeps.cpp，本头为唯一声明点）
// ============================================================================

// ---- 脚本范围表：u16×202。[item drop ref table] 以低/高字节写各半边，
//      generateTable 读低字节(min)/高字节(max)。 --------------------------------
struct stGenItemRange_t   // ORIG WongWork::stGenItemRange_t
{
    unsigned short m_range[202];
};

// ---- 单 type 组生成率块：槽址 = type*0x7110+rarity*0x12d8+level*24 ----------
// ORIG 中无 DropGenRateBlock_t 类型符号：CItemGenRateTable::C1（0x08539e66）
// 与 CSecialIMonsterDropItemGenRateTable::C1（0x0853a0b6）均内联 5×6×201 map
// 构造循环（外层 5×0x7110，type0 块同样构造但代码仅访问 type∈[1,4]）。
// 故两宿主类直接持有裸 map 数组成员，不引入 struct 包装（struct 会触发
// GCC 4.4 提取成员构造符号，与 ORIG 内联形态不符）。

}  // namespace WongWork（临时闭合：random_option 为全局命名空间）

namespace random_option
{
// CRandomOptionItemHandle 布局镜像（权威定义 GlobalData.cpp；DisPatcherCore.cpp
// 另有仅签名镜像，互不包含）。sizeof 0x18。
class CRandomOptionItemHandle
{
public:
    CRandomOptionItemHandle();           // 定义 GlobalData.cpp（W 0x085f0d64）
    virtual ~CRandomOptionItemHandle();  // 定义 GlobalData.cpp（W 0x085f0e46）
    void set_disable_drop();             // 定义 DropLayoutDeps.cpp（W 0x08539e42）
    unsigned int reset_option(unsigned long itemIdx, ENUM_RARITY rarity,
                              int optionType, RandomOption* options); // ORIG T 0x085f39d2
    bool give_option(unsigned long itemIdx, ENUM_RARITY rarity, int level,
                     int group, int grade, RandomOption* opts);          // ORIG T 0x085f2cc6
    bool RegenVer_give_option(unsigned long itemIdx, ENUM_RARITY rarity,
                              int level, int group, int grade, RandomOption*,
                              RegenerationROI*, int optType);            // ORIG T 0x085f1bb0
    unsigned int RegenVer_choose_item(ENUM_RARITY rarity, int level,
                                      const std::vector<int>& itemGroups); // ORIG T 0x085f1802
    // ORIG T 0x085f1560：随机选项槽按稀有度选 item（CItemGenRateTable::chooseItem
    // rarity==2 分支；定义置 CRandomOptionItemHandle 域）。
    unsigned int choose_item(ENUM_RARITY& rarity, int level);
    // UserMiscLink 批补（ORIG 实证）：
    void set_random_option_give(CRandomOptionGive* give);                // ORIG T 0x085f0ea2（写 m_optionSet）
    bool change_option(unsigned long itemIdx, ENUM_RARITY rarity, int level,
                       int grade, int group, int slot, RandomOption* opts); // ORIG T 0x085f3340

private:
    int _get_level_limit(ENUM_RARITY rarity, int level);                 // T 0x085f0f9a
    int _get_part_type(int limit);                                       // T 0x085f1088
    int _get_part_type_by_ItemGroup(int limit,
                                    const std::vector<int>& groups) const; // T 0x085f11a4
    int _get_base_item(ENUM_RARITY rarity, int limit);                   // T 0x085f1426
    // [Lane13] 返回类型 int→bool（ORIG choose_item 0x085f163d 调用点 xor $1 直测，P292）。
    bool _random_application(ENUM_RARITY rarity, int level);             // T 0x085f0ec2
    int _get_option_type(unsigned long itemIdx, ENUM_RARITY rarity,
                         int level, int group, unsigned char& outType);  // T 0x085f2338
    int RegenVer_get_option_quantity(ENUM_RARITY rarity, int level,
                                     int grade, int optType, int& outQuantity,
                                     RegenerationROI*); // ORIG T 0x085f1a62
    int _get_option_quantity(ENUM_RARITY rarity, int level, int grade,
                             int optType, int& outQuantity);         // T 0x085f25b0
    int _choose_option_group(ENUM_RARITY rarity, int level, int grade,
                             int optType, int group, int& outGroup); // T 0x085f2882
    int _get_option_value(int minV, int maxV);                           // T 0x085f2b02

private:
    CMTRand* m_rand;           // +0x04
    int m_field08;             // +0x08
    unsigned char m_disabled;  // +0x0c
    int m_field10;             // +0x10
    void* m_optionSet;         // +0x14
};
}  // namespace random_option

// ORIG 全局枚举（CSecialIMonsterDropItemGenRateTable::chooseItem 形参，
// ORIG mangled 为无命名空间限定的 23ENUM_DUNGEON_DIFFICULTY / 27ENUM_ITEM_DROP_
// DUNGEON_TYPE；WongWorkDrop_B 批补。DNF_ENUM_*_DEFINED 守卫与
// CBattle_Field_shared.h/CBossPlay.h 同名枚举共用）。
#ifndef DNF_ENUM_ENUM_DUNGEON_DIFFICULTY_DEFINED
#define DNF_ENUM_ENUM_DUNGEON_DIFFICULTY_DEFINED
enum ENUM_DUNGEON_DIFFICULTY
{
    ENUM_DUNGEON_DIFFICULTY_0 = 0
};
#endif  // DNF_ENUM_ENUM_DUNGEON_DIFFICULTY_DEFINED

#ifndef DNF_ENUM_ENUM_ITEM_DROP_DUNGEON_TYPE_DEFINED
#define DNF_ENUM_ENUM_ITEM_DROP_DUNGEON_TYPE_DEFINED
enum ENUM_ITEM_DROP_DUNGEON_TYPE
{
    ENUM_ITEM_DROP_DUNGEON_TYPE_0 = 0
};
#endif  // DNF_ENUM_ENUM_ITEM_DROP_DUNGEON_TYPE_DEFINED

namespace WongWork
{
// ---- CItemUpgradeTable（WongWork::CItemUpgrade 的表区子对象；upgrade_table_t
//      非虚包装，sizeof == sizeof(upgrade_table_t) = 0x4e4。ORIG CItemUpgrade
//      双基布局实证：IItemUpgrade@+0x00（多态主基）、表区@+0x04..0x4e7。） ----
// [R18rti 2026-09-13 · B3 RTTI 复刻] ORIG `_ZTIN8WongWork17CItemUpgradeTableE`
// （0x8c9d37c）是 plain __class_type_info（size 8、无基类记录），且 ORIG 全库无
// `_ZTI15upgrade_table_t` / `_ZTS15upgrade_table_t`（我方旧形 si 多发射此二符号，
// 见 NEW 0x8b7809c/0x8b78088）⇒ 该类型**不继承** upgrade_table_t，而是内嵌成员。
// 布局零变化：m_table 落 +0x000，sizeof 仍 0x4e4；CItemUpgrade 的次基类偏移 +0x4、
// ctor/dtor 尾调 upgrade_table_tC1/D1 的调用形、mangled 集合全部不变。
class CItemUpgradeTable
{
public:
    CItemUpgradeTable();              // W 0x0854b496（尾调 upgrade_table_t::C1）
    ~CItemUpgradeTable();             // D1/D2 W 0x0854b4aa（尾调 upgrade_table_tD1）

    // ORIG W 0x0854b382：仅返回表视图自身（mov 0x8(%ebp),%eax; ret；消费方按
    // upgrade_table_t* 使用）。
    const upgrade_table_t* getUpgradeInfoTable() const;

    // ORIG W 0x0854b38a：importUpgradeTable(path, 表视图) 尾透传（[P248 档①] bool）。
    bool loadUpgradeScript(const char* path);

private:
    upgrade_table_t m_table;          // +0x000（原基类子对象位置；布局零变化）
};
static_assert(sizeof(CItemUpgradeTable) == 0x4e4,
              "CItemUpgradeTable ABI drift (ORIG 0x4e4 == sizeof(upgrade_table_t))");

// ---- 掉落类型（槽址 type*0x7110 下标；chooseItem/onTryGenerateRandomItem 共用） ----
enum eItemDropType_t
{
    eItemDropType_0 = 0,
    eItemDropType_1 = 1,
    eItemDropType_2 = 2,
    eItemDropType_3 = 3
};


// ---- CItemGenRateTable（掉落等级累积表宿主；sizeof 0x2356C）-----------------
class CItemGenRateTable
{
public:
    CItemGenRateTable();                         // W 0x08539e66
    ~CItemGenRateTable();                        // W 0x0853a030
    void init();                                 // T 0x085345b6
    void destroy();                              // T 0x08534638
    void generateTable(stGenItemRange_t* range, bool keepRandomOption);  // T 0x0853466c
    // ORIG T 0x085349a4：random_option 槽（rarity==2）优先，否则按
    // type*0x7110+rarity*0x12d8+level*24 槽位 map：empty→-1；roll=randInt(maxKey)；
    // lower_bound(roll)==end 回退 --it；返回 it->second。
    int chooseItem(eItemDropType_t type, ENUM_RARITY rarity, int level);
    bool _itemTypeVerify(int type, const CItem* item);                        // T 0x08534906
    void set_disable_drop_random_option_item();  // W 0x08539e4e

private:
    std::map<unsigned int, int> m_slots[5][6][201];  // +0x00000（0x23550 字节）
    CMTRand* m_rand;                             // +0x23550
    random_option::CRandomOptionItemHandle m_optHandle;  // +0x23554
};  // sizeof 0x2356C

// ---- CSecialIMonsterDropItemGenRateTable（特殊怪物变体；sizeof 0x211FCC）----
class CSecialIMonsterDropItemGenRateTable
{
public:
    CSecialIMonsterDropItemGenRateTable();       // W 0x0853a0b6
    ~CSecialIMonsterDropItemGenRateTable();      // W 0x0853a35c
    void init();                                 // T 0x08534c1a
    void destroy();
    void generateTable(stGenItemRange_t* range);          // T 0x08534cd0
    bool _itemTypeVerify(int type, const CItem* item);
    // ORIG T 0x08534fce _ZN8WongWork35CSecialIMonsterDropItemGenRateTable10choose
    // ItemENS_15eItemDropType_tE11ENUM_RARITYi23ENUM_DUNGEON_DIFFICULTY27ENUM_ITEM_
    // DROP_DUNGEON_TYPE（WongWorkDrop_B._generateRandomItem 消费）。
    int chooseItem(eItemDropType_t type, ENUM_RARITY rarity, int level,
                   ENUM_DUNGEON_DIFFICULTY dungeonDiff,
                   ENUM_ITEM_DROP_DUNGEON_TYPE dropDungeonType);

private:
    std::map<unsigned int, int> m_blocks[5][3][5][6][201];  // 15×0x23550 = 0x211FB0
    CMTRand* m_rand;                             // +0x211FB0
    random_option::CRandomOptionItemHandle m_optHandle;  // +0x211FB4
};  // sizeof 0x211FCC

// ---- 首领/命名怪首杀权重（W 0x0853a6ae：[+4]=1 后拷回 [+0]）----------------
// ORIG 类型实为 CMonsterDrop 嵌套类型（mangled
// _ZN8WongWork12CMonsterDrop25stFirstMobHuntingWeight_tC1Ev/C2Ev），定义见
// 下方 class CMonsterDrop 内；类外保留同型 typedef 维持既有引用。

// ---- CMonsterDrop_Hell（地狱派对掉落；sizeof 0x237C0）-----------------------
class CMonsterDrop_Hell : public CItemGenerator
{
public:
    CMonsterDrop_Hell(int count);                // W 0x0853a56a
    ~CMonsterDrop_Hell();                        // W 0x0853a60e
    int loadScript(char const* path);            // T 0x08535344（虚槽 +0x04）

    // ---- P6（WongWorkDrop_A 跨类调用）----
    // A 文件 CMonsterDrop::setDropRatio/generateSpecificItem 需在 m_hell 上调用
    // 本组方法 ⇒ protected→public。仅访问控制变化，sizeof/偏移/mangled/vtable 零变化。
    void setDropRatio(float ratio);               // ORIG T 0x08535948（虚槽 +0x00）
    void generateSpecificItem(stGenerateRefData_t const& ref,
                              stGenerateResult_t& result);  // ORIG T 0x08535726（虚槽 +0x0c）
    void hellPartySpecificItem(stGenerateRefData_t const& ref,
                               stGenerateResult_t& result);  // ORIG T 0x08535b08

protected:
    // v[2..6] 的数组视角 w[0..4]（+0x08 起）：ORIG 0x08535948 以 disp=8 折叠索引
    // （`mov 0x8(%eax,%edx,4)` / `mov %eax,0x8(%edx,%ebx,4)`），4.4 对 `v[2+j]`
    // 恒生 lea 两步（P30）；匿名 union 视图零 sizeof/偏移/符号影响。
    struct stGenItemProb_t
    {
        union
        {
            int v[7];
            struct { int m_lo, m_hi; int w[5]; };
        };
    };

    std::vector<stGenItemProb_t> m_probs;        // +0x18
    std::vector<stGenItemProb_t> m_probsSub;     // +0x24（ctor 构造；装载未用）
    int m_rarityBasis[5][6];                     // +0x30（行 0 未用；脚本行数≤4）
    char m_padA8[0x18];                          // +0xa8..0xbf（未建模区）
    stGenItemRange_t m_range;               // +0xc0..0x253
    CItemGenRateTable m_table;                   // +0x254..0x237BF
    int _getGenItemProbIdx(int value);           // ORIG T 0x08535a88
};  // sizeof 0x237C0

// ---- CSpecialMonsterDrop（特殊怪物掉落；sizeof 0x21230C）--------------------
class CSpecialMonsterDrop : public CItemGenerator
{
public:
    CSpecialMonsterDrop(int count);              // W 0x0853a426
    ~CSpecialMonsterDrop();                      // W 0x0853a4ca
    int loadScript(char const* path);            // T 0x085373ee（虚槽 +0x04）

    // ---- P6（WongWorkDrop_A 跨类调用）----
    // A 文件 CMonsterDrop::setDropRatio/generateRandomItem 需在 m_special 上调用
    // 本组方法 ⇒ protected→public。仅访问控制变化，sizeof/偏移/mangled/vtable 零变化。
    void _generateRandomItem(unsigned char type, char role, unsigned char dungeonLv,
                             unsigned char difficulty, unsigned char partyPos,
                             int aiIdx, unsigned char hellFlag, int dropRate,
                             stGenerateResult_t& result, bool& done);
                                                  // ORIG T 0x08536882
    void setDropRatio(float ratio);               // ORIG T 0x085359e8（虚槽 +0x00）

protected:
    struct stGenItemProb_t
    {
        union
        {
            int v[7];
            struct { int m_lo, m_hi; int w[5]; };
        };
    };

    std::vector<stGenItemProb_t> m_probs;        // +0x18
    std::vector<stGenItemProb_t> m_probsSub;     // +0x24
    int m_rarityBasis[5][6];                     // +0x30（行 0 未用）
    float m_partyBonus[5][4];                    // +0xa8
    float m_dungeonBonus[5][5];                  // +0xf8
    float m_monsterTypeBonus[5][4];              // +0x15c
    stGenItemRange_t m_range;               // +0x1ac..0x33f
    CSecialIMonsterDropItemGenRateTable m_table; // +0x340..0x21230B

    int _getGenItemProbIdx(int value);            // ORIG T 0x0853736e
};  // sizeof 0x21230C


// 各类 sizeof 与 ORIG operator new 分配尺寸一致（initialize.md 实证：
// CMonsterDrop/CObjectDrop 0x2593a4、CClearReward 0x23b84、CDungeonDrop 0x18）；
// 内部成员起点全部来自对应 loadScript/ctor 的立即数偏移。
class CMonsterDrop : public CItemGenerator
{
public:
    CMonsterDrop(int count);                                   // W 0x0853a6c8
    int loadScript(char const* path);                          // T 0x08537938（虚槽 +0x04）
    // 非虚三参重载（ORIG T 0x085380fa 直呼；返回错误行，0=成功）。
    int loadScript(char const* path, char const* hell, char const* special);
    // ---- G1 Drop 批次补充（WongWorkDrop_A.cpp 逐报告转写）----
    void setDropRatio(float ratio);                            // T 0x08535cf8（虚槽 +0x00）
    void generateRandomItem(stGenerateRefData_t const& ref,
                            stGenerateResult_t& result);       // T 0x08536da8（虚槽 +0x08）
    void generateSpecificItem(stGenerateRefData_t const& ref,
                              stGenerateResult_t& result);     // T 0x08537070（虚槽 +0x0c）

protected:
    int _getGenItemProbIdx(int value);                         // T 0x085372ee
    bool _getBossGoldDrop(unsigned char type, unsigned short& outGold,
                          unsigned short& outCount);           // T 0x08535dc8
    void _generateRandomItem(unsigned char type, char role, unsigned char dungeonLv,
                             unsigned char difficulty, unsigned char partyPos,
                             int context3c, int dropRate,
                             stGenerateResult_t& result, bool& done);
                                                               // T 0x08535ed2
    void _generateMonseterSpecificItem(CMonster const* monster, char idxType,
                                       stGenerateResult_t& result);
                                                               // T 0x08536fce（ORIG 拼写如此）


protected:
    struct stGenItemProb_t
    {
        // b28d-solo-WongWorkDrop_A：setDropRatio 的 ORIG 形为
        // `mov 0x8(%eax,%edx,4)`（disp 8 + scaled index r，r=0..4）⇒ 源码用
        // +0x08 起的五档数组视图 w；写 v[r+2] 时 4.4 恒先 `lea 0x2(%eax)` 再
        // disp 0 索引（P30/N4 家族），w 视图直接折叠出 ORIG 单条 disp8。
        // sizeof/偏移/成员名零影响。
        union
        {
            int v[7];
            struct
            {
                int m_padv0;        // +0x00（v[0] 占位）
                int m_padv1;        // +0x04（v[1] 占位）
                int w[5];           // +0x08..+0x1b（v[2..6] 视图）
            };
        };
    };
    struct stConditionPair_t { unsigned short rate; unsigned short value; };
    // 首领/命名怪首杀权重项（ORIG 嵌套类型；ctor/dtor W 0x0853a6ae，
    //   mangled _ZN8WongWork12CMonsterDrop25stFirstMobHuntingWeight_tC1Ev/C2Ev；
    //   实现见 WongWorkDrop_A.cpp）
public:   // 类外 typedef 需可见（布局零变化，仅访问级别调整）
    struct stFirstMobHuntingWeight_t
    {
        stFirstMobHuntingWeight_t() throw();   // [b27 DL3R 探针] 无抛体：4.1 侧对
                                               // 本 ctor 调用未布 Special cleanup landing
        int m_weight;      // +0x00
        int m_field04;     // +0x04
    };
protected:

    std::vector<stGenItemProb_t> m_probs;            // +0x18
    std::vector<stGenItemProb_t> m_probsSub;         // +0x24
    int m_rarityBasis[5][6];                         // +0x30（行 0 未用）
    float m_partyBonus[5][4];                        // +0xa8
    float m_dungeonBonus[5][5];                      // +0xf8
    float m_monsterTypeBonus[5][4];                  // +0x15c
    stGenItemRange_t m_range;                   // +0x1ac..0x33f
    CItemGenRateTable m_table;                       // +0x340..0x238AB
    CMonsterDrop_Hell m_hell;                        // +0x238ac..0x4706B
    CSpecialMonsterDrop m_special;                   // +0x4706c..0x259377
    stFirstMobHuntingWeight_t m_firstMobHunt;        // +0x259378
    int m_firstMobWeight;                            // +0x259380（ctor=1）
    float m_firstMobWeightRate;                      // +0x259384（ctor=1.0f）
    // b28d-solo-WongWorkDrop_A：_getBossGoldDrop 的 ORIG 对 value 读用
    // `movzwl 0x25938a(%eax,%edx,4)` 单条 disp32（4.4 对 .value 恒先
    // `add $0x964e0,%edx` 再 disp8）；二维 u16 视图 [t][1] 复现 disp 折叠。
    union
    {
        stConditionPair_t m_conditionRate[4];        // +0x259388..0x259397
        unsigned short m_conditionRateW[4][2];       // [t][0]=rate [t][1]=value
    };
    // b28d-solo-WongWorkDrop_A：ORIG 对 m_goldQuantity[2*tier(+1)] 用
    // `add $0x964e4,%edx; movzwl 0xa(%eax,%edx,4)`（tier scaled 4）；
    // 二维视图 [tier][lo/hi] 复现该折叠（4.4 对 2*tier 恒 lea+scaled 2）。
    struct stGoldQty_t
    {
        unsigned short m_lo;                         // +0
        unsigned short m_hi;                         // +2
    };
    union
    {
        unsigned short m_goldQuantity[4];            // +0x259398..0x25939F
        unsigned short m_goldQuantityW[2][2];        // [tier][0]=lo [tier][1]=hi
        stGoldQty_t m_goldQuantityS[2];              // ORIG add $K + disp 折叠视图
    };
    unsigned short m_goldVolume[2];                  // +0x2593a0..0x2593a3
};  // sizeof 0x2593a4
// 类外同型 typedef：WarField 等既有非限定名引用保持编译（同一类型，无 ODR
// 分裂；sizeof 断言沿用 WongWork::stFirstMobHuntingWeight_t 名字）。
typedef CMonsterDrop::stFirstMobHuntingWeight_t stFirstMobHuntingWeight_t;

// CObjectDrop 以 CMonsterDrop 为基（ORIG ctor 0x0853a7f8 调 CMonsterDropC1 再置
// vptr 0x08c9a730）；自身覆写虚槽 +0x04（ORIG T 0x08538352）并预置队伍加成 1.0f。
class CObjectDrop : public CMonsterDrop
{
public:
    CObjectDrop(int count);                                    // W 0x0853a7f8
    int loadScript(char const* path);                          // T 0x08538352
    // 虚槽覆写（G1 Drop 批次；T 0x0853818a / T 0x08538234）
    void generateRandomItem(stGenerateRefData_t const& ref,
                            stGenerateResult_t& result);
    void generateSpecificItem(stGenerateRefData_t const& ref,
                              stGenerateResult_t& result);
};
// ---- 掉落概率档位（ORIG 编码属 namespace WongWork 层：CClearReward 方法
//      参数以 NS_ 引用，见 _ZN8WongWork12CClearReward18_getGenItemProbIdxEiNS_22eClearRewardDropProb_tE）
enum eClearRewardDropProb_t
{
    eDropProb_default = 0,
    eDropProb_event = 1,
    eDropProb_event2 = 2,
    eDropProb_pcroomDefault = 3,
    eDropProb_pcroomEvent = 4,
    eDropProb_pcroomBonus = 5
};


class CClearReward : public CItemGenerator
{
public:
    void setDropRatio(float ratio);                            // T 0x085385c8（虚槽 +0x00）
    CClearReward(int count);                                   // W 0x0853a968（C1/C2 同址）
    int loadScript(char const* path);                          // T 0x085392c0

    // 同型 typedef：类内名字可见（方法签名/调用方不变；mangled 归 ns 层）。
    typedef WongWork::eClearRewardDropProb_t eClearRewardDropProb_t;
    // 非虚查询（ORIG T/W 直呼；clear_reward 消费）
    int getGoldCardCommission(int n);                          // T 0x085385b2
    float getGoldCardCreateRate();                             // W 0x085bf000
    int getRewardItemRatePerMapMaxCount(int mapCount) const;   // W 0x085bf018

    // 虚槽 +0x08/+0x0c 实现（ORIG T）
    void generateRandomItem(stGenerateRefData_t const& ref,
                            stGenerateResult_t& result);       // T 0x0853873a
    void generateSpecificItem(stGenerateRefData_t const& ref,
                              stGenerateResult_t& result);     // T 0x08538c84
    // 非虚金币/金卡族（clear_reward 经 getGenerator(2) 直呼）
    void generatePremiumGoldCardItem(stGenerateRefData_t const& ref,
                                     stGenerateResult_t& result,
                                     ENUM_RARITY rarity, unsigned int defaultItem);
                                                                 // T 0x08538a92
    int generateGoldCardBlankItem(Inven_Item& item);            // T 0x08539166（返回 0/1）
    int generatePcRoomCardBlankItem(Inven_Item& item);          // T 0x0853900c（返回 0/1）
    void power_war_dungeon_gold_clear_reward(stGenerateRefData_t const& ref,
                                             stGenerateResult_t& result,
                                             int level);        // T 0x08538e64

protected:
    struct stGenItemProb_t { int v[3]; };                      // 每行三值 {lo, hi, value}
    // [b28d-solo-CClearReward] 三元组为 unsigned int：ORIG generateGoldCardBlankItem
    // 0x08539166/0x0853900c 实证 —— v[1]→Inven_Item::m_addInfo(unsigned int) 单段直落
    // 无中转（同型），v[2]→CItemList::create_item(...,int) 为 `mov 0x8(%eax),%eax;
    // mov %eax,%ebx` 两段中转（异型）。sizeof/偏移/mangled 不变。
    struct stGoldCardBlankItemInfo_t { unsigned int v[3]; };    // 空白卡三元组 {weight, itemIdx, count}

    // 12×vector 连续区（0.9 字节/个）：前 6 为 loadScript 脚本源组[A]，后 6 为
    // setDropRatio 缩放副本[B]（setDropRatio 0x085385c8 复制 0x18..0x5f → 0x60..0xa7，
    // _generateRandomItem/_getGenItemProbIdx 只读 [B]；ORIG ctor 构造全部 12 个）。
    std::vector<stGenItemProb_t> m_scriptProbs[6];   // +0x18..0x5f
    std::vector<stGenItemProb_t> m_workProbs[6];     // +0x60..0xa7
    int m_dropKindProb[2];                           // +0xa8/+0xac（[drop kind prob] 阈值/概率）
    char m_padb0[4];                                 // +0xb0..0xb3
    int m_dropItemTypeProb[4];                       // +0xb4（ORIG 索引 1..4 → 下标 i-1）
    int m_dungeonDropBonus[5];                       // +0xc4
    float m_dungeonGoldBonus[5];                     // +0xd8
    float m_partyBonus[4];                           // +0xec
    int m_rarityBasis[6];                            // +0xfc
    stGenItemRange_t m_range;                   // +0x114..0x2a7
    CItemGenRateTable m_table;                       // +0x2a8..0x23813
    int m_goldCardCost[201];                         // +0x23814..0x23b37
    float m_goldCardCreateRate;                      // +0x23b38
    int m_goldCardBlankTotal;                        // +0x23b3c
    std::vector<stGoldCardBlankItemInfo_t> m_goldBlanks;    // +0x23b40
    int m_rewardRatePerMap[10];                      // +0x23b4c（ctor memset 0x28）
    int m_pcRoomBlankTotal;                          // +0x23b74
    std::vector<stGoldCardBlankItemInfo_t> m_pcRoomBlanks;  // +0x23b78

private:
    // clear_reward 族内部实现（ORIG T；定义 CClearReward.cpp）
    int _getGenItemProbIdx(int value, eClearRewardDropProb_t drop);  // T 0x08538694
    void _generateRandomItem(stGenerateRefClearReward_t const& refClear, int param,
                             stGenerateResult_t& result, float dropRate);
                                                               // T 0x085387c0
    void _generateSpecificItem(stGenerateRefClearReward_t const& refClear,
                               stGenerateResult_t& result);    // T 0x08538bf8
};  // sizeof 0x23b84


class CDungeonDrop : public CItemGenerator
{
public:
    // （stGenerateRefClearReward_t：生成器域前置声明于上方，完整定义在
    //   CClearReward.cpp；ORIG mangled 属 WongWork::，勿移至全局区。）
    CDungeonDrop(int count);                                   // W 0x0853abc2
    // 虚槽 +0x0c 覆写（G1 Drop 批次；T 0x08539c3e）
    void generateSpecificItem(stGenerateRefData_t const& ref,
                              stGenerateResult_t& result);
};  // sizeof == 0x18 == ORIG 分配尺寸（ORIG 无 loadScript 覆写符号，虚槽走基类）

class CWorldDrop : public CItemGenerator
{
public:
    CWorldDrop(int count);                                     // W 0x0853a826
    // 虚槽 +0x04 覆写（vtable _ZTVN8WongWork10CWorldDropE 0x08c9a710；ORIG W
    // 0x0853a81c 空实现 return 0 —— ORIG 即如此，非占位）。
    int loadScript(char const* path);
    // 虚槽 +0x08 覆写（G1 Drop 批次；T 0x085383b8）
    void generateRandomItem(stGenerateRefData_t const& ref,
                            stGenerateResult_t& result);
    int loadScript(char const* path, char const* warArea);

    WorldDropInfo m_worldDrop[201];     // +0x18（ctor 循环 WorldDropInfoC1Ev ×0xc8）
    WorldDropInfo m_warAreaDrop[201];   // +0xfcc（双参 loadScript 第二装载目标）
};  // sizeof == 0x1f80 == ORIG 分配尺寸
 
 }  // namespace WongWork
 
// 布局自检（置于定义之外；本工具链实测 vector<bool>=0x14、std::map<int,int>=0x18）。
typedef char assert_sizeof_wg_luckpoint[sizeof(::CLuckPoint) == 0x24 ? 1 : -1];
typedef char assert_sizeof_refdata[sizeof(WongWork::stGenerateRefData_t) == 0x48 ? 1 : -1];
typedef char assert_sizeof_result[sizeof(WongWork::stGenerateResult_t) == 0x30 ? 1 : -1];
typedef char assert_sizeof_genrand[sizeof(WongWork::CGenerateRandomNumber) == 8 ? 1 : -1];
typedef char assert_sizeof_genupgrade[sizeof(WongWork::GenerateRandomUpgrade) == 4 ? 1 : -1];
typedef char assert_sizeof_itemgen[sizeof(WongWork::CItemGenerator) == 0x18 ? 1 : -1];
typedef char assert_sizeof_gencommon[sizeof(WongWork::CGeneratorCommonData) == 0x648 ? 1 : -1];

#endif  // GAME_CITEMGENERATOR_H_

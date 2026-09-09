#include "GameRand.h"
// df_game_r CStackableItem 还原（I1 批次，2026-08-17）。
// 函数形态以 docs/class_func_reports/CStackableItem.md + ORIG weak 反汇编为准
// （AE 口径：调用目标归一；字段偏移/常量逐条一致）。
// 依赖的辅助类（CMTRand/CSystemTime/CDataManager 等）由 GameStubs.cpp 提供桩；
// STStackableScript 内联完整布局为 I1 工作布局（脚本表批次细化）。

#include "CStackableItem.h"

#include "PvPTypes.h"   // 权威 CMTRand 完整类型（new CMTRand(seed) 需要；原局部 stub 已删除）
#include "AradRabbitBox.h"  // +0x19c 兔盒元素类型（W6_ARAD_ItemBonus 收敛）

#include <algorithm>
#include <string>
#include <string.h>
#include <time.h>

#include "../../ServerCommon/DNFFunctionLib.h"


// 最小构造/析构（ORIG 逐成员构造属后续精修批次；保证可链接）
// b28c-L8 判词 [N(布局建模)]：fresh 复验 _ZN14CStackableItemC1Ev 292=292，唯一残差
// 4 行 = 本类 vptr 写入时机。ORIG 0x08515763 构造序 = `call _ZN5CItemC1Ev`(0x85152d4)
// → `add $0x148; call _ZN12AradItemInfoC1Ev`(0x8515618) → **然后**才
// `movl $0x8c96ea8,(%eax)` 写本类 vptr → RecipeInfo(+0x174) / LotteryInfo(+0x1c0) / …
// ⇒ AradItemInfo 必为**次基类**而非成员（CItem sizeof=0x148 权威 ⇒ 次基类恰落 +0x148，
// 全部偏移/sizeof 不变；AradItemInfo 无虚函数 ⇒ 不引入第二 vptr）。成员模型下 4.4
// 恒在首成员 ctor 之前写 vptr，源码不可翻。同法先例：C28L9 AvatarRechargeServer.h、
// C28L16 AvatarFixedHiddenOptionServer.h。本波不改 CStackableItem.h（跨
// C28L17/18/21 三方写权 CItemList.cpp + Arad_ItemBonus.cpp 消费面），spec 已广播交
// Main 统一批次；改动清单=头继承表 + CStackableItem.cpp 12 处 + Arad_ItemBonus.cpp:431
// + CItemList.cpp:1986 去 `m_arad148.` 前缀。
// [b28d-N(布局建模)｜待专项批次] fresh 复验 292=292 维持 N：唯一残差 = 本类 vptr
// 写入时机（AradItemInfo 须为次基类，跨 CItemList.cpp/Arad_ItemBonus.cpp 消费面）。
// Main 2026-09-09 裁决：**本轮不做**（收益仅本件 4 行残差，代价为全域构造/析构语义
// 变更 + 14 处消费点 + 多基类 RTTI/EH 风险），spec 存档待专项批次：
//   头改 `class CStackableItem : public CItem, public AradItemInfo`，删 m_arad148 成员，
//   去 CStackableItem.cpp 12 处 `m_arad148.` 前缀 + CItemList.cpp:1986 + Arad_ItemBonus.cpp:431；
//   CItem sizeof=0x148 权威 ⇒ 全部偏移/sizeof 不变、AradItemInfo 无虚函数 ⇒ 不引入第二 vptr。
CStackableItem::CStackableItem()
{
}

CStackableItem::~CStackableItem()
{
}

// ORIG T 0x089c2832：if-return 链 + strcmp（每分支 `mov $N,%eax; jmp` 统一出口，
// 尾项默认 `mov $0x9,%eax` 直落；P17/P47 形态）。字符串映射（.rodata 0x8e19e71..）：
//   "[avatar]"→0 "[special avatar]"→1 "[cera]"→2 "[creature]"→3 "[equipment]"→4
//   "[stackable]"→5 "[etc]"→6 "[special creature]"→7 "[emblem]"→8，未知→9。
int stStackableBooster_t::tag2Type(const char* tag)
{
    if (strcmp(tag, "[avatar]") == 0) return 0;
    if (strcmp(tag, "[special avatar]") == 0) return 1;
    if (strcmp(tag, "[cera]") == 0) return 2;
    if (strcmp(tag, "[creature]") == 0) return 3;
    if (strcmp(tag, "[equipment]") == 0) return 4;
    if (strcmp(tag, "[stackable]") == 0) return 5;
    if (strcmp(tag, "[etc]") == 0) return 6;
    if (strcmp(tag, "[special creature]") == 0) return 7;
    if (strcmp(tag, "[emblem]") == 0) return 8;
    return 9;
}

// ORIG T 0x089c295a：同 tag2Type 形态；结束标签 "[/xxx]" 映射同值表
// （.rodata 0x8e19ec6..）："[/avatar]"→0 .. "[/emblem]"→8，未知→9。
int stStackableBooster_t::tag2EndType(const char* tag)
{
    if (strcmp(tag, "[/avatar]") == 0) return 0;
    if (strcmp(tag, "[/special avatar]") == 0) return 1;
    if (strcmp(tag, "[/cera]") == 0) return 2;
    if (strcmp(tag, "[/creature]") == 0) return 3;
    if (strcmp(tag, "[/equipment]") == 0) return 4;
    if (strcmp(tag, "[/stackable]") == 0) return 5;
    if (strcmp(tag, "[/etc]") == 0) return 6;
    if (strcmp(tag, "[/special creature]") == 0) return 7;
    if (strcmp(tag, "[/emblem]") == 0) return 8;
    return 9;
}

// ORIG 0x8515128：逐字段复制（3 int + vector<string> + char）
BoosterSelectInfo& BoosterSelectInfo::operator=(
    const BoosterSelectInfo& other)
{
    m_field0 = other.m_field0;
    m_field4 = other.m_field4;
    m_field8 = other.m_field8;
    m_nameVec = other.m_nameVec;
    m_field18 = other.m_field18;
    return *this;
}

// ORIG 0x8513b82：vector<string> 默认构造 + 三 int movl 清零 + char movb 清零
// （P10/P33：m_field18 必须 char）。
BoosterSelectInfo::BoosterSelectInfo()
{
    // [b28c-27] AE：ORIG 0x8513bb8 先写 m_field18 再 vector::clear（源码语句序
    // 必须同序；原序 clear→field18 使 landing pad 布局差 2 条）。
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_field18 = '\0';
    m_nameVec.clear();
}

int get_rand_int(int range);

enum ENUM_ITEM_CREATE_TYPE
{
    ENUM_ITEM_CREATE_TYPE_0 = 0,
    ENUM_ITEM_CREATE_TYPE_1 = 1
};

class CItemList
{
public:
    void create_item(ENUM_ITEM_CREATE_TYPE type, Inven_Item& item, int param);
};

class CEnvironment;
extern CEnvironment* G_CEnvironment();

struct CEnvironmentView
{
    char m_pad[0x1a8];
    int m_serverType;   // +0x1a8
};

class CDataManager
{
public:
    void set_AuctionRegFreeCeraItem(unsigned int idx);
    void set_lottery_use_cost(unsigned int cost, unsigned int flag);
    char m_pad[0xc];
    class CItemList* m_itemList;   // +0x0c
};
extern CDataManager* G_CDataManager();

struct CDataManagerView
{
    char m_pad[0x28];
    int m_field28;   // +0x28
};

#include "CSystemTime.h"

#include "ItemGloballyUniqueIdentifierGenerator.h"  // itemGloballyUniqueIdentifier_t 权威定义


class GameWorld;
extern GameWorld* G_GameWorld();

class GameWorld
{
public:
    bool IsPVPChannel() const;
};

class CUserCharacInfo;
class CUser
{
public:
    int get_state();
    CUserCharacInfo* get_charac_info();
};

class CUserCharacInfo
{
public:
    char getCurCharFirstGrowType() const;
};

// ORIG 0x8a86d28：26 项字符串→枚举映射，默认 0x16
int GetEquipmentTypeStringToEnum(std::string& type)
{
    int result = 0x16;
    if (type == "[hat avatar]")
    {
        result = 0;
    }
    else if (type == "[hair avatar]")
    {
        result = 1;
    }
    else if (type == "[face avatar]")
    {
        result = 2;
    }
    else if (type == "[coat avatar]")
    {
        result = 3;
    }
    else if (type == "[pants avatar]")
    {
        result = 4;
    }
    else if (type == "[shoes avatar]")
    {
        result = 5;
    }
    else if (type == "[breast avatar]")
    {
        result = 6;
    }
    else if (type == "[waist avatar]")
    {
        result = 7;
    }
    else if (type == "[skin avatar]")
    {
        result = 8;
    }
    else if (type == "[aurora avatar]")
    {
        result = 9;
    }
    else if (type == "[weapon]")
    {
        result = 10;
    }
    else if (type == "[title name]")
    {
        result = 11;
    }
    else if (type == "[coat]")
    {
        result = 12;
    }
    else if (type == "[shoulder]")
    {
        result = 13;
    }
    else if (type == "[pants]")
    {
        result = 14;
    }
    else if (type == "[shoes]")
    {
        result = 15;
    }
    else if (type == "[waist]")
    {
        result = 16;
    }
    else if (type == "[amulet]")
    {
        result = 17;
    }
    else if (type == "[wrist]")
    {
        result = 18;
    }
    else if (type == "[ring]")
    {
        result = 19;
    }
    else if (type == "[support]")
    {
        result = 20;
    }
    else if (type == "[magic stone]")
    {
        result = 21;
    }
    else if (type == "[creature]")
    {
        result = 22;
    }
    else if (type == "[artifact red]")
    {
        result = 23;
    }
    else if (type == "[artifact blue]")
    {
        result = 24;
    }
    else if (type == "[artifact green]")
    {
        result = 25;
    }
    return result;
}

// ===================== StackableAction 字符串→枚举映射（ORIG T） =====================
// getStackableActionType @0x089aeb9b：14 项字符串→Type 映射，默认 0（if-return 链，
// 每项 call eq/test/je/mov+jmp 统一出口，与 ORIG 同形）。
int getStackableActionType(std::string& type)
{
    if (type == "[megaphone]")
    {
        return 0x16;
    }
    if (type == "[upgrade cargo]")
    {
        return 0x2b;
    }
    if (type == "[chatemoticon]")
    {
        return 0x2c;
    }
    if (type == "[seria room deco]")
    {
        return 0x2a;
    }
    if (type == "[open guide book]")
    {
        return 0x2d;
    }
    if (type == "[class change]")
    {
        return 0x2e;
    }
    if (type == "[character name change]")
    {
        return 0x33;
    }
    if (type == "[bingo]")
    {
        return 0x34;
    }
    if (type == "[portable upgrade]")
    {
        return 0x35;
    }
    if (type == "[add fatigue]")
    {
        return 0x37;
    }
    if (type == "[candle golden wax]")
    {
        return 0x07;
    }
    if (type == "[add avatar socket]")
    {
        return 0x20;
    }
    if (type == "[LotteryCount]")
    {
        return 0x36;
    }
    if (type == "[equipment purify]")
    {
        return 0x21;
    }
    return 0;
}

// getStackableActionUsablePlace @0x089aed67：5 项字符串→UsablePlace，默认 1
int getStackableActionUsablePlace(std::string& place)
{
    if (place == "[village]")
    {
        return 1;
    }
    if (place == "[dungeon]")
    {
        return 2;
    }
    if (place == "[pvp]")
    {
        return 3;
    }
    if (place == "[seria room]")
    {
        return 4;
    }
    if (place == "[dont use assult]")
    {
        return 5;
    }
    return 1;
}

// getStackableActionUsableServerType @0x089aee0a：3 项字符串→UsableServerType，默认 1
int getStackableActionUsableServerType(std::string& type)
{
    if (type == "[general server]")
    {
        return 1;
    }
    if (type == "[event server]")
    {
        return 2;
    }
    if (type == "[starter server]")
    {
        return 3;
    }
    return 1;
}

void createRecipeInfo(RecipeInfo* info, std::vector<int>* source,
                      std::vector<std::pair<int, int> >* extra);
void createLotteryInfo(LotteryInfo* info, std::vector<int>* source);
void createStackableLotteryInfo(LotteryInfo* info, std::vector<int>* source);
void createStackableMultiBoxLotteryInfo(
    MultiBoxLotteryInfo* info,
    std::vector<std::vector<int> >* a, std::vector<int>* b,
    std::vector<int>* c);
int createGlobalEffectInfo(std::vector<stGlobalEffect_t>& out,
                           std::vector<int>* source);


// createRecipeInfo（自 GameStubs.cpp 迁移；ORIG 0x89ba83c）
// 语义：clear 后按 source 布局解析 3 组 pair 表 + 尾部 2 个标记字节 + 可选 extra 拷贝。
//   source = [count1, (p0,p1)xcount1, count2, (p0,p1)xcount2, count3, (p0,p1)xcount3,
//             tailValue?, ...]
//   m_vec0/count1、m_vecC/count2、m_vec18/count3；
//   若 source 仍有剩余元素：m_field24 = (tail==1)，m_field25 = (tail==2)（均读同一元素），
//   且 extra!=0 时 m_vec28 = *extra。
// 【N90】差值主体=libstdc++4.1 iterator/find 非 inline sret 外联（ORIG 8 insns/步 vs 4.4 inline 4）；豁免不逐形追。
// [b28d-solo-CStackableItem] NEAR 225=225。本波新增 P190 附注「2 个取址 double framePad」
// 撑帧 0x68→0x78（与 ORIG sub $0x78 逐值一致），逐行 diff 356→342。已按 ORIG 重写：
// `int n = *it; it++;` 拆句、`int i = 0; for (i = 0; …)` 双初始化（P191）、
// pair 逐字段赋值、尾部早退 `if (it == source->end()) return;`（P214）。
// 残余判 N95/N82：迭代器组槽 ORIG -0x48/-0x44/-0x40 vs 我们 -0x4c/-0x50（非均匀
// 偏移，声明序两形实测均不翻）+ n/i 槽序。
void createRecipeInfo(RecipeInfo* info, std::vector<int>* source,
                      std::vector<std::pair<int, int> >* extra)
{
    // b28d-solo 修法（P190 附注）：双取址 double 撑帧 +0x10（见函数头判词）。
    double framePad;
    double framePad2;
    (void)&framePad;
    (void)&framePad2;
    info->clear();
    std::vector<int>::iterator it = source->begin();
    std::vector<int>::iterator end = source->end();
    if (it == end)
    {
        return;
    }
    int n = *it;
    it++;
    int i = 0;
    for (i = 0; i < n; ++i)
    {
        std::pair<int, int> p;
        p.first = *it;
        it++;
        p.second = *it;
        it++;
        info->m_vec0.push_back(p);
    }
    n = *it;
    it++;
    for (i = 0; i < n; ++i)
    {
        std::pair<int, int> p;
        p.first = *it;
        it++;
        p.second = *it;
        it++;
        info->m_vecC.push_back(p);
    }
    n = *it;
    it++;
    for (i = 0; i < n; ++i)
    {
        std::pair<int, int> p;
        p.first = *it;
        it++;
        p.second = *it;
        it++;
        info->m_vec18.push_back(p);
    }
    if (it == source->end())
    {
        return;
    }
    info->m_field24 = (*it == 1);
    info->m_field25 = (*it == 2);
    if (extra != 0)
    {
        info->m_vec28 = *extra;
    }
}
// ============================================================================
// LotteryInfo 生命周期 + 解析辅助（自 GameStubs.cpp 迁移，保持 ORIG 语义）：
//   LotteryInfo C1/D1/clear（ORIG 0x83706d4 / 0x837069c）
//   createLotteryInfo（ORIG 0x89babc5）
//   createStackableMultiBoxLotteryInfo（ORIG 0x89bafd1；已实现，见下）
// ============================================================================
// b28c-L8 代落 C28L25（lane25 件）：ORIG C1 0x083706d4 仅构造两 vector 成员后
// call clear()，无 +0x00/+0x10 标量清零 ⇒ 标量不入初始化列表（清零由 clear 承担）。
LotteryInfo::LotteryInfo() : m_items(), m_counts()
{
    clear();
}
LotteryInfo::~LotteryInfo() {}
// ORIG 0x837069c
void LotteryInfo::clear()
{
    m_defaultIdx = 0;
    m_items.clear();
    m_defaultCount = 0;
    m_counts.clear();
}
// ORIG 0x89babc5：src 相邻元素配对填充 m_items，m_counts 每对补 1；奇数时 break
// 【N90】差值主体=libstdc++4.1 iterator/find 非 inline sret 外联（ORIG 8 insns/步 vs 4.4 inline 4）；豁免不逐形追。
// [b28d-N90] fresh 复验 97/85：维持 N90（同上，迭代器外联 + 帧差）。
void createLotteryInfo(LotteryInfo* info, std::vector<int>* source)
{
    info->clear();
    std::vector<int>::iterator it = source->begin();
    info->m_defaultIdx = *it;
    it++;
    info->m_defaultCount = 1;
    while (it != source->end())
    {
        std::pair<int, int> p;
        p.first = *it;
        it++;
        if (it == source->end())
            break;
        p.second = *it;
        it++;
        info->m_items.push_back(p);
        info->m_counts.push_back(1);
    }
}

// ORIG 0x89bafd1：解析 multibox 彩票脚本。
//   a (vector<vector<int>>)：每元素是一个 LotteryUnit 的原始 int 流
//     [count, defaultIdx, defaultNum, defaultFlag, (itemIdx,weight,count,flag)xN]
//     —— m_items.push_back({itemIdx,weight})，m_bonus.push_back({count,flag})
//   b (vector<int>)：field8=b[0]，fieldc=b[1]
//   c (vector<int>)：m_defaultCount/Idx/Num/Flag + m_items(stDefaultItem) 流
// 【N90】差值主体=libstdc++4.1 iterator/find 非 inline sret 外联（ORIG 8 insns/步 vs 4.4 inline 4）；豁免不逐形追。
// [b28d-solo-CStackableItem] fresh 复验 667/656（起点 652/638）：a 部分已按 ORIG 重写
// ——`vec(*itA)` → `it = (*itA).begin()`（deEv 非 ptEv）→ unit 构造/clear；边界一律
// `(*itA).end()`；失败路径改正极性早退 `if (it == (*itA).end()) break;`；b 部分改
// `if (it == b->end()) return;`；c 部分 `*it`/`it++` 拆句 + 内层 break（ndiff 902→499）。
// [b28d-N90/N95] 判 N 封顶（fresh 652/648）：本波新增**前置死局部 `int framePad;`**
// （P232 复活）令全函数槽基回推 4，纯槽号差行 346→216、逐行 diff 857→518 行
// （加 pad 前 ORIG -0xa4/-0xb8 = 我们 -0xa0/-0xb4；加后首 250 行逐条对齐）。
// 残余 = STL 迭代器外联结构差（N90：4.1 每步 8 insn sret 外联 vs 4.4 inline 4；
// p167 差集 = ORIG `__gnu_cxx::eq`×6 / `deEv`×1 / `vector<int>::end`×1 vs
// 我们 `ne`×5 / stDefaultItem D1×2 / _Unwind_Resume×1 = N89/N91 族）
// + c 部分嵌套 if 的 ppEi/dummy-arg 与 eq/jne 极性（N86/N108）+ 剩余局部槽序（N95）。
void createStackableMultiBoxLotteryInfo(
    MultiBoxLotteryInfo* info,
    std::vector<std::vector<int> >* a, std::vector<int>* b,
    std::vector<int>* c)
{
    // b28d-solo 修法（P232）：前置死局部回推槽基 4（见函数头判词）。
    int framePad;
    (void)framePad;
    if (info == 0 || a == 0 || b == 0)
    {
        return;
    }
    info->clear();
    // ---- 解析 a：构建 m_units ----
    // [b28d-solo-CStackableItem] ORIG 0x89bafd1：`vec(*itA)` → `it = (*itA).begin()`
    // → `unit` 构造/clear（it 先于 unit）；所有边界比较用 **(*itA).end()**（原码如此，
    // 非 vec.end()）；每次 `*it` 与 `it++` 为两条独立语句。
    std::vector<std::vector<int> >::iterator itA = a->begin();
    while (itA != a->end())
    {
        std::vector<int> vec(*itA);
        std::vector<int>::iterator it = (*itA).begin();
        MultiBoxLotteryInfo::LotteryUnit unit;
        unit.clear();
        unit.m_count = *it;
        it++;
        unit.m_defaultIdx = *it;
        it++;
        if (it == (*itA).end())
        {
            break;
        }
        unit.m_defaultNum = *it;
        it++;
        if (it == (*itA).end())
        {
            break;
        }
        unit.m_defaultFlag = (*it != 0);
        it++;
        if (it == (*itA).end())
        {
            break;
        }
        while (it != (*itA).end())
        {
            std::pair<int, int> p_item, p_bonus;
            p_item.first = *it;
            it++;
            if (it == (*itA).end()) break;
            p_item.second = *it;
            it++;
            if (it == (*itA).end()) break;
            p_bonus.first = *it;
            it++;
            if (it == (*itA).end()) break;
            p_bonus.second = *it;
            it++;
            unit.m_items.push_back(p_item);
            unit.m_bonus.push_back(p_bonus);
        }
        info->m_units.push_back(unit);
        itA++;
    }
    // ---- 解析 b：field8 / fieldc ----
    if (!b->empty())
    {
        std::vector<int>::iterator it = b->begin();
        info->m_field8 = *it;
        it++;
        if (it == b->end())
        {
            return;
        }
        info->m_fieldc = *it;
        it++;
    }
    // ---- 解析 c：默认项 + m_items ----
    std::vector<int>::iterator it = c->begin();
    if (it != c->end())
    {
        info->m_default.m_defaultCount = *it;
        it++;
        if (it != c->end())
        {
            info->m_default.m_defaultIdx = *it;
            it++;
            if (it != c->end())
            {
                info->m_default.m_defaultNum = *it;
                it++;
                if (it != c->end())
                {
                    info->m_default.m_defaultFlag = (char)*it;
                    it++;
                    if (it != c->end())
                    {
                        while (it != c->end())
                        {
                            MultiBoxLotteryInfo::stDefaultItem item;
                            item.m_itemIdx = *it;
                            it++;
                            if (it == c->end()) break;
                            item.m_prob = *it;
                            it++;
                            if (it == c->end()) break;
                            item.m_count = *it;
                            it++;
                            if (it == c->end()) break;
                            item.m_flag = (char)*it;
                            it++;
                            info->m_default.m_items.push_back(item);
                        }
                    }
                }
            }
        }
    }
}

class CChattingEmoticonList
{
public:
    void* find_emoticon(int idx);
};

class STStackableScript
{
public:
    STStackableScript();
    ~STStackableScript();
    int m_field0;                        // +0x00
    int m_field4;                        // +0x04
    int m_field8;                        // +0x08
    int m_fieldc;                        // +0x0c
    int m_field10;                       // +0x10
    int m_field14;                       // +0x14
    int m_field18;                       // +0x18
    int m_field1c;                       // +0x1c
    int m_field20;                       // +0x20
    int m_field24;                       // +0x24
    int m_field28;                       // +0x28
    int m_field2c;                       // +0x2c
    char m_field30;                      // +0x30
    bool m_field31;                      // +0x31
    char m_pad32[6];                     // +0x32
    bool m_usable[0xb];                  // +0x38
    char m_pad43[0x19];                  // +0x43
    int m_field5c;                       // +0x5c
    int m_field60;                       // +0x60
    int m_field64;                       // +0x64
    int m_field68;                       // +0x68
    std::string m_str6c;                 // +0x6c
    char m_pad70[0x18];                  // +0x70
    std::vector<int> m_vec88;            // +0x88
    std::vector<std::string> m_vec94;    // +0x94
    char m_pada0[0xc];                   // +0xa0
    std::string m_strac;                 // +0xac
    char m_padb0[0x14];                  // +0xb0
    int m_fieldc4;                       // +0xc4
    int m_fieldc8;                       // +0xc8
    int m_fieldcc;                       // +0xcc
    bool m_fieldd0;                      // +0xd0
    char m_padd1[7];                     // +0xd1
    bool m_fieldd8;                      // +0xd8
    bool m_fieldd9;                      // +0xd9
    bool m_fieldda;                      // +0xda
    char m_paddb[1];                     // +0xdb
    std::set<int> m_setdc;               // +0xdc
    int m_fieldf4;                       // +0xf4
    int m_fieldf8;                       // +0xf8
    char m_fieldfc;                      // +0xfc
    char m_padfd[3];                     // +0xfd
    int m_field100;                      // +0x100
    char m_field104;                     // +0x104
    STSpecialMonsterDrop m_specialDrop;  // +0x105
    char m_field10e;                     // +0x10e
    bool m_field10f;                     // +0x10f
    std::vector<std::pair<int, int> > m_vec110;  // +0x110
    std::set<STItemScript::ENUM_ITEM_CATEGORY> m_set11c;  // +0x11c
    char m_pad134[0x1c];                 // +0x134
    int m_field150;                      // +0x150
    int m_field154;                      // +0x154
    std::vector<STItemScript::SEXPERTJOB::SUSABLE> m_vec158;  // +0x158
    DyeInfo m_dye164;                    // +0x164
    char m_pad174[4];                    // +0x174
    int m_field178;                      // +0x178
    char m_field17c;                     // +0x17c
    char m_pad17d[3];                    // +0x17d
    std::string m_str180;                // +0x180
    std::string m_str184;                // +0x184
    std::string m_str188;                // +0x188
    int m_field18c;                      // +0x18c
    std::vector<int> m_vec190;           // +0x190
    std::vector<CRabbitBox> m_vec19c;    // +0x19c（ORIG vector<CRabbitBox>，secret_shop_event_item 实证）
    char m_pad1a8[4];                    // +0x1a8
    int m_field1ac;                      // +0x1ac
    int m_field1b0;                      // +0x1b0
    int m_field1b4;                      // +0x1b4
    char m_pad1b8;                       // +0x1b8
    unsigned char m_field1b9;            // +0x1b9
    unsigned char m_field1ba;            // +0x1ba
    unsigned char m_field1bb;            // +0x1bb
    unsigned char m_field1bc;            // +0x1bc
    unsigned char m_field1bd;            // +0x1bd
    unsigned char m_field1be;            // +0x1be
    char m_field1bf;                     // +0x1bf
    bool m_field1c0;                     // +0x1c0
    char m_pad1c1[3];                    // +0x1c1
    int m_field1c4;                      // +0x1c4
    char m_pad1c8[8];                    // +0x1c8
    char m_field1d0;                     // +0x1d0
    char m_pad1d1[0xf];                  // +0x1d1
    int m_field1e0;                      // +0x1e0
    int m_field1e4;                      // +0x1e4
    int m_field1e8;                      // +0x1e8
    std::vector<int> m_vec1ec;           // +0x1ec
    std::vector<StackableAction::UsablePlace> m_vec1f8;  // +0x1f8
    char m_pad204;                       // +0x204
    char m_field205;                     // +0x205
    char m_pad206[0xe];                  // +0x206
    std::vector<StackableAction::UsableServerType> m_vec214;  // +0x214
    int m_field220;                      // +0x220
    std::vector<int> m_vec224;           // +0x224
    char m_pad230[4];                    // +0x230
    std::vector<std::vector<int> > m_vec234;  // +0x234
    char m_pad240[4];                    // +0x240
    std::vector<std::vector<int> > m_vec244;  // +0x244
    std::vector<int> m_vec250;           // +0x250
    std::vector<int> m_vec25c;           // +0x25c
    std::vector<STStackablePackageData> m_vec268;  // +0x268（ORIG vector<STStackablePackageData>）
    std::vector<std::vector<STStackablePackageData> > m_vec274;  // +0x274
    stStackableBooster_t m_booster;      // +0x280
    std::vector<STStackablePackageData> m_vec2a4;  // +0x2a4（ORIG vector<STStackablePackageData>）
    char m_pad2b0[0x7c];                 // +0x2b0
    char m_field32c;                     // +0x32c
    char m_pad32d[0x2b];                 // +0x32d
    int m_field358;                      // +0x358
    char m_pad35c[4];                    // +0x35c
    char m_field360;                     // +0x360
    char m_pad361[3];                    // +0x361
    // ORIG set_item 0x0850dcf4 `mov 0x364(%eax),%edx; mov %edx,0x28c(%eax)`
    // 4 字节直读 ⇒ 该字段为 int（ushort 版恒产 movzwl 双段）。pad 同步缩短保持
    // +0x380 后续偏移不变。
    int m_field364;                      // +0x364
    char m_pad368[0x18];                 // +0x368
    unsigned short m_field380;           // +0x380
    char m_pad382[2];                    // +0x382
    char m_pad384[0x10];                 // +0x384
    unsigned short m_field394;           // +0x394
    bool m_field396;                     // +0x396
    char m_pad397;                       // +0x397
    unsigned short m_field398;           // +0x398
    char m_field39a;                     // +0x39a
    bool m_field39b;                     // +0x39b
    int m_field39c;                      // +0x39c
    char m_field3a0;                     // +0x3a0
    char m_field3a1;                     // +0x3a1
    char m_pad3a2[2];                    // +0x3a2
    std::map<int, int> m_map3a4;         // +0x3a4
    char m_pad3bc[4];                    // +0x3bc
    std::vector<std::pair<int, int> > m_vec3c0;  // +0x3c0
    int m_field3cc;                      // +0x3cc
    char m_field3d0;                     // +0x3d0
    bool m_field3d1;                     // +0x3d1
    char m_pad3d2[2];                    // +0x3d2
    std::vector<std::pair<int, int> > m_vec3d4;  // +0x3d4
    int m_field3e0;                      // +0x3e0
    int m_field3e4;                      // +0x3e4
    std::vector<stLimitCubeResult> m_vec3e8;     // +0x3e8
    std::map<STEnchantSystemMapKey, STEnchantSystemMapData> m_map3f4;  // +0x3f4
    char m_field40c;                     // +0x40c
    char m_fields40d[2];                 // +0x40d
    char m_pad40f[9];                    // +0x40f
    int m_field418;                      // +0x418
    BoosterSelectInfo m_boosterSelect;   // +0x41c
    // [b28d-P248档③] ORIG set_item 0x0850e1e2 store 侧 `movzbl 0x438(%eax),%edx;
    // mov %dl,0x320(%eax)`（bool→bool 直存，无 test/setne 归一化）⇒ 本字段同型化为
    // bool，与 CStackableItem.h m_char320 bool 配对（宽度 1B、偏移/布局零变化）。
    bool m_field438;                     // +0x438
    char m_pad439[3];                    // +0x439
    int m_field43c;                      // +0x43c
    int m_field440;                      // +0x440
    int m_field444;                      // +0x444
    int m_field448;                      // +0x448
    int m_field44c;                      // +0x44c
    int m_field450;                      // +0x450
    int m_field454;                      // +0x454
    int m_field458;                      // +0x458
    int m_field45c;                      // +0x45c
    int m_field460;                      // +0x460
    int m_field464;                      // +0x464
    int m_field468;                      // +0x468
    int m_field46c;                      // +0x46c
    char m_pad470[4];                    // +0x470
    int m_field474;                      // +0x474
    int m_field478;                      // +0x478
    unsigned short m_field47c;           // +0x47c
    char m_field47e;                     // +0x47e
    char m_pad47f;                       // +0x47f
    std::set<int> m_set480;              // +0x480
    std::set<int> m_set498;              // +0x498
    bool m_field4b0;                     // +0x4b0
    char m_pad4b1[7];                    // +0x4b1
    int m_field4b8;                      // +0x4b8
    char m_pad4bc[0x30];                 // +0x4bc
    int m_field4ec;                      // +0x4ec
    char m_pad4f0[0x18];                 // +0x4f0
    int m_field508;                      // +0x508
    std::set<int> m_set50c;              // +0x50c
};

// ===================== 报告函数（CStackableItem.md） =====================

bool CStackableItem::IsAvatarOptionChangeItem() const
{
    return m_avatarOptionChange;
}

int CStackableItem::check_able_To_use(int job, int level)
{
    if (!check_job_type((char)job))
    {
        return 1;
    }
    if (!check_low_level(level))
    {
        return 2;
    }
    return 0;
}

// 【N90】差值主体=libstdc++4.1 iterator/find 非 inline sret 外联（ORIG 8 insns/步 vs 4.4 inline 4）；豁免不逐形追。
// P182/N82 封顶（b26 Lane27）：29 条指令全等，仅帧 0x28 vs 0x10（4.4 outgoing/
// slack 多 24B）连带 2 char 槽平移（-0xc/-0x10 vs -0x4/-0x8）；帧策略跨版本差。
// b28c-L8 fresh 复验：44 行口径 29=29、NDIFF=4，判词维持 N82（callee
// _ZNK5CItem14check_job_typeEc 定义在 CItem.cpp 0x0850d728 = 跨 TU ⇒ P210 定义前置不可用；
// P190 double framePad 反向撑帧亦不可（本例是 4.4 多撑，需减帧））。
// [b28d-N115/N82] fresh 29=29 维持 N：帧 0x28 vs 0x10 + 2 char 参数槽平移（-0xc/-0x10
// vs -0x4/-0x8）。根因 = N115：4.1 参数直写帧底（outgoing 仅实参 4B），4.4 -O0 恒预留
// 16B 对齐 outgoing 区（帧 +0x18）。P210 定义前置不可用（callee CItem::check_job_type
// 定义在 CItem.cpp = 跨 TU，迁定义违 R1）；P190 double 撑帧方向反（本例需减帧）。
bool CStackableItem::check_change_job_type(char changeJob,
                                           char checkJob) const
{
    if (!check_job_type(checkJob))
    {
        return 0;
    }
    if ((char)m_arad148.m_field4 < 0)
    {
        return 1;
    }
    return (char)m_arad148.m_field4 == changeJob;
}

// [b28d-solo-CStackableItem] AE 101=101。修法（新范式，P218/P310 扩展）：局部 `i`
// 声明 unsigned int + **裸比较** `while (it->m_count > i)` ⇒ 4.4 产 ORIG 形
// `mov m_count,%eax; cmp i,%eax; seta`。此前 `int i + (unsigned) 双 cast` 恒产
// `mov i,%ebx; cmp m_count,%ebx; setb`（+1 条 mov，且方向反）；反写 `m_count > i`
// 带 cast 形劣化 +2（`mov %eax,%edx; mov i,%eax; cmp`）。外层/内层 end() 按 P13 缓存。
void CStackableItem::getBoosterItem(
    std::vector<stStackableBoosterItemInfo_t>& out) const
{
    // ORIG 0x850f76e/0x850f793：外层 begin/end 在循环外各求值一次（P13）。
    std::vector<stStackableBoosterElement_t>::const_iterator it =
        m_booster.m_elements.begin();
    std::vector<stStackableBoosterElement_t>::const_iterator itEnd =
        m_booster.m_elements.end();
    for (; it != itEnd; ++it)
    {
        unsigned int i = 0;
        // ORIG 0x850f75e：m_count/m_prob 均按无符号比较（seta/setb）。
        // b28d-solo 实验：i 改 unsigned + 裸比较（ORIG seta 形）。
        while (it->m_count > i)
        {
            int rand = get_rand_int(it->m_prob);
            // ORIG 0x850f7e5/0x850f7f3：内层 begin/end 亦各求值一次（P13）。
            std::vector<stStackableBoosterItemInfo_t>::const_iterator it2 =
                it->m_items.begin();
            std::vector<stStackableBoosterItemInfo_t>::const_iterator it2End =
                it->m_items.end();
            for (; it2 != it2End; ++it2)
            {
                int idx = it2->m_itemIdx;
                if (static_cast<unsigned int>(rand)
                    < static_cast<unsigned int>(it2->m_prob))
                {
                    out.push_back(*it2);
                    break;
                }
            }
            ++i;
        }
    }
}

bool CStackableItem::getUsableItemType(int type) const
{
    std::vector<int>::const_iterator it =
        std::find(m_usableTypes.begin(), m_usableTypes.end(), type);
    if (it != m_usableTypes.end())
    {
        return 1;
    }
    return 0;
}

int CStackableItem::get_1st_material_count(unsigned int itemIdx)
{
    for (std::vector<std::pair<int, int> >::const_iterator it =
             m_limitCube.m_materials.begin();
         it != m_limitCube.m_materials.end(); it++)
    {
        if ((unsigned int)it->first == itemIdx)
        {
            return it->second;
        }
    }
    return -1;
}

// ORIG 0x822c9e0（弱符号 W，14 字节）：mov 0x29c(%eax),%eax —— 第 2 材料项索引
int CStackableItem::get_2nd_material_index()
{
    return m_limitCube.m_2ndMaterialIdx;
}

// ORIG 0x822c9ee（弱符号 W，14 字节）：mov 0x2a0(%eax),%eax —— 第 2 材料项数量
int CStackableItem::get_2nd_material_count()
{
    return m_limitCube.m_2ndMaterialCount;
}

int CStackableItem::get_amplify_reinforce_count(int count)
{
    std::map<int, int>::iterator it = m_amplifyMap.begin();
    std::map<int, int>::iterator end = m_amplifyMap.end();
    int sum = 0;
    int bestKey = 0;
    int bestCount = 0;
    while (it != end)
    {
        sum += it->second;
        if (count < sum)
        {
            return it->first;
        }
        if (bestCount < it->second)
        {
            bestCount = it->second;
            bestKey = it->first;
        }
        ++it;
    }
    return bestKey;
}

// [b28d-solo-CStackableItem] NEAR 118=118。已修：`sum` 局部 unsigned 化（新范式
// P218/P310 扩展）⇒ 比较翻正为 ORIG `mov rand,%eax; cmp sum,%eax; ja`（此前 int sum
// 恒产 `mov sum,%eax; cmp rand,%eax; jb`）。残余 N95 封顶：range 槽 ORIG -0x38
// （在 begin/end/pp 临时槽之后），4.4 恒 -0x44（临时槽之前）；把 range 声明移到循环后
// 实测反向（range -0x48/it -0x44）⇒ 槽分配方向不可择，判 N95/N93。
// ORIG 0x850fbe4 两循环复用同一 const_iterator 槽 -0x48（第二循环 `it = begin()`
// 重新赋值），循环体先 operator*（deEv）取引用地址入槽（-0x10/-0xc），增量段 ppEi。
void CStackableItem::get_limit_cube_item(std::pair<int, int>& out)
{
    unsigned long range;
    int itemIdx = 0;
    int count = 0;
    int total = 0;
    std::vector<stLimitCubeResult>::const_iterator it =
        m_limitCube.m_results.begin();
    while (it != m_limitCube.m_results.end())
    {
        const stLimitCubeResult& r = *it;
        total += r.m_prob;
        it++;
    }
    range = total;
    unsigned int rand = m_pRand->randInt(range);
    // b28d-solo 修法：sum unsigned（裸比较翻操作数序，P218/P310 扩展；见函数头判词）。
    unsigned int sum = 0;
    it = m_limitCube.m_results.begin();
    while (it != m_limitCube.m_results.end())
    {
        const stLimitCubeResult& r = *it;
        sum += r.m_prob;
        if (rand <= sum)
        {
            itemIdx = r.m_itemIdx;
            count = r.m_count;
            break;
        }
        it++;
    }
    out.first = itemIdx;
    out.second = count;
}

bool CStackableItem::is_included_material_for_limit_cube(unsigned int itemIdx)
{
    for (std::vector<std::pair<int, int> >::const_iterator it =
             m_limitCube.m_materials.begin();
         it != m_limitCube.m_materials.end(); it++)
    {
        if ((unsigned int)it->first == itemIdx)
        {
            return 1;
        }
    }
    return 0;
}

int CStackableItem::quickslot_priority()
{
    if (m_nItemType == 0 || m_nItemType == 4)
    {
        return 1;
    }
    return 0;
}

bool CStackableItem::verify()
{
    if (m_usablePeriod != 0 && getStackableLimit() != 1)
    {
        return 0;
    }
    if (m_nItemType == 0xf || m_nItemType == 0x10)
    {
        if (m_booster.m_elements.empty())
        {
            return 0;
        }
    }
    return 1;
}

bool CStackableItem::verifyUsableServerType()
{
    if (m_usableServers.empty())
    {
        return 1;
    }
    StackableAction::UsableServerType serverType =
        static_cast<StackableAction::UsableServerType>(0);
    if (((CEnvironmentView*)G_CEnvironment())->m_serverType == 0)
    {
        serverType = StackableAction::USABLE_SERVER_TYPE_GENERAL;
    }
    if (((CEnvironmentView*)G_CEnvironment())->m_serverType == 2)
    {
        serverType = StackableAction::USABLE_SERVER_TYPE_STARTER;
    }
    if (((CEnvironmentView*)G_CEnvironment())->m_serverType == 1)
    {
        serverType = StackableAction::USABLE_SERVER_TYPE_EVENT;
    }
    if (std::find(m_usableServers.begin(), m_usableServers.end(), serverType) ==
        m_usableServers.end())
    {
        return 0;
    }
    return 1;
}

// ORIG 0x0822ca58 (W)：return m_int39c == type
bool CStackableItem::verifyStackableAction(StackableAction::Type type) const
{
    // b28c-L8 判词 [N(N18a)]：ORIG 0x0822ca58 = `mov 0x39c(%eax),%eax; mov %eax,%edx;
    // mov 0xc(%ebp),%eax; cmp %eax,%edx`（字段读两段），4.4 恒折单段直落 %edx；
    // (int)/(unsigned) cast 互换与 std::underlying_type 形均不改（N18a 反向判词：
    // `mov mem,%eax; mov %eax,%reg` 单段 regmove 不可造）⇒ 残 1 行封顶。
    // [b28d-P254 翻案] m_int39c 改 StackableAction::Type（enum，CStackableItem.h）
    // 后 4.4 复现两段装载 ⇒ 本件 IDENTICAL（10=10）。旧 N18a 判词作废。
    return m_int39c == type;
}

// ===================== make_item（ORIG 0x850f8b8） =====================

void CStackableItem::make_item(Inven_Item& item) const
{
    switch (m_nItemType)
    {
    case 8:
        item.m_field1 = 4;
        break;
    case 1:
        item.m_field1 = 3;
        break;
    case 9:
    case 10:
        item.m_field1 = 7;
        break;
    case 0x14:
        item.m_field1 = 9;
        break;
    case 2:
        item.m_field1 = 10;
        break;
    default:
        item.m_field1 = 2;
        break;
    }
    item.m_fieldb = 0;
    int t = 0;
    if (getUsablePeriod() != 0)
    {
        t = ((CSystemTime*)0x941f714)->getCurSec() +
            getUsablePeriod() * 0x15180;
    }
    else if (getExpirationDate() != 0)
    {
        t = getExpirationDate();
    }
    if (t != 0)
    {
        item.m_fieldb = (unsigned short)((t - 0x44a54a80) / 0x15180);
    }
    item.ResetItemAttr();
    item.m_fieldd = 0;
    ((itemGloballyUniqueIdentifier_t*)((char*)&item + 0x15))->reset();
    item.m_amp.reset();
    if (GetAttachType() == 7)
    {
        item.SetTradeLimitCount((unsigned char)getTradeLimitCount());
    }
}

// ===================== get_lotto_item（ORIG 0x850ebfc） =====================

// [b28d-solo-CStackableItem] ORIG 0x850ebfc：pairIt 先声明（默认构造 C1Ev，P50）后
// `pairIt = begin()`；循环体 `item = std::pair<int,int>(*pairIt)` 走 aSEOS0_（P135），
// 之后一律读 item.first/second（-0x44/-0x40）；countIt 为 const_iterator 转换构造。
void CStackableItem::get_lotto_item(char* out)
{
    Inven_Item* pItem = (Inven_Item*)out;
    int itemIdx = -1;
    unsigned long range = 100000;
    int rand = m_pRand->randInt(range);
    int sum = 0;
    std::vector<std::pair<int, int> >::iterator pairIt;
    int count = 1;
    std::vector<int>::const_iterator countIt = m_lotto.m_counts.begin();
    std::pair<int, int> item;
    pairIt = m_lotto.m_items.begin();
    while (pairIt != m_lotto.m_items.end())
    {
        item = std::pair<int, int>(*pairIt);
        sum += item.second;
        if (sum > rand)
        {
            itemIdx = item.first;
            count = *countIt;
            break;
        }
        pairIt++;
        ++countIt;
    }
    if (itemIdx == 0 && count > 0)
    {
        pItem->m_addInfo = 0;
        pItem->m_addInfo2 = count;
    }
    else
    {
        if (itemIdx == -1)
        {
            itemIdx = m_lotto.m_defaultIdx;
            count = m_lotto.m_defaultCount;
            if (itemIdx == 0)
            {
                pItem->m_addInfo = itemIdx;
                pItem->m_addInfo2 = count;
                return;
            }
        }
        pItem->m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, *pItem, count);
    }
}

// ===================== verifyUsablePlace（ORIG 0x850e9f4） =====================

// [b28d-solo-CStackableItem] ORIG 0x850e9f4：place 为 UsablePlace 类型（find 实例
// `…EEEES3_ET_S9_S9_RKT0_` 的 RKT0_=UsablePlace，int 版 mangled 为 `…EEEiE`）；
// 比较为单表达式 `find(...) == m_usablePlaces.end()`（end() 求值两次：-0x18 供
// 比较、-0x10 供 find），迭代器为非 const iterator（eq 实例 `eqIPN…`）。
bool CStackableItem::verifyUsablePlace(CUser* user)
{
    if (m_usablePlaces.empty())
    {
        return 1;
    }
    StackableAction::UsablePlace place =
        static_cast<StackableAction::UsablePlace>(0);
    if (G_GameWorld()->IsPVPChannel())
    {
        place = StackableAction::USABLE_PLACE_PVP;
    }
    else
    {
        switch (user->get_state())
        {
        case 5:
            place = StackableAction::USABLE_PLACE_DUNGEON;
            break;
        case 3:
            place = StackableAction::USABLE_PLACE_VILLAGE;
            break;
        }
    }
    if (std::find(m_usablePlaces.begin(), m_usablePlaces.end(), place)
        == m_usablePlaces.end())
    {
        return 0;
    }
    if ((char)m_arad148.m_field4 < 0 ||
        ((char)m_arad148.m_field4 >= 0 &&
         ((CUserCharacInfo*)user)->getCurCharFirstGrowType() ==
             (char)m_arad148.m_field4))
    {
        return 1;
    }
    return 0;
}

// ===================== set_item（ORIG 0x850d81e） =====================

struct CItemView
{
    char m_pad[0x8c];
    int m_int8c;                 // +0x8c
    char m_pad90[0xc];           // +0x90
    // ORIG set_item 0x0850e0xx：`mov %dl,0xd0(%eax)`、`mov %edx,0xfc(%eax)`、
    // `mov %edx,0x100(%eax)` ⇒ m_charD0@0xd0、m_intFC@0xfc、m_int100@0x100。
    // 旧 pad 0x24 使三处偏移整体少 0x10（真语义 bug）。
    char m_padac[0x34];          // +0x9c..0xcf
    char m_charD0;               // +0xd0
    char m_padD1[0x2b];          // +0xd1
    int m_intFC;                 // +0xfc
    int m_int100;                // +0x100
};

// [b28d-solo-CStackableItem] fresh 复验 1060/1073（起点 1059/1100）：已修——
// ①CItemView 三处偏移整体少 0x10（m_charD0@0xc0→0xd0、m_intFC 0xec→0xfc、
//   m_int100 0xf0→0x100）真语义 bug；②删 ORIG 全函数无写入的
//   `m_boolF8 = script.m_field31`（P171）；③删 scriptText/strncpy 冗余缓冲，
//   script2Database 直传 m_strac.c_str()；④cMyTrace 改全表达式临时链（P227）；
//   ⑤镜像 STStackableScript 13 字段按 ORIG `movzbl` 单段形改型；⑥m_field364→int。
// [b28d-N] fresh 1059/1071（差 12 条，逐行 diff 323 行、槽号归一后匹配 76 行）判 N 封顶：
// 残余族 = ①槽整体深 4（ORIG -0x64/-0x258 vs 我们 -0x68/-0x25c；前置死局部 `int
// framePad` 实测反向变 -0x6c，N95 槽基不可上移）；②ORIG 把 `lea -0x64,%esi` 地址缓存进
// callee-saved 而我们直用 %edx（P183/P226 地址物化族）；③装载序/中转（N76/P211）；
// ④`m_vec94[i]` lea-vs-add（N107）；⑤比较方向 seta-vs-setb（N18b）。
// 注：函数内 `__asm__ __volatile__` rep stosl 清零块（ORIG 0x7d dwords=500B）沿用既有
// 批次做法，非 P0 register 绑定；本波未改动（如 Main 要求可换 P312 `char buf[500]={0}`
// + 前置死局部形重测）。
void CStackableItem::set_item(STStackableScript& script)
{
    char buf[500];
    unsigned long seed = time(0) + script.m_field0;
    m_pRand = new CMTRand(seed);
    m_index = script.m_field0;
    m_grade = script.m_field4;
    m_field0c = script.m_field14;
    m_field10 = script.m_field18;
    m_field14 = script.m_field1c;
    m_field30 = script.m_field24;
    m_rarity = script.m_field10;
    m_char60 = script.m_fieldfc;
    if (script.m_field30)
    {
        G_CDataManager()->set_AuctionRegFreeCeraItem(m_index);
    }
    __asm__ __volatile__(
        "movl $0, %%eax\n\t"
        "movl $0x7d, %%edx\n\t"
        "movl %0, %%edi\n\t"
        "movl %%edx, %%ecx\n\t"
        "rep stosl\n\t"
        : : "b"(buf) : "eax", "edx", "ecx", "edi", "memory");
    strcpy(buf, script.m_strac.c_str());
    if (!CodePage::script2Database((char*)script.m_strac.c_str(), buf))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x80, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script.m_strac.c_str(),
            "void CStackableItem::set_item(STStackableScript&)", 0x80);
    }
    m_str2c = buf;
    m_usablePeriod = script.m_fieldf4;
    m_expirationDate = script.m_fieldf8;
    m_field64 = script.m_field20;
    m_field68 = script.m_field4ec;
    m_int2cc = script.m_field18c;
    m_pair24.first = script.m_field28;
    m_pair24.second = script.m_field2c;
    m_field50 = script.m_fieldc;
    m_attachType = script.m_field8;
    m_nItemType = script.m_field1e0;
    m_int2b8 = script.m_field3cc;
    if (m_nItemType == 3)
    {
        createRecipeInfo(&m_recipe174, &script.m_vec88, 0);
        m_recipe174.m_vec28 = script.m_vec3c0;
        m_recipe174.additionalInfo(script);
    }
    if (m_nItemType == 6)
    {
        createLotteryInfo(&m_lotto, &script.m_vec88);
    }
    else if (m_nItemType == 7)
    {
        createStackableLotteryInfo(&m_lotto, &script.m_vec88);
    }
    else if (m_nItemType == 0xd)
    {
        createStackableLotteryInfo(&m_lotto, &script.m_vec88);
    }
    else if (m_nItemType == 0x1a || m_nItemType == 0x1b)
    {
        createStackableMultiBoxLotteryInfo(
            &m_multiBox, &script.m_vec244, &script.m_vec250,
            &script.m_vec25c);
    }
    else if (m_nItemType == 0xe || m_nItemType == 0x26)
    {
        m_pkg21c = script.m_vec268;
        m_pkg234 = script.m_vec274;
        if (!script.m_vec2a4.empty())
        {
            m_pkg228 = script.m_vec2a4;
            script.m_vec2a4.clear();
        }
    }
    else if (m_nItemType == 0xf || m_nItemType == 0x10)
    {
        m_booster = script.m_booster;
    }
    else if (m_nItemType == 0x11)
    {
        m_booster = script.m_booster;
        m_nItemType = 0x10;
    }
    else if (m_nItemType == 0x22)
    {
        m_booster = script.m_booster;
        m_boosterSelect = script.m_boosterSelect;
    }
    else if (m_nItemType == 0x17)
    {
        createGlobalEffectInfo(m_vec280, &script.m_vec88);
    }
    else if (m_nItemType == 0x1d)
    {
        m_limitCube.m_materials = script.m_vec3d4;
        m_limitCube.m_2ndMaterialIdx = script.m_field3e0;
        m_limitCube.m_2ndMaterialCount = script.m_field3e4;
        m_limitCube.m_results = script.m_vec3e8;
    }
    for (int i = 0; i <= 10; ++i)
    {
        m_jobUsable[i] = script.m_usable[i];
    }
    m_usableLevel = script.m_field5c;
    m_field4c = script.m_field60;
    m_stackable = 1;
    m_int28c = script.m_field364;
    m_int2bc = script.m_field1e4;
    m_usableTypes.clear();
    if (m_nItemType == 2 && m_int2bc == 1)
    {
        if (script.m_vec94.size() > 1)
        {
            for (int i = 1; i < script.m_vec94.size(); ++i)
            {
                int t = GetEquipmentTypeStringToEnum(script.m_vec94[i]);
                if (t != 0x16)
                {
                    m_usableTypes.push_back((int)t);
                }
            }
        }
    }
    m_int2d0 = script.m_field68;
    m_int2d4 = script.m_field1b4;
    m_option2d8.m_field0 = script.m_field1b9;
    m_option2d8.m_field1 = script.m_field1ba;
    m_option2d8.m_field2 = script.m_field1bb;
    m_stat2dc.m_flagLo = script.m_field360;
    m_stat2dc.sec = script.m_field358;
    m_stat2dc.m_flagHi = script.m_field1bf;
    m_option2e4.m_field0 = script.m_field1bc;
    m_option2e4.m_field1 = script.m_field1bd;
    m_option2e4.m_field2 = script.m_field1be;
    m_char2e7 = script.m_field1c0;
    m_int2e8 = script.m_field1c4;
    m_char2ec = script.m_field1d0;
    m_jewelTargetSocket = script.m_field380;
    m_itemGroupName = script.m_field100;
    m_char78 = script.m_field104;
    m_int58 = -1;
    m_bool79 = 0;
    m_int80 = script.m_fieldc8;
    m_int7c = script.m_fieldcc;
    m_impossibleGift = script.m_fieldd0;
    m_strb0 = script.m_str6c;
    m_drop = script.m_specialDrop;
    script.m_specialDrop.clear();
    m_char2f0 = script.m_field10e;
    m_short2ee = script.m_field394;
    m_enchantMap = script.m_map3f4;
    m_char30c = script.m_field40c;
    memcpy(m_chars30d, script.m_fields40d, 2);
    m_char30f = script.m_field396;
    m_short310 = script.m_field398;
    m_char312 = script.m_field39a;
    m_char313 = script.m_field39b;
    m_short314 = (unsigned short)script.m_field39c;
    m_char316 = script.m_field3a0;
    m_char320 = script.m_field438;
    if (m_char320)
    {
        m_int318 = script.m_field43c;
        m_int31c = script.m_field440;
    }
    m_isRoutingItem = script.m_field10f;
    m_vecC0 = script.m_vec110;
    m_questBonus = 0;
    ((CItemView*)this)->m_charD0 = script.m_field3d0;
    m_char321 = script.m_field3d1;
    m_setd4 = script.m_setdc;
    m_intEC = script.m_field64;
    m_intF0 = script.m_field444;
    m_intF4 = script.m_field448;
    ((CItemView*)this)->m_intFC = script.m_field150;
    ((CItemView*)this)->m_int100 = script.m_field154;
    m_vec104.clear();
    m_vec104 = script.m_vec158;
    m_int324 = script.m_field44c;
    m_int328 = script.m_field454;
    m_int32c = script.m_field450;
    m_int330 = script.m_field458;
    m_int334 = script.m_field45c;
    m_int338 = script.m_field460;
    m_int33c = script.m_field464;
    m_set128 = script.m_set11c;
    if (script.m_field474 != 0)
    {
        G_CDataManager()->set_lottery_use_cost(script.m_field0, script.m_field474);
    }
    m_int340 = script.m_field478;
    m_char344 = script.m_field3a1;
    m_amplifyMap = script.m_map3a4;
    m_arad148.m_field0 = script.m_field178;
    m_arad148.m_field4 = script.m_field17c;
    m_arad148.m_str8 = script.m_str180;
    m_arad148.m_strC = script.m_str184;
    m_arad148.m_str10 = script.m_str188;
    if (!script.m_vec19c.empty())
    {
        m_arad148.m_vec20.reserve(script.m_vec19c.size());
        std::copy(script.m_vec19c.begin(), script.m_vec19c.end(),
                  std::back_inserter(m_arad148.m_vec20));
    }
    if (!script.m_vec190.empty())
    {
        m_arad148.m_vec14.reserve(script.m_vec190.size());
        std::copy(script.m_vec190.begin(), script.m_vec190.end(),
                  std::back_inserter(m_arad148.m_vec14));
    }
    m_int360 = script.m_field418;
    m_char364 = script.m_field32c;
    *reinterpret_cast<unsigned short*>(&m_avatar365.m_field0) = script.m_field47c;
    m_avatar365.m_field2 = script.m_field47e;
    m_set368 = script.m_set480;
    m_set380 = script.m_set498;
    m_avatarOptionChange = script.m_field4b0;
    m_int39c = (StackableAction::Type)script.m_field1e8;  // [b28d-N254] enum 字段
    m_vec3a0 = script.m_vec1ec;
    m_usablePlaces = script.m_vec1f8;
    if (m_int39c == 0x2c)
    {
        for (unsigned int i = 0; i < m_vec3a0.size(); ++i)
        {
            void* emo = ((CChattingEmoticonList*)
                ((CDataManagerView*)G_CDataManager())->m_field28)
                ->find_emoticon(m_vec3a0[i]);
            typedef void (*EmoticonFn)(void*, int);
            EmoticonFn fn = *(EmoticonFn*)(*(char**)emo + 0x18);
            fn(emo, m_index);
        }
    }
    m_usableServers = script.m_vec214;
    m_char3c5 = script.m_field205;
    m_impossibleCharacCargo = script.m_fieldda;
    ((CItemView*)this)->m_int8c = script.m_field220;
    m_vec90 = script.m_vec224;
    m_dyeInfo9c = script.m_dye164;
    m_randomOptionUnseal = script.m_field4b8 != 0;
    m_char144 = script.m_fieldd8;
    m_parentBoosterItemId = 0;
    m_int3d0 = script.m_field468;
    m_int3c8 = script.m_field46c;
    if (m_int3c8 > 1)
    {
        m_booster = script.m_booster;
    }
    m_int3cc = script.m_field1ac;
    m_tradeLimitCount = script.m_field508;
    m_set3d8 = script.m_set50c;
}

// ===================== get_multibox_default_item（ORIG 0x850f136） =====================

// [b28d-N18b] 判 N 封顶（fresh 153/151）。唯一残余族（语义等价、跨版本 codegen）：
// `sum += (*it).m_prob`：ORIG 4.1 走 `mov 0x8(%eax),%edx; mov sum,%eax;
// lea (%edx,%eax,1),%eax; mov %eax,sum` 四段；4.4 恒折 `mov mem,%eax; add %eax,sum`
// （-2 条）。实测反劣/无效：`sum = sum + prob`、`sum = prob + sum`、sum→unsigned
// （unsigned 还会把比较翻成无符号 jae，与 ORIG 有符号 jle 反，已回退）。
// 本件 `flag = stDefaultItemInfo::m_defaultFlag != 0` 的 test/setne 物化已匹配
// （该字段保持 char，见 get_multibox_lottery_item 判词的字段分型说明）。
// ORIG 0x850f136：rand/sum/itemIdx/count/flag 声明在函数头（-0x20/-0x1c/-0x18/-0x14/-0xd），
// 循环体内先 `it = begin()`（-0x2c）再 get_rand_int(1000000)；内层 while 每次调 end()、
// 增量 `it++`（ppEi）。
void CStackableItem::get_multibox_default_item(
    std::vector<std::pair<Inven_Item, bool> >* out)
{
    if (out == 0)
    {
        return;
    }
    int rand = 0;
    // [b28d-solo-CStackableItem] ORIG 0x0850f1bd 累加段 = 读-加-写四段
    // `mov 0x8(%eax),%edx; mov -0x1c(%ebp),%eax; lea (%edx,%eax,1),%eax;
    // mov %eax,-0x1c(%ebp)`；4.4 对「int 字段 + int 累加器」恒折
    // `mov mem,%eax; add %eax,sum`（N18b）。volatile 化实测槽漂移(-0x1c→-0x28)+
    // 比较物化(setg/test/je)，已回退。唯一复现形 = 字段侧 unsigned 化（异型 ⇒ 4.4
    // 不折叠；探针 /tmp/probe_sum2.cpp a1 逐条同形）⇒ 走 R2 报 Main 代落
    // MultiBoxLotteryInfo.h:33 stDefaultItem::m_prob int→unsigned int。
    int sum = 0;
    int itemIdx = 0;
    int count = 0;
    bool flag = false;
    for (unsigned int i = 0;
         i < (unsigned int)m_multiBox.m_default.m_defaultCount; i++)
    {
        std::vector<MultiBoxLotteryInfo::stDefaultItem>::iterator it =
            m_multiBox.m_default.m_items.begin();
        rand = get_rand_int(1000000);
        sum = 0;
        itemIdx = 0;
        while (it != m_multiBox.m_default.m_items.end())
        {
            sum += (*it).m_prob;
            if (sum > rand)
            {
                itemIdx = (*it).m_itemIdx;
                count = (*it).m_count;
                flag = (*it).m_flag != 0;
                break;
            }
            it++;
        }
        if (itemIdx == 0)
        {
            itemIdx = m_multiBox.m_default.m_defaultIdx;
            count = m_multiBox.m_default.m_defaultNum;
            flag = m_multiBox.m_default.m_defaultFlag != 0;
        }
        Inven_Item item;
        item.m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, item, count);
        std::pair<Inven_Item, bool> p;
        p.first = item;
        p.second = flag;
        out->push_back(p);
    }
}

// ===================== get_multibox_default_item_arad（ORIG 0x850f38a） =====================

// [b28d-solo-CStackableItem] ORIG 0x850f38a：**static 成员**（首参 0x8(%ebp)、无 this，
// P245/P281）；静态表 push_back 走右值重载 push_backEOS1_（临时对象直传，P270）；
// 选择循环 rand<=sum、operator*、it++（ppEi），pair.second 直写 0。
void CStackableItem::get_multibox_default_item_arad(
    std::vector<std::pair<Inven_Item, bool> >* out)
{
    if (out == 0)
    {
        return;
    }
    static std::vector<MultiBoxLotteryInfo::LotteryDefaultArad>
        defaultItemListArad;
    if (defaultItemListArad.empty())
    {
        defaultItemListArad.push_back(
            MultiBoxLotteryInfo::LotteryDefaultArad(0x28be16, 10, 50000));
        defaultItemListArad.push_back(
            MultiBoxLotteryInfo::LotteryDefaultArad(0x28be16, 0x32, 25000));
        defaultItemListArad.push_back(
            MultiBoxLotteryInfo::LotteryDefaultArad(0x28be16, 100, 5000));
        defaultItemListArad.push_back(
            MultiBoxLotteryInfo::LotteryDefaultArad(0x28d150, 1, 10000));
    }
    int rand = 0;
    int sum = 0;
    int itemIdx = 0;
    int count = 0;
    unsigned int i = 0;
    if (i < defaultItemListArad.size())
    {
        rand = get_rand_int(100000);
        sum = 0;
        itemIdx = 0;
        for (std::vector<MultiBoxLotteryInfo::LotteryDefaultArad>::iterator
                 it = defaultItemListArad.begin();
             it != defaultItemListArad.end(); it++)
        {
            sum += (*it).m_field8;
            if (sum >= rand)
            {
                itemIdx = (*it).m_field0;
                count = (*it).m_field4;
                break;
            }
        }
        Inven_Item item;
        item.m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, item, count);
        std::pair<Inven_Item, bool> p;
        p.first = item;
        p.second = false;
        out->push_back(p);
    }
}

// ===================== get_multibox_lottery_item（ORIG 0x850edda） =====================

// [b28d-solo-CStackableItem] AE 214=214。修法（P300/P248 档②）：本函数 `flag` 局部
// char→bool + **LotteryUnit::m_defaultFlag** 字段 char→bool（MultiBoxLotteryInfo.h，
// 已 R2 广播）⇒ `flag = unit.m_defaultFlag;` 与 `p.second = flag;` 均产 ORIG 直存
// `movzbl -0x5c; mov %al`（此前 char 版 4.4 归一化 `cmpb/setne` 多 1 条）。
// 注意**同名字段分属两类、真型相反**：stDefaultItemInfo::m_defaultFlag 保持 char
// （其消费点 get_multibox_default_item / createStackableMultiBoxLotteryInfo 要求
// char 形）——字段级分型后三件同时满足（见各自判词）。
// ORIG 0x850edda：rand/sum/itemIdx/count/flag 声明在函数头（-0x24/-0x20/-0x1c/-0x18/-0xd）；
// 外层 while 每次调 end()、增量 unitIt++（ppEi）；unit 拷贝构造落 -0x68；内层双迭代器
// itemIt(-0x40)/bonusIt(-0x44) 均 ppEi，字段访问走 operator*（deEv）。
void CStackableItem::get_multibox_lottery_item(
    std::vector<std::pair<Inven_Item, bool> >* out)
{
    if (out == 0)
    {
        return;
    }
    int rand = 0;
    int sum = 0;
    int itemIdx = 0;
    int count = 0;
    // b28d-solo 修法：flag char→bool（P300/P248 档②，配 m_defaultFlag bool 化 ⇒ 直存）。
    bool flag = 0;
    std::vector<MultiBoxLotteryInfo::LotteryUnit>::iterator unitIt =
        m_multiBox.m_units.begin();
    while (unitIt != m_multiBox.m_units.end())
    {
        MultiBoxLotteryInfo::LotteryUnit unit(*unitIt);
        for (int i = 0; i < unit.m_count; i++)
        {
            std::vector<std::pair<int, int> >::iterator it =
                unit.m_items.begin();
            std::vector<std::pair<int, int> >::iterator it2 =
                unit.m_bonus.begin();
            rand = get_rand_int(1000000);
            sum = 0;
            itemIdx = 0;
            while (it != unit.m_items.end())
            {
                sum += (*it).second;
                if (sum > rand)
                {
                    itemIdx = (*it).first;
                    count = (*it2).first;
                    flag = (*it2).second != 0;
                    break;
                }
                it++;
                it2++;
            }
            if (itemIdx == 0)
            {
                itemIdx = unit.m_defaultIdx;
                count = unit.m_defaultNum;
                flag = unit.m_defaultFlag;
            }
            Inven_Item item;
            item.m_addInfo = itemIdx;
            G_CDataManager()->m_itemList->create_item(
                (ENUM_ITEM_CREATE_TYPE)2, item, count);
            std::pair<Inven_Item, bool> p;
            p.first = item;
            p.second = flag;
            out->push_back(p);
        }
        unitIt++;
    }
}

// ===================== 简单虚函数 / 访问器（ORIG weak） =====================

int CStackableItem::GetItemType() const
{
    return m_nItemType;
}

bool CStackableItem::IsAvatarItem() const
{
    return 0;
}

bool CStackableItem::IsCreatureItem() const
{
    if (m_nItemType > 8 && m_nItemType <= 0xa)
    {
        return 1;
    }
    return 0;
}

bool CStackableItem::IsClearAvatar() const
{
    return 0;
}

bool CStackableItem::IsEnchantalbe() const
{
    return 0;
}

bool CStackableItem::isEnableAvatarFunc(ENUM_AVATAR_FUNC_FILTER filter) const
{
    return 0;
}

int CStackableItem::GetGuildPowerWarPoint() const
{
    return m_int80;
}

int CStackableItem::GetUserPowerWarPoint() const
{
    return m_int7c;
}

bool CStackableItem::IsImpossibleGift() const
{
    return m_impossibleGift;
}

bool CStackableItem::IsRandomOptionUnseal() const
{
    return m_randomOptionUnseal;
}

void CStackableItem::set_parent_booster_item_id(unsigned long id)
{
    m_parentBoosterItemId = id;
}

unsigned long CStackableItem::get_parent_booster_item_id() const
{
    return m_parentBoosterItemId;
}

int CStackableItem::getTradeLimitCount() const
{
    return m_tradeLimitCount;
}

// ===================== 简单访问器 / 状态（ORIG weak） =====================

// ORIG 0x822c9fc：返回 this+0x28c
int CStackableItem::getStackableLimit() const
{
    return m_int28c;
}

// ORIG 0x822ca0a：返回 this+0x280（引用→拷贝构造源）
const std::vector<stGlobalEffect_t>& CStackableItem::getGlobalEffectData() const
{
    return m_vec280;
}

// ORIG 0x8694736：movzbl 0x2e7(%eax); ret —— 直接返回 bool 字段（无 test/setne 归一）。
bool CStackableItem::IsExpAffect()
{
    return m_char2e7;
}

// +0x2d8 为 CountinuousItemOption 子对象（m_option2d8：3 独立字节）。
CStackableItem::CoolTimeOption CStackableItem::GetCoolTimeOption()
{
    CoolTimeOption opt;
    // b28c-L8：ORIG 0x086946d2 = `movzwl 0x2d8(%edx),%ecx; mov %cx,(%eax)` 16 位
    // 直读（uchar 视图实测反劣 +1：4.4 对 uchar→ushort 追加 movzbl %dl 归一）。
    // 残差 1 条 = 4.4 第二读复用 %edx 覆盖基址 ⇒ 需重载 0xc(%ebp)，而 4.1 用
    // %ecx/%edx 分列（N7/N18a 型寄存器分配固有差，实测两形均不可翻）。
    // [b28d-solo-N7] fresh 10/11 判 N7。本波补测：聚合初始化形 10/13 反劣
    // （先 movw/movb 清零再赋值）已回退；类型旋钮无可用（sec/flag 均 1/2 字节 packed，
    // 改型即改 sizeof=3；返回类型 CoolTimeOption 固定 sret）。
    opt.sec  = m_option2d8.m_sec16;
    opt.flag = m_option2d8.m_field2;
    return opt;
}

// ORIG 0x086946f4 (W 非 const)：读 +0x2e4(+0x2e6)，effect 时长(秒)/标志
// +0x2e4 为 CountinuousItemOption 子对象（m_option2e4：3 独立字节）。
// 【N90】差值主体=libstdc++4.1 iterator/find 非 inline sret 外联（ORIG 8 insns/步 vs 4.4 inline 4）；豁免不逐形追。
CStackableItem::CoolTimeOption CStackableItem::GetEffectItemOption()
{
    CoolTimeOption opt;
    opt.sec  = m_option2e4.m_sec16;    // b28c-L8：同 GetCoolTimeOption（ORIG 0x086946f4）
                                       // [b28d-solo-N7] fresh 10/11 维持 N7（同族同判词）。
    opt.flag = m_option2e4.m_field2;
    return opt;
}

// ORIG 0x08694716 (W 非 const)：无 ctor 调用，直接 8 字节拷贝
//   mov 0x2e0(%eax),%edx; mov 0x2dc(%eax),%eax; mov %eax,(%ecx); mov %edx,0x4(%ecx)
// +0x2dc 为 StatChangeInfo 子对象（m_stat2dc：int + 低两字节 flag）。
// 调用方(_processEffectItem)按 ORIG 只测 flag 的低 16 位。
StatChangeInfo CStackableItem::GetStatChangeInfo()
{
    // 直接 reinterpret 拷贝：不调用 StatChangeInfo ctor，生成 ORIG 的 8 字节整拷。
    return *reinterpret_cast<StatChangeInfo*>(&m_stat2dc);
}

// ORIG 0x08694746 (W 非 const)：flds +0x2e8 —— 返回经验加成比例(float)
float CStackableItem::GetExpAffectRatio()
{
    // b28c-L8：ORIG 0x08694746 = `mov 0x8(%ebp),%eax; add $0x2e8,%eax; flds (%eax)`
    // （指针经 add 推进后 flds 单元址）。旧形 `*(float*)&m_int2e8` 4.4 会折成
    // `flds 0x2e8(%eax)`；改经匿名 union float 视图 + 返回类型与字段同型即复现。
    return m_flt2e8;
}

// ORIG 0x822ca28：movzwl 0x2b4(%eax) —— 返回珍宝目标槽位（16 位，单次零扩展）
unsigned short CStackableItem::getJewelTargetSocket() const
{
    return m_jewelTargetSocket;
}

// ORIG W 0x822c9d2 _ZNK14CStackableItem12get_sub_typeEv：
// mov 0x2bc(%eax) —— 返回 m_int2bc（sub_type）
int CStackableItem::get_sub_type() const
{
    return m_int2bc;
}

// ORIG 0x0869475e W _ZNK14CStackableItem26IsUse2ndUpgradeTableRecipeEv：
// movzbl 0xd0(%eax) —— 返回 CItem 基类 +0xd0 字节（m_boolD0）
bool CStackableItem::IsUse2ndUpgradeTableRecipe() const
{
    return m_boolD0;
}

// ORIG 0x822ca48 (W)：movzbl 0x321(%eax),%eax —— 直接返回 bool 字段（无 test/setne）。
bool CStackableItem::IsDeletableItem() const
{
    return m_char321;
}

// ORIG _ZN14CStackableItem18IsSharedEffectItemEv W 0x0822ca18：movzbl 0x2ec(%eax),%eax —— 返回 m_char2ec 字节（共享特效物品标记）
char CStackableItem::IsSharedEffectItem()
{
    return m_char2ec;
}

// ORIG _ZN14CStackableItem13GetEmancipateEv W 0x0854b4e8：movzwl 0x2ee(%eax),%eax —— 返回 m_short2ee（解绑值）
unsigned short CStackableItem::GetEmancipate()
{
    return m_short2ee;
}

// ============================================================================
// CStackableItem::is_random_amplify_reinforce_ticket @ 0x854b518
//   返回 +0x344 bool（movzbl 0x344(%eax); ret，无 test/setne 归一）。
// ============================================================================
bool CStackableItem::is_random_amplify_reinforce_ticket()
{
    return m_char344;
}

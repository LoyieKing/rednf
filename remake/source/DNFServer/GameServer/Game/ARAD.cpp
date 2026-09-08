// df_game_r Game/ ARAD 命名空间类实现（批3：从 GameStubs.cpp 迁移，批4：Arad 脚本生命周期）。
// 依据 docs/class_func_reports/ARAD__Arad_DataManager/* + 各子对象报告 + ORIG elf 反汇编。
#include "AradServerStateMessage.h"
#include "Arad_EventPeriodDataManager.h"
#include "Arad_StatisticsEventItemList.h"
#include "AvatarFixedHiddenOptionServer.h"  // #1 注册项（本 TU 新建）
#include "AvatarRechargeServer.h"            // #2 注册项
#include "EmblemCompoundServer.h"            // #3 注册项（FixAradEmblemConvert 新建 TU）
#include "AvatarConvertServer.h"             // #4 注册项（FixAradEmblemConvert 新建 TU）
#include "LevelupSupportEventManger.h"      // #6 注册项
#include "CreateDnfEventSciprt.h"           // #8 注册项
#include "ARAD_FN_DeleteGameScript.h"   // FN_DeleteGameScript 唯一类声明（Arad_DataManager.h 不含）
#include "Arad_Script.h"
#include "CUser.h"          // CerashopAddRestrict::Manager（Init 注册）
#include "PacketGuard.h"    // notifyOpenMessageDialog 组包
#include "LogManager.h"
#include "DNFLexWrapper.h"  // parsing：limited_map 段 Load(g_dnfLex.getInputStreamBuffer())
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace ARAD
{

// ============================================================================
// ScriptInterface（虚基接口）
// （ORIG nm 无 ScriptInterface 析构符号：析构隐式平凡，不提供定义——
//  见 Arad_DataManager.h ScriptInterface 注释。）

// ============================================================================
// ScriptManager（ORIG LoadScript 0x81a11dc / DestroyScript 0x81a12f8 /
// RegistScript 0x81a136c / Init 0x81a13f6）
// ============================================================================
// b28d-solo-ARAD verdict=IDENTICAL/AE：P13 end 缓存 + P221 全表达式临时链
//（`cMyTrace(...)("Fail"/"Success")`）——具名 cMyTrace 时 4.4 把聚合对象排到低槽
//（fail -0x30/success -0x20、标量 it -0xc/e -0x10），临时链后槽位与 ORIG 逐一对齐
//（it -0x2c / e -0x30 / fail -0x28 / success -0x18）。残余 diff 全为 .o 未 relink
// 的 call/jmp 与 .rodata 重定位显示（§6.3 假阳性）。
bool ScriptManager::LoadScript()
{
    // ORIG 0x81a11dc：Init() 后遍历 set 调用各脚本 Load()；全成功返回 1，
    // 否则 cMyTrace "Fail" 返回 0；每次成功也记 "Success"。
    Init();
    // b28d-60：P13 end 缓存（ORIG end() 在循环外一次 → -0x30 槽）；
    // 成功路径 cMyTrace 第4/5参 ORIG 为 (0,1) ⇒ 源码 (false, true)（原 true,false 反）。
    for (std::set<ScriptInterface*>::const_iterator it = m_scripts.begin(),
                                                     e = m_scripts.end();
         it != e; ++it)
    {
        if (!(*it)->Load())
        {
            // ORIG 0x81a1276：失败记 cMyTrace(pretty, 0x12, 9, true, true) + "Fail"。
            // b28d-solo-ARAD：P221 全表达式临时链（槽序试探）。
            cMyTrace(__PRETTY_FUNCTION__, 0x12, 9, true, true)("Fail");
            return false;
        }
        // ORIG 0x81a12f4：每次成功记 cMyTrace(pretty, 0x15, 9, true, false) + "Success"。
        cMyTrace(__PRETTY_FUNCTION__, 0x15, 9, false, true)("Success");
    }
    return true;
}

bool ScriptManager::DestroyScript()
{
    // ORIG 0x81a12f8：遍历 set 逐个 Unload()，返回 1。
    // ORIG end() 一次求值（P13 缓存）；it 槽浅 -0xc / e 槽深 -0x10 ⇒
    // it 先声明（P212 头部直构试探）。
    std::set<ScriptInterface*>::const_iterator it = m_scripts.begin();
    std::set<ScriptInterface*>::const_iterator e = m_scripts.end();
    for (; it != e; ++it)
        (*it)->Destroy();
    return true;
}

void ScriptManager::RegistScript(ScriptInterface* s, int priority)
{
    // ORIG 0x81a136c：find 已存在（重复注册）→
    // printf("Arad_ScriptManager::RegistScript() Duplicate Class : Line(%d)\n", priority) + exit(0)；
    // 不存在 → insert，返回 true。
    // b28d-60：NEAR 残差=insert 后 `leave` 前一条对齐 nop（ORIG 无）——if/else
    // 双块形替代 guard-return 试探（消 4.4 的块对齐 nop）。
    if (m_scripts.find(s) == m_scripts.end())
    {
        m_scripts.insert(s);
    }
    else
    {
        printf("Arad_ScriptManager::RegistScript() Duplicate Class : Line(%d)", priority);
        exit(0);
    }
}

void ScriptManager::Init()
{
    // ORIG 0x81a13f6（59 指令）：按序 RegistScript(Singleton<X>::Get(), 0x13) 共 9 个：
    //   AvatarFixedHiddenOptionServer → AvatarRechargeServer → EmblemCompoundServer →
    //   AvatarConvertServer → CerashopAddRestrict::Manager → LevelupSupportEventManger →
    //   Arad_DataManager → CreateDnfEventSciprtServer → LevelupSupport2ndEventManger。
    // 当前依赖闭包完整的六个（保持 ORIG 相对次序；#3/#4 由 FixAradEmblemConvert 落地）：
    RegistScript(Singleton<AvatarFixedHiddenOptionServer>::Get(), 0x13);
    RegistScript(Singleton<AvatarRechargeServer>::Get(), 0x13);
    RegistScript(Singleton<EmblemCompoundServer>::Get(), 0x13);   // #3（ORIG 次序）
    RegistScript(Singleton<AvatarConvertServer>::Get(), 0x13);    // #4（ORIG 次序）
    RegistScript(Singleton<CerashopAddRestrict::Manager>::Get(), 0x13);
    RegistScript(Singleton<LevelupSupportEventManger>::Get(), 0x13);    // #6（ORIG 次序）
    RegistScript(Singleton<Arad_DataManager>::Get(), 0x13);
    RegistScript(Singleton<CreateDnfEventSciprtServer>::Get(), 0x13);   // #8（ORIG 次序）
    RegistScript(Singleton<LevelupSupport2ndEventManger>::Get(), 0x13); // #9（ORIG 次序）
}

Arad_InterfaceGameScript::Arad_InterfaceGameScript(std::string name)
    : m_scriptDataName(name)
{
}

Arad_InterfaceGameScript::~Arad_InterfaceGameScript() {}

std::string Arad_InterfaceGameScript::scriptDataName() const
{
    // ORIG 0x8188b18：return m_scriptDataName
    return m_scriptDataName;
}

}  // namespace ARAD

// ============================================================================
// 6 个脚本子对象位于【全局命名空间】（ORIG mangled _ZN13ExceptionItem... 等，
// 无 ARAD:: 前缀）——见 Arad_DataManager.h 说明。
// ============================================================================
// Arad_ScriptLoader（open/parsing 见 ScriptExtras.cpp）
// ============================================================================

// ============================================================================
// ExceptionItem（+0x1c）—— ORIG ctor 0x8188b50 / dtor 0x8188b7a / parsing 0x81a1fce /
// find_index 0x81a210c。解析 "material_exception_item" 节，行读 material_exception_item_index。
// ============================================================================
ExceptionItem::ExceptionItem() {}
ExceptionItem::~ExceptionItem() {}

// b28d-solo-ARAD verdict=IDENTICAL/AE：P283 goto 尾标签复现 4.1「begin 后 jmp 到底部
// 条件块」；具名 `int idx` 由块内提至函数级 + 具名 insert 结果 `res`（P212/P247 槽序
// 旋钮）后槽位与 ORIG 全对齐（idx -0x20 / insert sret -0x28 / .second -0x24）。
int ExceptionItem::parsing(char* data)
{
    Arad_Script script;
    if (!script.Load(data, "material_exception_item"))
        return 0x27;
    if (script.GetRowNum() == 0)
        return 0x2a;
    // ORIG 循环每轮重调 GetRowNum
    // b28d-60：P283 4.1「begin 后无条件 jmp 到底部条件块」布局——原 while(true)+
    // break 使 4.4 条件置顶（setle/jne end）；改 goto 尾标签复现 setg/jne body。
    int r = 0;
    int idx;   // b28d-solo-ARAD：P212 全头部声明（槽序试探）
    goto check;
loop_body:
    {
        idx = script.GetInt(r, "material_exception_item_index");
        // b28d-solo-ARAD：具名 sret 槽试探（ORIG insert sret 在 -0x28、.second -0x24）。
        std::pair<std::map<int, int>::iterator, bool> res =
            m_exceptionItemIndex.insert(std::make_pair(idx, idx));
        if (!res.second)
            return 0x33;   // ORIG insert 失败（.second==0）→ 0x33
        ++r;
    }
check:
    if (script.GetRowNum() > r)
        goto loop_body;
    return 0;
}

bool ExceptionItem::find_index(int index)
{
    // ORIG 0x81a210c：find 先于 end（求值序），且尾为 test/je/mov$1/jmp/mov$0
    // 分支归一化（P56）——具名迭代器 + if(cond)return true 复现。
    std::map<int, int>::iterator it = m_exceptionItemIndex.find(index);
    if (it != m_exceptionItemIndex.end())
        return true;
    return false;
}

// ============================================================================
// PurchaseLimitItem（+0x38）—— ORIG ctor 0x8188bae / dtor 0x8188bd8 / parsing 0x81a216e /
// find 0x81a2310。解析 "purchase_limit_item" 节，读 Item_Index/purchase_Type/puchase_limit_count。
// ============================================================================
PurchaseLimitItem::PurchaseLimitItem() {}
PurchaseLimitItem::~PurchaseLimitItem() {}

// b28d-solo-ARAD verdict=IDENTICAL/AE：Info 提到循环外构造一次（ORIG 0x81a21dc 在
// r=0 之前）+ Item_Index 写入 Info+0x04（make_pair first = `lea -0x2c; add $0x4`，
// 即 info.reserved 槽）+ 具名 insert 结果 res ⇒ 槽位全对齐（script -0x88 / Info -0x2c /
// insert sret -0x34 / .second -0x30）。残余仅 4.4 循环对齐 nop（§6.1 豁免）。
int PurchaseLimitItem::parsing(char* data)
{
    Arad_Script script;
    if (!script.Load(data, "purchase_limit_item"))
        return 0x4c;                  // ORIG 0x81a21ab：Load 失败返 0x4c
    if (script.GetRowNum() == 0)
        return 0x4f;                  // ORIG 0x81a21cc：无行返 0x4f
    // b28d-solo-ARAD：ORIG 0x81a21dc 在 r=0 之前**循环外**构造一次 Info（-0x2c），
    // 循环内复用 ⇒ Info 声明必须在函数级（原循环体内声明使 script 槽 -0x8c、Info -0x34）。
    Info info;
    int r = 0;
    goto check;
loop_body:
    {
        // b28d-solo-ARAD：ORIG 0x81a220a 把 Item_Index 写入 Info+0x04（reserved 槽，
        // make_pair 的 first 即 `lea -0x2c; add $0x4`），非独立 idx 局部。
        info.reserved = (unsigned int)script.GetInt(r, "Item_Index");
        info.purchaseType = (unsigned short)script.GetInt(r, "purchase_Type");
        info.limitCount = (unsigned short)script.GetInt(r, "puchase_limit_count");
        // ORIG 0x81a2297：insert 而非 operator[]（重复 key 保留首条），
        // 插入失败（已存在）返 0x5c。
        // b28d-solo-ARAD：具名 sret（同 ExceptionItem）令 4.4 把 insert 返回槽排到
        // ORIG 的 -0x34（.second -0x30），Info 上移到 -0x2c。
        std::pair<std::map<unsigned int, Info>::iterator, bool> res =
            m_purchaseLimitMap.insert(std::make_pair(info.reserved, info));
        if (!res.second)
            return 0x5c;
        ++r;
    }
check:
    if (script.GetRowNum() > r)
        goto loop_body;
    return 0;
}

bool PurchaseLimitItem::find(unsigned int itemIndex, Info& out)
{
    // ORIG 0x81a2310：map<uint,Info> 命中写 out 返回 true。
    // P96：4.4 的 const_iterator 与 iterator 是不同类型，find 返回值赋 const_iterator
    // 会多一次转换构造（ORIG 无该 call）；ORIG 侧 cv 限定为非 const iterator。
    std::map<unsigned int, Info>::iterator it = m_purchaseLimitMap.find(itemIndex);
    // P222：ORIG 0x81a235f 为正向包裹（`test; je <mov $0 失败岛>; 命中体; mov $1`），
    // 否定早返 `if (it == end) return false;` 产反向块序。
    if (it != m_purchaseLimitMap.end())
    {
        out = it->second;
        return true;
    }
    return false;
}

// ============================================================================
// AradBonusItem（+0x54）—— ORIG ctor 0x8188c0c / dtor 0x8188c36 / parsing 0x81a238a /
// find 0x81a252c。解析 "bonus_item" 节，读 item_id/bonus_type/bonus_value。
// ============================================================================
AradBonusItem::AradBonusItem() {}
AradBonusItem::~AradBonusItem() {}

// b28d-solo-ARAD verdict=IDENTICAL/AE：ORIG 0x81a23f8 在 r=0 之前**循环外**构造
// 一次 Data（-0x38），循环内复用；make_pair 实例 `_ZSt9make_pairIRjRN13AradBonusItem4DataEE`
// 的 first = &Data+0x04（即 unsigned int itemId 字段），无独立 itemIdx 局部
//（ORIG 0x81a2426 `call GetInt; mov %eax,-0x34` 直存字段）。
int AradBonusItem::parsing(char* data)
{
    Arad_Script script;
    if (!script.Load(data, "bonus_item"))
        return 0x79;
    if (script.GetRowNum() == 0)
        return 0x7c;
    // b28d-solo-ARAD：Data 循环外构造（同 PurchaseLimitItem::Info）。
    Data d;
    int r = 0;
    goto check;
loop_body:
    {
        // ORIG 0x81a2426/0x81a2446/0x81a2467：item_id→Data+4、bonus_type→Data+8、
        // bonus_value→Data+0（GetInt 结果直存，无中间局部）。
        d.itemId = (unsigned int)script.GetInt(r, "item_id");
        d.bonusType = (unsigned short)script.GetInt(r, "bonus_type");
        d.bonusValue = script.GetInt(r, "bonus_value");
        // b28d-solo-ARAD：具名 sret 令 insert 返回槽落 ORIG 的 -0x40（.second -0x3c），
        // Data 上移到 -0x38。
        std::pair<std::map<int, Data>::iterator, bool> res =
            m_bonusMap.insert(std::make_pair(d.itemId, d));
        if (!res.second)
            return 0x88;   // ORIG insert 失败（.second==0）→ 0x88
        ++r;
    }
check:
    if (script.GetRowNum() > r)
        goto loop_body;
    return 0;
}

// b28d-solo-ARAD verdict=IDENTICAL/AE：P96/P270 非 const iterator（消 const_iterator
// 转换构造 call）+ P0 具名 key（ORIG 0x81a2535 `mov %eax,-0x1c`）+ P247 具名 Data tmp
// 后结构整体赋值（ORIG 0x81a2590 逐字段读节点到 -0x14/-0x10/-0xc，再 memcpy(out,&tmp,10)
// 带自赋值检查）。槽位 key -0x1c / find -0x20 / end -0x18 / tmp -0x14 全对齐。
bool AradBonusItem::find(unsigned int itemId, Data& out)
{
    // ORIG 0x81a252c：map<int,Data> 命中写 out（拷 10 字节）返回 true。
    int key = itemId;   // ORIG 具名 key 槽（-0x1c）
    std::map<int, Data>::iterator it = m_bonusMap.find(key);
    if (it != m_bonusMap.end())
    {
        out = Data(it->second);
        return true;
    }
    return false;
}

// ============================================================================
// Arad_Mileage_Data（+0x70）—— ORIG ctor 0x8188cc0 / dtor 0x8188cea / parsing 0x81a3416 /
// find 0x81a3668 / default_value 0x81a36f0。解析 "default"（percent）与 "item"（ipg_no/percent）。
// ============================================================================
// [b22] ORIG ctor 0x08188cc0 不写 +4（m_defaultPercent）：仅基类 ctor+vptr+vector ctor，
// 16→14 条。默认值清零发生在 parsing()（`m_defaultPercent = 0` 行），ctor 侧删初始化。
Arad_Mileage_Data::Arad_Mileage_Data() {}
Arad_Mileage_Data::~Arad_Mileage_Data() {}

// b28d-solo-ARAD verdict=IDENTICAL/AE：补具名 `int found = find(ipg)`（P0 槽往返，
// ORIG 0x81a3598 `mov %eax,-0xc`）后全帧槽位对齐（script -0x74 / r -0x18 / ipg -0x14 /
// percent -0x10 / found -0xc / Data -0x20），154=154 指令全等；残余 diff 全为 .o
// 未 relink 的 call/jmp 与 .rodata 重定位显示（§6.3 假阳性）。
int Arad_Mileage_Data::parsing(char* data)
{
    Arad_Script script;
    if (!script.Load(data, "default"))
        return 0x18c;
    if (script.GetRowNum() == 0)
        return 0x18f;

    m_defaultPercent = 0;
    m_defaultPercent = script.GetInt(0, "percent");
    if (m_defaultPercent < 0 || m_defaultPercent > 0x3e8)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Script_Loader.cpp",
            "virtual int Arad_Mileage_Data::parsing(char*)",
            0x197, "[ERROR] invalid mileage default value. (%f)", m_defaultPercent);
        return 0x198;
    }

    if (!script.Load(data, "item"))
        return 0x19e;
    if (script.GetRowNum() == 0)
        return 0;

    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        int ipg = script.GetInt(r, "ipg_no");
        int percent = script.GetInt(r, "percent");
        // b28d-solo-ARAD：P0——ORIG 0x81a3598 `call find; mov %eax,-0xc(%ebp)` 有具名
        // 结果槽（-0xc），判断为 `cmpl $0x0,-0xc; jns <错误块>`；内联 find() 被 4.4
        // 折成 `shr $0x1f,%eax` 且整帧浅 4（script -0x70 vs ORIG -0x74）。
        int found = find(ipg);
        if (found < 0)
        {
            Data d(ipg, percent);
            m_mileageList.push_back(d);
        }
        else
        {
            LogManager::logFormat(
                1, "localjapan/Arad_Script_Loader.cpp",
                "virtual int Arad_Mileage_Data::parsing(char*)",
                0x1b0, "[ERROR] already exist item index (IPG:%d, percent:%d)",
                ipg, percent);
            return 0x1b1;
        }
    }
    return 0;
}

int Arad_Mileage_Data::find(int ipgNo)
{
    // ORIG 0x81a3668：按 ipgNo 找 Data，命中返回 percent，未命中返回 -1。
    for (std::vector<Data>::iterator it = m_mileageList.begin();
         it != m_mileageList.end(); ++it)
        if (it->ipgNo == ipgNo)
            return it->percent;
    return -1;
}

int Arad_Mileage_Data::default_value()
{
    // ORIG 0x81a36f0：return *(int*)(this+0x04)
    return m_defaultPercent;
}

// ============================================================================
// Arad_OnePlusCardEvent_ItemList（+0x84）—— ORIG ctor 0x8188d0a / dtor 0x8188d6a /
// parsing 0x81a36fc / find_item x2 / is_limited_map。解析 "item"（web_id/item_index）与
// "limited_map"（index）。
// ============================================================================
// ORIG ctor @0x08188d0a / dtor @0x08188d6a（vector 成员自动构造/析构）。
Arad_OnePlusCardEvent_ItemList::Arad_OnePlusCardEvent_ItemList()
{
}

Arad_OnePlusCardEvent_ItemList::~Arad_OnePlusCardEvent_ItemList()
{
}

int Arad_OnePlusCardEvent_ItemList::parsing(char* data)
{
    Arad_Script script;
    // [b23判词·AE假阳性] 167/167 归一化逐条相等（含两段 Load 的实参来源、循环体内
    // 默认值先行 store、logFormat 五个 %esp 实参槽序）；残余 diff 全为未 relink .o 的
    // call/jmp 重定位占位与 .rodata/g_dnfLex 数据地址（立即数为节内偏移 + R_386_32，
    // 已逐处核对其字符串内容与 ORIG 0x8bac8xx 一致，§6.3 假阳性），relink 后转
    // IDENTICAL_AE。
    if (!script.Load(data, "item"))
        return 0x1db;
    if (script.GetRowNum() == 0)
        return 0;
    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        // ORIG 0x081a3767/0x081a376d：循环体先写默认值（movw $0x0 到 -0x12、
        // movl $0xffffffff 到 -0x10），再 GetInt 覆盖（P0：具名默认初始化局部）。
        unsigned short webId = 0;
        unsigned long itemIndex = 0xffffffff;
        webId = (unsigned short)script.GetInt(r, "web_id");
        itemIndex = (unsigned long)script.GetInt(r, "item_index");
        if (find_item(webId))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_Script_Loader.cpp",
                "virtual int Arad_OnePlusCardEvent_ItemList::parsing(char*)",
                0x1ea,
                "[ERROR] already exist data id OnePlusCardEvent(id:%d, item:%u)",
                (int)webId, (unsigned int)itemIndex);
            return 0x1eb;
        }
        m_cardList.push_back(Data(webId, itemIndex));
    }
    // ORIG 0x081a3857：第二段 Load 用 g_dnfLex.getInputStreamBuffer() 当前输入缓冲，
    // 非形参 data（item 段 Load 消耗后 lexer 流位置）。
    if (!script.Load(g_dnfLex.getInputStreamBuffer(), "limited_map"))
        return 0x1f3;
    if (script.GetRowNum() == 0)
        return 0;
    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        unsigned int map = (unsigned int)script.GetInt(r, "index");
        if (is_limited_map(map))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_Script_Loader.cpp",
                "virtual int Arad_OnePlusCardEvent_ItemList::parsing(char*)",
                0x1fe,
                "[ERROR] already exist limited map index (map:%u)",
                map);
            return 0x1ff;
        }
        m_limitedMap.push_back(map);
    }
    return 0;
}

bool Arad_OnePlusCardEvent_ItemList::find_item(unsigned short webId, Data& out)
{
    // ORIG 0x081a398e：Data(webId,0) + std::find → operator==（0x081a420a）按 webId 匹配。
    Data tmp(webId, 0);
    std::vector<Data>::iterator it = std::find(m_cardList.begin(), m_cardList.end(), tmp);
    if (it == m_cardList.end())
        return false;
    out.webId = it->webId;
    out.itemIndex = it->itemIndex;
    return true;
}

bool Arad_OnePlusCardEvent_ItemList::find_item(unsigned short webId)
{
    // ORIG 0x081a3a6e：Data(0,0) 委托两参 find_item，返回值透传。
    Data tmp(0, 0);
    return find_item(webId, tmp);
}

bool Arad_OnePlusCardEvent_ItemList::is_limited_map(unsigned int mapIndex)
{
    std::vector<unsigned int>::iterator it = std::find(
        m_limitedMap.begin(), m_limitedMap.end(), mapIndex);
    if (it == m_limitedMap.end())
    {
        return 0;
    }
    return 1;
}

// ============================================================================
// Arad_VillageAttackRewardItem（+0xa0）—— ORIG ctor 0x8188dbe / dtor 0x8188e1e /
// parsing 0x81a3b48 / find_reward_item 0x81a3e3c / find_ranking_reward_item 0x81a3ef0。
// 解析 "reward_item"（win_point/item_index/item_count）与 "reward_ranker"
// （item_index/item_count/item_index2/item_count2）。
// ============================================================================
Arad_VillageAttackRewardItem::Arad_VillageAttackRewardItem() {}
Arad_VillageAttackRewardItem::~Arad_VillageAttackRewardItem() {}

int Arad_VillageAttackRewardItem::parsing(char* data)
{
    // ORIG 0x81a3b48：Load 失败分别返回 0x232/0x24c；GetRowNum()==0 提前返回 0；
    // 循环条件每轮重调 GetRowNum()。原 if 包裹写法吞掉了错误码（真语义差）。
    Arad_Script script;
    if (!script.Load(data, "reward_item"))
        return 0x232;
    if (script.GetRowNum() == 0)
        return 0;
    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        // ORIG 0x81a3bbc：先构造 d（-0x1c）再构造 out（-0x28），GetInt 填 out 后拷 d，
        // find 用 d、logFormat/push_back 用 out。按此形态书写以匹配字节。
        Data d;
        Data out;
        out.winPoint = script.GetInt(r, "win_point");
        out.itemIndex = script.GetInt(r, "item_index");
        out.itemCount = script.GetInt(r, "item_count");
        d = out;
        if (find_reward_item(d.winPoint, d))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_Script_Loader.cpp",
                "virtual int Arad_VillageAttackRewardItem::parsing(char*)",
                0x244,
                "[ERROR] Already exist data. (point:%d, item:%u, count:%d)",
                out.winPoint, out.itemIndex, out.itemCount);
            return 0x245;
        }
        m_rewardList.push_back(out);
    }
    if (!script.Load(data, "reward_ranker"))
        return 0x24c;
    if (script.GetRowNum() == 0)
        return 0;
    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        RankData d;
        d.itemIndex = script.GetInt(r, "item_index");
        d.itemCount = script.GetInt(r, "item_count");
        d.itemIndex2 = script.GetInt(r, "item_index2");
        d.itemCount2 = script.GetInt(r, "item_count2");
        m_rankList.push_back(d);
    }
    return 0;
}

// b28d-solo-ARAD verdict=IDENTICAL/AE：ORIG 0x81a3e3c 不是循环而是
// `std::find(begin, end, out)`——Ghidra C 第四实参即 out（0x10(%ebp)），
// Data::operator== 仅比 winPoint（Arad_DataManager.h:357），故 winPoint 形参未使用
//（ORIG 全程无 0xc(%ebp) 读取，原码遗留，P171 照抄）；命中后 `out = *it`
//（ORIG 0x81a3ed1 三对 dword mov = 结构整体赋值 P247）。
bool Arad_VillageAttackRewardItem::find_reward_item(int winPoint, Data& out)
{
    // ORIG 0x81a3e52：std::find<iterator,Data>(sret -0x18, begin -0x10, end -0x14, out)。
    (void)winPoint;   // ORIG 未使用该形参（比较值取自 out）
    std::vector<Data>::iterator it =
        std::find(m_rewardList.begin(), m_rewardList.end(), out);
    if (it != m_rewardList.end())
    {
        out = *it;
        return true;
    }
    return false;
}

bool Arad_VillageAttackRewardItem::find_ranking_reward_item(short rank, RankData& out)
{
    // ORIG 0x81a3ef0：cmpw rank + js（rank<0）+ cmp 16 位 size + jle（rank>=size）
    // —— 有符号 16 位比较；合并 || 以匹配 js/jle 前置 false 布局。
    if (rank >= 0 && rank < (short)m_rankList.size())
    {
        out = m_rankList[rank];
        return true;
    }
    return false;
}

// ============================================================================
// Arad_DataManager（ORIG ctor 0x8188030；位于 ARAD 命名空间）
// ============================================================================
namespace ARAD
{
Arad_DataManager::Arad_DataManager()
    : m_scriptMap(), m_exceptionItem(), m_purchaseLimitItem(), m_bonusItem(),
      m_mileageData(), m_onePlusCard(), m_villageAttackReward()
{
}

Arad_DataManager::~Arad_DataManager()
{
    Destroy();    // ORIG 0x08188170：D1 写 vptr 后调 Destroy()（成员随后自动析构）
}

bool Arad_DataManager::Load()
{
    // ORIG 0x81882cc：转发 initAradData()
    return initAradData();
}

int Arad_DataManager::Destroy()
{
    // ORIG 0x81882e0：转发 destroyAradData()，返回 true（vtable 槽1）。
    destroyAradData();
    return 1;
}

// b28d-solo-ARAD verdict=IDENTICAL/AE：P118/P139 第一参用 __FUNCTION__（ORIG 局部静态符号
// _ZZN4ARAD16Arad_DataManager12initAradDataEvE12__FUNCTION__）；P221 全部
// cMyTrace 改「构造即调用」全表达式临时链复现 ORIG 递减槽序（-0x100…-0x30）；
// P197③ ep/openResult 用「声明=0 + 独立赋值」两句式（ORIG 0x08188304 -0x20、
// 0x08188691 -0x1c 各一条 movl $0）；单基类指针变量 script 复用 -0x20 槽（P211 档③
// 派生→基类赋值中转）；P225 5 处 open 检查改 `while(openResult != 0){…return}` 复现
// `cmpl $0,槽; setne; test; je`；trace2(0x59) 标志位按 ORIG 为 (true,false)。
// 345=345 指令全等，clean 归一化 0 残差。
bool Arad_DataManager::initAradData()
{
    // b28d-solo-ARAD：ORIG 0x08188304 单槽 -0x20 初始化一次，ep/st 两段复用同一
    // 基类指针变量（Ghidra local_24 被两派生类型共用；虚调用 vptr+8=槽2=loadScriptData）。
    // 派生*→基类* 的赋值另出 ORIG `mov %ebx,%eax; mov %eax,-0x20` 中转（P211 档③）。
    Arad_InterfaceGameScript* script = 0;
    // 各段起始/失败/成功均按 ORIG 记 cMyTrace（line/flag/bC/bD 逐点核对）：
    // 段①起(0x48,9,1,0)、Fail(0x4c/0x52,9,1,1)、Success(0x55,9,0,1)；
    // 段②起(0x59,9,1,0)、Fail(0x5d/0x63,9,1,1)、Success(0x66,9,0,1)；
    // 段③起(0x94,9,1,0)、open 失败(0x97/0x9d/0xa3/0xb2/0xbb,9,1,1) 带
    // "…script load fail. (line:%d)" 参数为 open 返回值。
    cMyTrace(__FUNCTION__, 0x48, 9, true, false)("\t- Loading Event Period Script - ");
    script = new Arad_EventPeriodDataManager();
    if (script == 0 || !script->loadScriptData())
    {
        cMyTrace(__FUNCTION__, 0x4c, 9, true, true)("Fail");
        return false;
    }
    if (!__insertGameScript(script))
    {
        cMyTrace(__FUNCTION__, 0x52, 9, true, true)("Fail");
        return false;
    }
    cMyTrace(__FUNCTION__, 0x55, 9, false, true)("Success");

    cMyTrace(__FUNCTION__, 0x59, 9, true, false)("\t- Loading Statistics Item List - ");
    script = new Arad_StatisticsEventItemList();
    if (script == 0 || !script->loadScriptData())
    {
        cMyTrace(__FUNCTION__, 0x5d, 9, true, true)("Fail");
        return false;
    }
    if (!__insertGameScript(script))
    {
        cMyTrace(__FUNCTION__, 0x63, 9, true, true)("Fail");
        return false;
    }
    cMyTrace(__FUNCTION__, 0x66, 9, false, true)("Success");

    int openResult = 0;
    cMyTrace(__FUNCTION__, 0x94, 9, true, false)("\t- Loading NexonFolder Script - ");
    openResult = m_exceptionItem.open("Nexon/AradSpecialUseItems.txt");
    while (openResult != 0) { cMyTrace(__FUNCTION__, 0x97, 9, true, true)(
        "AradSpecialUseItems script load fail. (line:%d)", openResult);
    return false; }
    openResult = m_purchaseLimitItem.open("Nexon/AradSpecialUseItems.txt");
    while (openResult != 0) { cMyTrace(__FUNCTION__, 0x9d, 9, true, true)(
        "AradSpecialUseItems script load fail. (line:%d)", openResult);
    return false; }
    openResult = m_bonusItem.open("Nexon/AradSpecialUseItems.txt");
    while (openResult != 0) { cMyTrace(__FUNCTION__, 0xa3, 9, true, true)(
        "AradSpecialUseItems script load fail. (line:%d)", openResult);
    return false; }
    openResult = m_mileageData.open("Nexon/CeraShopMileage.txt");
    while (openResult != 0) { cMyTrace(__FUNCTION__, 0xb2, 9, true, true)(
        "CeraShopMileage script load fail. (line:%d)", openResult);
    return false; }
    openResult = m_villageAttackReward.open("Nexon/VillageAttackRewardItem.txt");
    while (openResult != 0) { cMyTrace(__FUNCTION__, 0xbb, 9, true, true)(
        "VillageAttackRewardItem script load fail. (line:%d)", openResult);
    return false; }
    return true;
}

void Arad_DataManager::destroyAradData()
{
    // ORIG 0x8188952：std::for_each(begin,end,FN_DeleteGameScript()) 逐项删除，clear。
    // FN_DeleteGameScript::operator() 定义归 Arad_Verify.cpp（ORIG 0x8188c56），
    // 类声明见 ARAD_FN_DeleteGameScript.h（唯一声明点）。
    std::for_each(m_scriptMap.begin(), m_scriptMap.end(), FN_DeleteGameScript());
    m_scriptMap.clear();
}

bool Arad_DataManager::__insertGameScript(Arad_InterfaceGameScript* script)
{
    // ORIG 0x81889bc：scriptDataName 未注册则 m_scriptMap[name]=script 返回 true；否则 false。
    // 【b26 NL10】ORIG 以 (end结果槽, find结果槽) 序调 iterator::operator!=，
    // 源码写 end() != it 复现该实参序（4.1 实参求值 R→L）。
    std::map<std::string, Arad_InterfaceGameScript*>::iterator it =
        m_scriptMap.find(script->scriptDataName());
    if (m_scriptMap.end() != it)
        return false;
    m_scriptMap[script->scriptDataName()] = script;
    return true;
}

Arad_InterfaceGameScript* Arad_DataManager::findGameScript(std::string name)
{
    // ORIG 0x81882ac：return m_scriptMap[name]（未命中插入空指针，返回 0）。
    return m_scriptMap[name];
}

// Singleton<T>::Get() 模板定义已在 Arad_DataManager.h 提供；此处显式实例化。
template class Singleton<Arad_DataManager>;
template class Singleton<ScriptManager>;

// b28d-solo-ARAD verdict=IDENTICAL/AE：P248 档①（callee 返回类型旋钮）——返回类型
// int→bool 复现 ORIG 0x0819798d `call CUser::Send; mov %eax,%ebx`（4.4 对 int 返回恒
// `movzbl %al,%ebx`；bool 返回直出全宽 mov，探针 /tmp/probe_arad2.cpp f_bool）。
// 返回类型不入 mangled；调用点（Arad_ServerStateManager/GameWorld/Inter_LoadEtc）均
// 作语句调用忽略返回值，语义等价（0=失败，非0=成功）。
bool notifyOpenMessageDialog(CUser* user, int a, int b,
                             const AradServerStateMessage& msg)
{
    // [b23判词·AE假阳性] 65/65 归一化逐条相等（含 guard 局部 -0x18 寻址、put_binary
    // 的 `movl $0xc` 长度实参、异常清理路径的 %ebx/%esi 交换序）；残余 diff 全为未
    // relink .o 的 call 重定位占位 + GlobalData::s_systemTime_ 数据地址显示 0x0
    // （§6.3 假阳性），relink 后转 IDENTICAL_AE。
    if (user == 0)
        return 0;
    PacketGuard guard;
    // ORIG 0x8197907：getCurSec() 结果存 -0xc(%ebp) 栈槽后未再读（P0 具名死局部，
    // ORIG 原码含该存储语句；-O0 下保留槽与 store，与 ORIG 一致）。
    int curSec = GlobalData::s_systemTime_.getCurSec();
    (void)curSec;
    guard.put_header(0, 0x21a);
    guard.put_int(a);
    guard.put_int(b);
    guard.put_binary((const char*)&msg, 0xc);
    guard.finalize(true);
    return user->Send(guard);
}

}  // namespace ARAD

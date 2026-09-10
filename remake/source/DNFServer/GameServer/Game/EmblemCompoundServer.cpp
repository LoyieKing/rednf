// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EmblemCompoundServer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ EmblemCompoundServer + ARAD::SCRIPT::EmblemCompound 实现。
// 逐指令转写依据 docs/class_func_reports/ARAD__SCRIPT__EmblemCompound/*.md 与
// docs/class_func_reports/EmblemCompoundServer/{EmblemCompoundServer,~EmblemCompoundServer,
// Test,Load,Destroy}.md（ORIG 地址见各函数注释）。
#include <cstdio>
#include <string>

#include "EmblemCompoundServer.h"
#include "GameTypes.h"      // cMyTrace
#include "DNFLexWrapper.h"  // loadRDARScriptFile/unloadRDARScriptFile/ScanType/
                            // ScanInt(bool*)/ScanTypeOrInt（真实符号声明）

// ---- 布局断言（ORIG 反汇编实证）----
typedef char emblem_compound_size_assert[sizeof(ARAD::SCRIPT::EmblemCompound) == 0x84 ? 1 : -1];
typedef char emblem_compound_result_size_assert[sizeof(ARAD::SCRIPT::EmblemCompound::Result) == 0xa ? 1 : -1];
typedef char emblem_compound_server_size_assert[sizeof(EmblemCompoundServer) == 0x88 ? 1 : -1];

namespace ARAD
{
namespace SCRIPT
{

// ============================================================================
// EmblemCompound —— ORIG ctor 0x088b1408 / dtor 0x088b14c2。
// 成员按声明序自构造/自析构即与 ORIG 一致：
//   ctor：map(+0x00) → map(+0x18) → 7×vector(+0x30 起，步长 0xc)
//   dtor：逆序析构 7 个 vector → map(+0x18) → map(+0x00)
// ============================================================================
#include "EmblemCompoundServer/EmblemCompound.inc"
{
}

#include "EmblemCompoundServer/dtor_EmblemCompound.inc"
{
}

// ============================================================================
// loadScript —— ORIG T 0x088b154a。
// "Etc/EmblemRenewal/" + "emblem_compound_jpn.etc"；三节：
//   [emblem compound cost]   grade value         → m_cost（重复 grade → printf+-2）
//   [emblem compound result] grade 4×short flag  → m_result（new Result(0xa)；
//                              重复 grade → delete+printf+-2）
//   [emblem list]            listIdx itemIndex   → m_list[listIdx].push_back
// 循环形态与 ORIG 相同：节内由 ScanTypeOrInt 取数值项（isType==false&&isInt==true
// 才处理）；遇 TYPE/EOF 退出后回到外层 ScanType(token,true)。查重均为
// end()→find()→operator==、命中则插入否则报错的分支形态（与 ORIG 一致）。
// 返回 0=成功 / -1=打开失败 / -2=重复 grade。
// ============================================================================
// [b28d-solo-EmblemCompoundServer] loadScript 判词（ORIG 0x088b154a，273 insn）。
// 已修：外层 while→for(;;)+头部 break 复现 4.1 条件块前置布局（P283/P164，消 ~30 行
// 布局差）；new Result()（P90 value-init）；list 节 int 槽 + prvalue 转换临时
// （p167 实证 ORIG 调 push_back(T&&) `EOm`，具名左值会错选 `RKm`；修正后 p167 CLEAR
// 35/35）。
// 残余 N：ORIG +0x1c2 的 value-init 前置清零为 4.1 循环展开
// （`mov;mov;mov;movw;mov;movw;…;je;add;jmp` 清 8B + movb 尾，9 条），4.4 恒
// 展开 4×movw+movb（探针 /tmp/nr_probe.cpp `new R()` 实证，5 条）⇒ N78/跨版本
// 展开形态不可复现；gap 6 全在此段。其余助记符序列逐条同构（mnem-diff 0 块）。
#include "EmblemCompoundServer/loadScript.inc"
{
    if (!loadRDARScriptFile("Etc/EmblemRenewal/", "emblem_compound_jpn.etc"))
        return -1;

    std::string token;
    bool isType = false;
    bool isInt = false;
    // ORIG +0x40：`jmp cond` + 条件块在循环体之前（4.1 while 布局）⇒
    // for(;;)+头部 break 复现（P283/P164）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[emblem compound cost]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                int value = ScanInt((bool*)0);   // ORIG +0xb6：value 先扫再查重
                std::map<int, int>::iterator endIt = m_cost.end();
                if (m_cost.find(grade) == endIt)
                    m_cost.insert(std::map<int, int>::value_type(grade, value));
                else
                {
                    printf("[emblem compound cost] duplicate grade:%d", grade);
                    return -2;
                }
            }
        }
        else if (token == "[emblem compound result]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                // ORIG +0x1c2：new Result()（P90 value-init 前置清零循环 2×movw+movb，
                // 覆盖 +0..+8，跳过 +9 对齐 pad）后逐字段写入。
                Result* result = new Result();
                result->m_listIdx = static_cast<short>(ScanInt((bool*)0));
                result->m_itemIndex = static_cast<short>(ScanInt((bool*)0));
                result->m_itemCount = static_cast<short>(ScanInt((bool*)0));
                result->m_field6 = static_cast<short>(ScanInt((bool*)0));
                // [b23S3C] ORIG 0x088b17cx 段 `call ScanInt; mov %eax,-0x14(%ebp);
                // mov -0x60(%ebp),%eax; cmpl $0x1,-0x14(%ebp); sete %dl; mov %dl,
                // 0x8(%eax)` ⇒ 扫描结果有**具名 int 槽**（P0 反向：ORIG 有槽我方
                // 无）。单表达式 `(ScanInt()==1)` 恒 `cmp $0x1,%eax; sete %al`
                // 直比（少 2 条）⇒ 拆出具名局部复现槽 + 内存比较。
                int selectableValue = ScanInt((bool*)0);
                result->m_selectable = (selectableValue == 1);
                std::map<int, Result*>::iterator endIt = m_result.end();
                if (m_result.find(grade) == endIt)
                    m_result.insert(std::map<int, Result*>::value_type(grade, result));
                else
                {
                    ::operator delete(result);
                    printf("[emblem compound result] duplicate grade:%d", grade);
                    return -2;
                }
            }
        }
        else if (token == "[emblem list]")
        {
            for (;;)
            {
                int listIdx = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                // ORIG +0x36c：ScanInt 结果先落 int 槽(-0xc)，再转 unsigned long
                // 转换临时槽(-0x18)；p167 实证 ORIG 调 push_back(T&&)（EOm）⇒
                // 实参必须是 prvalue（禁具名左值，否则选 RKm 左值重载）。
                int itemIdx = ScanInt((bool*)0);
                // ORIG 直接 this + listIdx*12 + 0x30 索引（无边界检查，忠实保留）。
                m_list[listIdx].push_back(static_cast<unsigned long>(itemIdx));
            }
        }
    }
    unloadRDARScriptFile();
    return 0;
}

// ============================================================================
// clearScript —— ORIG T 0x088b1926：清 cost map → begin/end 各取一次后遍历
// delete Result* → 清 result map。不清理 m_list（ORIG 如此）。
// ============================================================================
#include "EmblemCompoundServer/clearScript.inc"
{
    m_cost.clear();
    std::map<int, Result*>::iterator it = m_result.begin();
    std::map<int, Result*>::iterator endIt = m_result.end();
    for (; it != endIt; ++it)
        ::operator delete(it->second);
    m_result.clear();
}

// getCost —— ORIG T 0x088b19b0：find→end→operator!=，命中返回 value，未命中 -1。
#include "EmblemCompoundServer/getCost.inc"
{
    std::map<int, int>::iterator it = m_cost.find(grade);
    if (it != m_cost.end())
        return it->second;
    return -1;
}

// getResult —— ORIG T 0x88b1a14：同上形态，命中返回 Result*，未命中 0。
#include "EmblemCompoundServer/getResult.inc"
{
    std::map<int, Result*>::iterator it = m_result.find(grade);
    if (it != m_result.end())
        return it->second;
    return 0;
}

#include "EmblemCompoundServer/getList.inc"
{
    // ORIG：先默认构造迭代器，再赋值 find 结果（帧内多出 C1 调用与拷贝）。
    std::map<int, Result*>::iterator it;
    it = m_result.find(grade);
    if (it != m_result.end())
        return &m_list[it->second->m_listIdx];
    return 0;
}

}  // namespace SCRIPT
}  // namespace ARAD

// ============================================================================
// 注：ORIG C1 中间的 ARAD::Singleton<EmblemCompoundServer>::Singleton()（W
// 0x08193636，空基类构造）未建模——与 CerashopAddRestrict::Manager 同口径，
// Singleton<T> 采用 Arad_DataManager.h 的 Meyers 模板。
// ============================================================================
#include "EmblemCompoundServer/EmblemCompoundServer.inc"
{
}

#include "EmblemCompoundServer/dtor_EmblemCompoundServer.inc"
{
}

// Load —— ORIG T 0x081915ba：cMyTrace(pretty,0x1c7,9,true,false) +
// operator()("\t- Loading Emblem Compound script - ") + loadScript()==0。
#include "EmblemCompoundServer/Load.inc"
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1c7, 9, true, false);
    trace("\t- Loading Emblem Compound script - ");
    if (loadScript())
        return false;
    return true;
}

// Destroy —— ORIG T 0x0819162c：return 1。基类 ScriptInterface 槽1 为 void
// （仓库权威约定），可观察行为一致。
#include "EmblemCompoundServer/Destroy.inc"
{
    return 1;
}

// Test —— ORIG T 0x081915b4：空（非虚成员）。
#include "EmblemCompoundServer/Test.inc"
{
}

// ============================================================================
// LinkClosureB.AradServers 追加段：
//   GetEmblemInfo        ORIG T 0x08191818（GetEmblemInfo.md 全文转写）
//   GetEmblemRandomIndex ORIG T 0x081919e8（GetEmblemRandomIndex.md 全文转写）
//   IsResultSelectable   ORIG T 0x08191ad6（IsResultSelectable.md 全文转写）
//   CheckCondition       ORIG T 0x08191636（CheckCondition.md 全文转写）
//   Process              ORIG T 0x08191afa（Process.md 全文转写）
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "MoreTypes.h"          // arad::SigAradEmblemCompound 完整定义
#include "Inven_Item.h"
#include "CDataManager.h"       // G_CDataManager / find_item / ENUM_CMDPACKET
#include "LogManager.h"
#include "PacketGuard.h"
#include "CMailBoxHelper.h"     // ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"
#include "CompoundEmblemDeps.h" // getCompoundEmblemStatistic
#include "AvatarConvertServer.h" // StatisticsEmblemCreate

// [b28d-solo-EmblemCompoundServer] GetEmblemInfo 判词（ORIG 0x08191818，126 insn）：
// IDENTICAL/AE。修法：SigAradEmblemCompound 加 materialInfo[2] 匿名 union 数组视角
// （P235/P246，scaled 索引 `(%eax,%edx,8)`）；slot 内联实参无槽（P0/P157）；
// materialInfo::itemIdx 同型化为 unsigned int 复现 ORIG `mov 0x2(%eax),%ecx` 单条
// 直装（P211 档③/P248；int 字段恒 `mov→%eax; mov %eax,%ecx` 两段）。
#include "EmblemCompoundServer/GetEmblemInfo.inc"
                                         arad::SigAradEmblemCompound* sig,
                                         int idx)
{
    // ORIG 0x08191818：slot 为内联实参（无槽），每次使用点重读 m_material[idx].slot
    // （P157），scaled 索引 (%eax,%edx,8) 由数组视角复现。
    const Inven_Item* ref = user->getCurCharacInvenR()->GetInvenRef(
        1, sig->m_material[idx].slot);
    if (ref == 0 || ref->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0x16);
        ARAD::PRINT_ERROR::isEmpty("EMBLEM COMPOUND", user,
                                   sig->m_material[idx].slot);
        return false;
    }
    if (user->CheckItemLock(1, sig->m_material[idx].slot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0xd5);
        ARAD::PRINT_ERROR::CheckItemLock("EMBLEM COMPOUND", user,
                                         sig->m_material[idx].slot);
        return false;
    }
    CItem* item = G_CDataManager()->find_item((int)ref->m_addInfo);
    if (item == 0 || item->GetItemType() != 0x14)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0x11);
        ARAD::PRINT_ERROR::find_item("EMBLEM COMPOUND", user,
                                     (int)ref->m_addInfo);
        return false;
    }
    // 写回条目 grade(+2)/itemIdx(+4)：ORIG scaled store。
    sig->m_material[idx].grade = (short)item->get_grade();
    sig->m_material[idx].itemIdx = ref->m_addInfo;
    return true;
}

// [b28d-solo-EmblemCompoundServer] GetEmblemRandomIndex 判词（ORIG 0x081919e8，77 insn）：
// IDENTICAL/AE。修法：listIdx 改 `int listIdx = 0`（ORIG movl 清零 ⇒ P10）；
// get_rand_int 结果具名 int 局部（槽 -0x10）；empty 后重发 m_result[grade]
// （P157 逐使用点重读）+ `if(!m_selectable)` 取反形（P168a/P292，xor $1）。
#include "EmblemCompoundServer/GetEmblemRandomIndex.inc"
                                               unsigned short selectIdx)
{
    ARAD::SCRIPT::EmblemCompound::Result* result = m_result[grade];
    // ORIG：rand(1000) < elem+2(m_itemIndex) → 取 +0(m_listIdx)，否则 +4(m_itemCount)
    int listIdx = 0;
    int randValue = get_rand_int(1000);
    if (randValue < (int)(short)result->m_itemIndex)
        listIdx = result->m_listIdx;
    else
        listIdx = result->m_itemCount;

    std::vector<unsigned long>& list = m_list[listIdx];
    if (list.empty())
        return 0;
    // ORIG +0x8f 重发 m_result[grade]（P157 逐使用点重读），取反形 xor $1。
    if (!m_result[grade]->m_selectable)
        return (int)list[get_rand_int((int)list.size())];
    return (int)list[selectIdx];
}

#include "EmblemCompoundServer/IsResultSelectable.inc"
{
    // P0：ORIG 将 m_result[grade]（map::operator[] 返回）右值直传解引用，无
    // 具名局部槽（0x08191af1 `mov (%eax),%eax` 直连 `movzbl 8(%eax)`）。
    return m_result[grade]->m_selectable != 0;
}

// [b28d-solo-EmblemCompoundServer] CheckCondition 判词（ORIG 0x08191636，124 insn）：
// IDENTICAL/AE。修法：grade0 具名 int 局部（ORIG 槽 -0xc，movzwl+cwtl）；money/need
// 去具名局部改内联（`get_money() < m_cost[grade0]`，P52 左值先物化 %ebx）；
// logFormat 的 money/need 逐实参重算（P157/P92）。
#include "EmblemCompoundServer/CheckCondition.inc"
                                          arad::SigAradEmblemCompound* sig)
{
    // [b23S3C] N：帧/寄存器分配差（ORIG push esi/sub 0x30 vs 0x34）+ grade
    // 有符号扩展比较（movswl/cwtl 物化）非本件可复现，语义等价已核（详见 b22_s3_verdicts.tsv）
    if (!GetEmblemInfo(user, sig, 0))
        return false;
    if (!GetEmblemInfo(user, sig, 1))
        return false;
    // ORIG +0x6c：grade0 提升为具名 int 局部（槽 -0xc），grade1 重读提升后比较。
    int grade0 = (int)sig->m_grade0;
    if (!((int)sig->m_grade1 == grade0 && grade0 < 7))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0xd8);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x1e8,
            "EMBLEM COMPOUND : [grade1:%d] [grade2:%d] [charac_no:%d]",
            (int)sig->m_grade0, (int)sig->m_grade1,
            (unsigned int)user->getCurCharacNo());
        return false;
    }
    // ORIG +0x10c：money 左值先求值保活 %ebx，m_cost[grade0] 右值直比（无 need/money 槽）。
    if (user->getCurCharacInvenR()->get_money() < m_cost[grade0])
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 10);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x1f0,
            "EMBLEM COMPOUND : [money:%d] [need:%d] [charac_no:%d]",
            user->getCurCharacInvenR()->get_money(), m_cost[grade0],
            (unsigned int)user->getCurCharacNo());
        return false;
    }
    return true;
}

// [b28d-solo-EmblemCompoundServer] Process 判词（ORIG 0x08191afa，426 insn）。
// 已修：补 `newItem.m_addInfo = resultItem->get_index()`（ORIG +0x2fc，原缺此步）；
// make_pair 改 `(unsigned long&)m_addInfo` 左值直传复现 ORIG 符号
// `_ZSt9make_pairIRmRiE…`（0x08111a37）；put_byte 实参改
// `Singleton<EmblemCompoundServer>::Get()->IsResultSelectable(grade0)`（ORIG +0x555，
// 非 this，且去 `?1:0` 物化）；resultIdx 改 unsigned int 复现 find_item 实参先装
// %ebx 跨 receiver call（P290/P52）；cost/invW 去具名局部（P157 重取）。
// 残余 N：ORIG +0x3e4 `movzwl; not; shr $0xf,%ax; test %al; jne`（4.1 对 short<0
// 的符号位提取 lowering）；4.4 对 `< 0` 恒 `cmpw $0; jns`，对 (unsigned short)~x>>15
// 形恒提升 32 位（movzwl+sar 6 条，实测）⇒ 跨版本 lowering 封顶（P196 负例族），
// gap 3 全在此段；其余助记符序列逐条同构（mnem-diff 仅此 1 块）。
#include "EmblemCompoundServer/Process.inc"
                                   arad::SigAradEmblemCompound* sig)
{
    unsigned int resultIdx = (unsigned int)GetEmblemRandomIndex(sig->m_grade0, sig->m_selectIdx);
    if (resultIdx == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0x13);
        return false;
    }
    CItem* resultItem = G_CDataManager()->find_item(resultIdx);
    if (resultItem == 0 || resultItem->GetItemType() != 0x14)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0x16);
        ARAD::PRINT_ERROR::find_item("EMBLEM COMPOUND", user, resultIdx);
        return false;
    }

    // ORIG +0xe1：m_cost[grade0] 内联实参（无 cost 槽，值保活 %ebx 跨 receiver call），
    // getCurCharacInvenW 每个调用点重取（P157/P52）。
    if (user->getCurCharacInvenW()->use_money(m_cost[sig->m_grade0],
                                              eMoneySubReason(0x23), true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 10);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x24e,
            "EMBLEM COMPOUND : [money:%d] [need:%d] [charac_no:%d]",
            user->getCurCharacInvenR()->get_money(), m_cost[sig->m_grade0],
            (unsigned int)user->getCurCharacNo());
        return false;
    }
    // 删除两张纹章材料（type1 slot0/slot1，reason=8）
    if (user->getCurCharacInvenW()->delete_item(INVEN_TYPE(1), (int)sig->m_slot0, 1,
                          eItemDelReason(8), true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 1);
        ARAD::PRINT_ERROR::delete_item("EMBLEM COMPOUND", user, sig->m_slot0);
        return false;
    }
    if (user->getCurCharacInvenW()->delete_item(INVEN_TYPE(1), (int)sig->m_slot1, 1,
                          eItemDelReason(8), true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 1);
        ARAD::PRINT_ERROR::delete_item("EMBLEM COMPOUND", user, sig->m_slot1);
        return false;
    }

    // 合成结果物品：ORIG +0x2ee：newItem 构造 → m_addInfo = resultItem->get_index()
    // → 虚 make_item(newItem)（vptr+8）→ set_add_info(1) → insert(reason=9)。
    Inven_Item newItem;
    newItem.m_addInfo = resultItem->get_index();
    resultItem->make_item(newItem);
    newItem.set_add_info(1);
    short insertSlot = (short)user->getCurCharacInvenW()->insertItemIntoInventory(
        newItem, eItemAddReason(9), true, true);
    // [b28d-solo-EmblemCompoundServer] N：ORIG +0x3e4 用 `movzwl; not; shr $0xf,%ax;
    // test %al; jne`（4.1 对 short<0 的符号位提取 lowering）；4.4 对 `< 0` 恒
    // `cmpw $0; jns`，对 (unsigned short)~x>>15 形恒提升 32 位（movzwl+sar，实测
    // 6 条 vs ORIG 5 条）⇒ 跨版本 lowering 封顶，保持自然语义。
    if (insertSlot < 0)
    {
        const char* title =
            g_scriptStringManager_.findString(4, "game_server_msg_122", (bool*)0);
        const char* body =
            g_scriptStringManager_.findString(4, "game_server_msg_123", (bool*)0);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, newItem, 0, (unsigned int)user->getCurCharacNo(), body,
            (unsigned int)std::strlen(body), 0, user->GetServerGroup(),
            false, false);
    }
    sig->m_resultIdx = resultIdx;

    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x24e);
    packet.put_byte(1);
    packet.put_int(user->getCurCharacInvenR()->get_money());
    packet.put_short(sig->m_slot0);
    packet.put_short(sig->m_slot1);
    // ORIG +0x555：receiver 是 Singleton<EmblemCompoundServer>::Get()（08193776），
    // grade0 先物化 %ebx 跨 receiver call（P52）；bool 直传 put_byte（勿 ?1:0 物化）。
    packet.put_byte(
        ARAD::Singleton<EmblemCompoundServer>::Get()->IsResultSelectable(
            sig->m_grade0));
    packet.put_int(sig->m_resultIdx);
    packet.finalize(true);
    user->Send(packet);
    if (insertSlot >= 0)
        user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                             (int)insertSlot);

    getCompoundEmblemStatistic()->increaseCount((int)sig->m_grade0);

    std::vector<std::pair<int, int> > created;   // StatisticsEmblemCreate 入参
    // ORIG 0x08111a37 `make_pair<unsigned long&,int&>(unsigned long&,int&)`（IRmRiE）：
    // m_addInfo 以 unsigned long& 左值直传（P61/P315；值临时会实例化出另一符号）。
    created.push_back(std::make_pair((unsigned long&)newItem.m_addInfo,
                                     newItem.m_addInfo2));
    AvatarConvertServer::StatisticsEmblemCreate(created);
    return true;
}

// df_game_r Game/ AvatarConvertServer + ARAD::SCRIPT::AvatarConvert 实现。
// 逐指令转写依据 docs/class_func_reports/ARAD__SCRIPT__AvatarConvert/*.md 与
// docs/class_func_reports/AvatarConvertServer/{AvatarConvertServer,~AvatarConvertServer,
// Test,Load,Destroy}.md（ORIG 地址见各函数注释）。
#include <cstring>
#include <new>

#include <cstdio>
#include <string>

#include "AvatarConvertServer.h"
#include "GameTypes.h"      // cMyTrace
#include "DNFLexWrapper.h"  // loadRDARScriptFile/unloadRDARScriptFile/ScanType/
                            // ScanInt(bool*)/ScanTypeOrInt（真实符号声明）

// ---- 布局断言（ORIG 反汇编实证；std::map=0x18 与 ORIG 一致）----
typedef char avatar_convert_size_assert[sizeof(ARAD::SCRIPT::AvatarConvert) == 0x18 ? 1 : -1];
typedef char avatar_convert_getinfo_size_assert[sizeof(ARAD::SCRIPT::AvatarConvert::GetInfo) == 0x8 ? 1 : -1];
typedef char avatar_convert_result_size_assert[sizeof(ARAD::SCRIPT::AvatarConvert::ConvertResult) == 0x24 ? 1 : -1];
typedef char avatar_convert_server_size_assert[sizeof(AvatarConvertServer) == 0x1c ? 1 : -1];

namespace ARAD
{
namespace SCRIPT
{

// ============================================================================
// GetInfo —— POD 聚合；trivial D1/D0 弱符号由编译器在 vector 销毁路径产生
// （ORIG W 0x088b3148）。
//
// ConvertResult —— ORIG W ctor 0x088b1f42 / dtor 0x088b1f8e。ORIG loadScript 先对
// 0x24 字节 memset(0) 再调用本 ctor，ctor 内仅默认构造两个 vector——标量不在此
// 清零，与 ORIG 一致。
// ============================================================================
// ============================================================================
// AvatarConvert —— ORIG ctor 0x088b1b0e：map ctor。dtor 0x088b1b22：仅 map dtor
// （不 delete 条目——释放须经 clearScript，ORIG 如此）。
// ============================================================================
AvatarConvert::AvatarConvert()
{
}

AvatarConvert::~AvatarConvert()
{
}

// ============================================================================
// loadScript —— ORIG T 0x88b1b36。"Etc/EmblemRenewal/" + "avatar_convert_jpn.etc"。
// 每条记录：TYPE 头 → grade=ScanInt、flag=(ScanInt==1) → 循环 ScanTypeOrInt 取
// 数值键 + 2×short（push GetInfo{key,idx,cnt}，累计 cnt 入 m_cumWeights）→
// 因 TYPE 退出时查重插入 m_convertMap（total=m_cumWeights.back()），因 EOF/非法
// 退出则整条丢弃。重复 grade：printf("%s duplicate grade:%d") 后返回 -2
// （ORIG 此路径不释放 cr——忠实保留）。
// 返回 0=成功 / -1=打开失败 / -2=重复 grade。
// ============================================================================
// [b28d-solo-AvatarConvertServer] NEAR（ORIG 0x088b1b36，190 vs 190，助记符序列全等，
// p167 CLEAR 25/25）。修法：P299/P318 value-init（删 ConvertResult 用户 ctor/dtor ⇒
// `new ConvertResult()` 前置零初始化，消原 operator new+memset+placement new 的 N83）+
// P164 循环头形（for(;;)+if(!ScanType) break，复现 4.1 入口直落条件头）+
// P0 去 key 具名局部（info.m_key 直收 ScanTypeOrInt 返回值）+ P220 正极性 find==end +
// field20 具名临时（复现 ORIG -0x1c 比较前落槽）。
// 残差：槽号整体浅 4（块内 endIt 的 end() sret -0x48 vs ORIG -0x34；试函数头先声明后
// 赋值反而劣化 195 条 ⇒ 回退）⇒ N82/P112 槽分配族，语义完成。
int AvatarConvert::loadScript()
{
    if (!loadRDARScriptFile("Etc/EmblemRenewal/", "avatar_convert_jpn.etc"))
        return -1;

    std::string token;
    bool isType = false;
    bool isInt = false;
    for (;;)
    {
        if (!ScanType(token, true))
            break;               // P164：条件落循环头，复现 4.1 入口直落条件头
        int totalWeight = 0;   // ORIG -0x40：每条记录重置
        // ORIG 0x088b1baf：`new ConvertResult()` 的 value-init 前置零初始化
        // （operator new(0x24) → 8×rep stos dword + 1B → call C1Ev）。
        ConvertResult* convertResult = new ConvertResult();
        convertResult->m_grade = ScanInt((bool*)0);
        int field20 = ScanInt((bool*)0);   // ORIG -0x1c：比较前落槽
        convertResult->m_field20 = (field20 == 1);
        for (;;)
        {
            GetInfo info;
            // P0：ORIG 无具名 key 局部——ScanTypeOrInt 返回值直落 info.m_key。
            info.m_key = ScanTypeOrInt(isType, token, isInt, true);
            if (isType)
                break;   // TYPE → 插入分支
            if (!isInt)
                break;   // EOF/非法 → 丢弃该记录
            info.m_count = static_cast<short>(ScanInt((bool*)0));
            info.m_weight = static_cast<short>(ScanInt((bool*)0));
            convertResult->m_infos.push_back(info);
            totalWeight += info.m_weight;
            convertResult->m_cumWeights.push_back(totalWeight);
        }
        if (isType)
        {
            std::map<int, ConvertResult*>::iterator endIt = m_convertMap.end();
            if (m_convertMap.find(convertResult->m_grade) == endIt)
            {
                convertResult->m_totalWeight = convertResult->m_cumWeights.back();
                m_convertMap.insert(
                    std::map<int, ConvertResult*>::value_type(convertResult->m_grade,
                                                              convertResult));
            }
            else
            {
                printf("%s duplicate grade:%d", token.c_str(), convertResult->m_grade);
                return -2;
            }
        }
    }
    unloadRDARScriptFile();
    return 0;
}

// ============================================================================
// clearScript —— ORIG T 0x88b1dc2：begin/end 各取一次，遍历 delete ConvertResult*
// （delete 表达式自带判空分支，与 ORIG 的 test→D1→operator delete 形态一致）
// → map.clear()。
// ============================================================================
void AvatarConvert::clearScript()
{
    std::map<int, ConvertResult*>::iterator it = m_convertMap.begin();
    std::map<int, ConvertResult*>::iterator endIt = m_convertMap.end();
    for (; it != endIt; ++it)
        delete it->second;
    m_convertMap.clear();
}

// getConvertResult —— ORIG T 0x88b1e48：先默认构造迭代器再赋值 find 结果；
// 命中返回指针，未命中 0。
AvatarConvert::ConvertResult* AvatarConvert::getConvertResult(int grade)
{
    std::map<int, ConvertResult*>::iterator it;
    it = m_convertMap.find(grade);
    if (it != m_convertMap.end())
        return it->second;
    return 0;
}

}  // namespace SCRIPT
}  // namespace ARAD

// ============================================================================
// AvatarConvertServer —— ORIG C1 0x081921b6 / D1 0x081921ec。基类口径同
// EmblemCompoundServer.cpp 注释（Singleton W 0x0819380a 未建模）。
// ============================================================================
AvatarConvertServer::AvatarConvertServer()
{
}

AvatarConvertServer::~AvatarConvertServer()
{
}

// Load —— ORIG T 0x081930ba：cMyTrace(pretty,0x37f,9,true,false) +
// operator()("\t- Loading Avatar Convert script - ")；loadScript() 非0 → 0。
bool AvatarConvertServer::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x37f, 9, true, false);
    trace("\t- Loading Avatar Convert script - ");
    if (loadScript())
        return false;
    return true;
}

// Destroy —— ORIG T 0x0819312c：return 1（基类槽1 void 约定）。
int AvatarConvertServer::Destroy()
{
    return 1;
}

// Test —— ORIG T 0x0819220c：空（非虚成员）。
void AvatarConvertServer::Test()
{
}

// ============================================================================
// LinkClosureB.AradServers 追加段：
//   StatisticsAvatarDisjoint ORIG T 0x08193136（StatisticsAvatarDisjoint.md 转写）
//   StatisticsEmblemCreate  ORIG T 0x081931dc（StatisticsEmblemCreate.md + 汇编，
//     包布局 Packet_Emblem_Create_Statistic：PacketHeader(0x17a4,0x36)+count@0xa
//     +grades@0xe[5]+addInfo@0x22[5]；ORIG packet 类 C1 W @0x08110bdc 归
//     PacketTypes 域，此处按字节布局组装，不重复建模。）
//   CheckCondition          ORIG T 0x08192212（CheckCondition.md 全文转写）
//   Convert                 ORIG T 0x081926de（Convert.md 全文转写）
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "CAvatarItemMgr.h"
#include "CDataManager.h"       // G_CDataManager / find_item / ENUM_CMDPACKET
#include "LogManager.h"
#include "CStatisticServerProxy.h"
#include "CServerProxyMgr.h"
#include "GlobalData.h"
#include "AvatarRechargeServer.h"

#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MoreTypes.h"          // arad::SigAradAvatarConvert 完整定义
#include "Packet_Avater_Disjoint_Statistic.h"  // 统计包权威布局（ORIG W C1 0x08110bae）
#include "Packet_Emblem_Create_Statistic.h"    // 统计包权威布局（ORIG W C1 0x08110bdc）
// DB_UpdateAvatarHistory —— ORIG makeRequest T @0x0844536a。
// 权威定义在 CompoundDeathLCB.cpp（LinkClosureB.CompoundDeath 批），
// 此处仅经 DB_UpdateAvatarHistory.h 引用，避免多重定义。
#include "DB_UpdateAvatarHistory.h"

    // [b28d-solo-AvatarConvertServer] AE（ORIG 0x08193136，49/49，p167 CLEAR）。
    // N7 间接 call 目标寄存器（ORIG 0x0819319b `mov (%eax),%edx; call *%edx`）在
    // 4.4.7 下恒 %eax；Main 已按本泳道双后端读数把本 TU 路由到 446 后端
    // （dnf_game_cxx.sh「仅换 446 后端」表），逐操作数一致。P282 已核虚槽 +0xc。
void AvatarConvertServer::StatisticsAvatarDisjoint(const CItem* item,
                                                   int isRechargeStone)
{
    // ORIG 0x08193136：Packet_Avater_Disjoint_Statistic（W C1 0x08110bae）构造
    // 于 -0x34；m_field0a=1 先行；gradeIdx 具名临时 + if/else 两段（sete 物化 +
    // 槽往返写回 +0x0e）；m_pad17 ← isRechargeStone（4 字节写 +0x17）；代理内联。
    Packet_Avater_Disjoint_Statistic packet;
    packet.m_field0a = 1;
    int gradeIdx = 0;
    if (item->get_rarity() == 2)
        gradeIdx = 2;
    else
        gradeIdx = item->get_grade() - 1;
    packet.m_gradeIdxA = gradeIdx;
    packet.m_subTypeA = item->GetItemType();
    packet.m_flagA = 1;
    packet.m_pad17 = static_cast<unsigned int>(isRechargeStone);
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x28);
}

// [b28d-solo-AvatarConvertServer] DIFF 封顶（ORIG 0x081931dc，115 vs 114，p167 CLEAR）。
// 修法：权威 Packet_Emblem_Create_Statistic C1 W 0x08110bdc（消原 char[0x36]+memset+
// 字节写）+ m_grades/m_values 具名数组（P311/P95）+ 语句位 `it++` 复现 ppEi（探针实证，
// N113 仅适用 for 增量段）+ int i 复现 cmpl $5/jg + 代理内联（消 TU-local helper）。
// 残差（语义完成、byte 层 N 封顶）：① find_item 实参求值序——ORIG 0x0819327b
// `mov -0x14(%ebp),%ebx; call G_CDataManager; mov %ebx,0x4(%esp)`，4.4 恒 receiver 先
// （P75/N82附）② `mov -0x18(%ebp),%ebx; call size; cmp %eax,%ebx`（N22）③ m_values 的
// disp/索引分解 -0x58+4(i+8) vs -0x50+4(i+6)（N94/N4 族）。
void AvatarConvertServer::StatisticsEmblemCreate(
    const std::vector<std::pair<int, int> >& created)
{
    Packet_Emblem_Create_Statistic packet;
    if (created.size() > 5)
        return;

    // P198③/P212：i 先声明占最浅槽、赋值留到 begin/end 之后（ORIG i=0 在
    // end() 调用后发射）。
    int i;
    std::vector<std::pair<int, int> >::const_iterator it = created.begin();
    std::vector<std::pair<int, int> >::const_iterator endIt = created.end();
    i = 0;
    while (it != endIt)
    {
        const int itemId = it->first;
        const int addInfo = it->second;
        it++;                                    // 语句位后置：4.4 保留 ppEi（探针实证）
        CItem* item = G_CDataManager()->find_item(itemId);
        if (item == 0)
            return;                              // ORIG 中途 return
        if (item->GetItemType() != 0x14)
            continue;                            // 非头像物品不计入
        packet.m_grades[i] = item->get_grade();
        packet.m_slots[i + 6] = addInfo;
        ++i;
        if (i > 5)
            break;                               // ORIG 计数上限 cmp $5
    }
    if (i != (int)created.size())
        return;
    packet.m_count = i;
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x36);
}

// [b28d-solo-AvatarConvertServer] DIFF 封顶（ORIG 0x08192212，293 vs 292，p167 CLEAR 35/35）。
// 真语义 bug：成功条件极性整段反置——ORIG 0x08192617 `movzbl 0x20; xor $1; test; je` +
// 0x08192639 `cmp; jb` ⇒ 失败条件 = `!m_field20 && size >= selectNo`；原实现写成
// `m_field20 || size <= selectNo`（条件与返回值双向错）。修法：invR/mgr 内联（P52/P0，
// 消帧 0x54→0x44）+ m_slot 三处使用点 `(unsigned short)` 两段零扩展 + 删 `(int)` 冗余 cast。
// 残差：1 条 N18a——ORIG 0x081923d0 `mov 0x4(%eax),%eax; mov %eax,%ebx`（两段装 callee-saved），
// 4.4 恒折单段 ⇒ N 封顶。
bool AvatarConvertServer::CheckCondition(CUser* user,
                                         arad::SigAradAvatarConvert* sig)
{
    // P0/P52：ORIG 无 invR 具名局部（链式 receiver）；m_slot 为 short 字段但
    // ORIG 三处使用点均为 `movzwl 0x2; movzwl %ax` 两段零扩展 ⇒ 使用点显式
    // `(unsigned short)` 收窄（负值语义照抄 ORIG）。
    const Inven_Item* ref =
        user->getCurCharacInvenR()->GetInvenRef(2,
            (int)(unsigned short)sig->m_slot);
    if ((ref == 0 || ref->isEmpty()))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x16);
        ARAD::PRINT_ERROR::isEmpty("AVATAR CONVERT", user,
                                   (unsigned short)sig->m_slot);
        return false;
    }
    if (ref->m_addInfo != sig->m_itemId)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x11);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x2b5,
            "AVATAR CONVERT : [item_id1:%d] [item_id2:%d] [charac_no:%d]",
            ref->m_addInfo, sig->m_itemId,
            (unsigned int)user->getCurCharacNo());
        return false;
    }
    if (user->CheckItemLock(2, (int)(unsigned short)sig->m_slot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0xd5);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            700,
            "AVATAR CONVERT : lock [item_id1:%d] [item_id2:%d] [charac_no:%d]",
            ref->m_addInfo, sig->m_itemId,
            (unsigned int)user->getCurCharacNo());
        return false;
    }
    CItem* item = G_CDataManager()->find_item(sig->m_itemId);
    if (item == 0 || !item->IsAvatarItem())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x11);
        ARAD::PRINT_ERROR::find_item("AVATAR CONVERT", user, sig->m_itemId);
        return false;
    }
    if (!item->isEnableAvatarFunc((ENUM_AVATAR_FUNC_FILTER)2))
    {   // AVATAR_FUNC_FILTER_DISJOINT
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x17);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x2d2,
            "AVATAR CONVERT : isEnableAvatarFunc(AVATAR_FUNC_FILTER_DISJOINT) fail "
            "[item_id:%d] [charac_no:%d]",
            item->get_index(), (unsigned int)user->getCurCharacNo());
        return false;
    }
    // avatar 到期检查（ORIG 0x081924ec：无 mgr 具名局部——链式 receiver +
    // ref->m_addInfo2 先装 ebx 跨两次调用保活，P52）。
    if (user->getCurCharacInvenW()->GetAvatarItemMgrR()
            ->GetExpireDate(ref->m_addInfo2) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x17);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x2d9,
            "AVATAR CONVERT : GetExpireDate() fail [ui_id:%d] [charac_no:%d]",
            ref->m_addInfo2, (unsigned int)user->getCurCharacNo());
        return false;
    }
    ARAD::SCRIPT::AvatarConvert::ConvertResult* result =
        getConvertResult(item->get_grade());
    if (result == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x11);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x2e2,
            "AVATAR CONVERT : getConvertResult() fail [item_id:%d] [charac_no:%d]",
            sig->m_itemId, (unsigned int)user->getCurCharacNo());
        return false;
    }
    // [b28d-solo-AvatarConvertServer 真语义 bug] ORIG 0x08192614：
    // `movzbl 0x20(%eax); xor $1; test; je <mov$0>` + `cmp; jb <mov$0>` ⇒
    // 失败条件 = !m_field20 && size >= selectNo（原实现写成 m_field20 || size <= selectNo，
    // 极性整段反置）。结尾 `return !bVar2` 即条件假时 return true。
    if (!result->m_field20 &&
        result->m_infos.size() >=
            (std::vector<ARAD::SCRIPT::AvatarConvert::GetInfo>::size_type)sig->m_selectNo)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x17);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x2e9,
            "AVATAR CONVERT : [isSelect:%d] [selectNo:%d] [item_id:%d] [charac_no:%d]",
            (unsigned int)result->m_field20, (unsigned int)sig->m_selectNo,
            sig->m_itemId, (unsigned int)user->getCurCharacNo());
        return false;
    }
    return true;
}

// [b28d-solo-AvatarConvertServer] DIFF 封顶（ORIG 0x081926de，610 vs 601，p167 CLEAR
// 85/85，§6.2 超大函数三件套口径）。真语义 bug 3 处：
//   ① `sig->m_resultCount` 取错字段——ORIG 0x081928a5 取 GetInfo+0x4（数量），
//      原实现取 +0x6（权重）；字段名已按 ORIG 语义改 m_count@+0x4 / m_weight@+0x6。
//   ② newItem 漏写 m_addInfo/m_addInfo2——ORIG 0x0819292d 写 +0x2、0x0819294e 写 +0x7
//      （且 m_addInfo2 在 stackable 校验之后赋值）。
//   ③ StatisticsEmblemCreate 实参应复用 items——ORIG 0x0819301d `lea -0x98(%ebp)`
//      （原实现多建 created vector：p167 EXTRA vector<pair> C1/D1/push_back）。
// 修法：invW 内联 5 处（p167 MISSING getCurCharacInvenW×4）+ oldRef 走 const 版
// getCurCharacInvenR/GetInvenRef（MISSING const 两符号）+ GetInfo 逐字段 operator[]（P157，
// MISSING ixEj）+ make_pair 左值实参 + MoreTypes m_resultItemId→unsigned long（MISSING
// make_pairIRmRsE）+ 去 count 局部。
// 残差：N82 槽分配（帧 0x14c vs 0x140，全帧浅移 4）+ N18a（sig->m_itemId 两段装 ebx）+
// edi 保活（cMyTrace 段 ORIG `movzwl %ax,%edi; mov %edi,0xc(%esp)`）+ P168a③b（ORIG
// is_stackable `test %al,%al; je` 无 xor，4.4 恒 xor 物化）⇒ 语义完成、byte 层 N 封顶。
bool AvatarConvertServer::Convert(CUser* user,
                                  arad::SigAradAvatarConvert* sig)
{
    CItem* item = G_CDataManager()->find_item(sig->m_itemId);
    if (item == 0 || !item->IsAvatarItem())
    {
        ARAD::PRINT_ERROR::find_item("AVATAR CONVERT", user, sig->m_itemId);
        return false;
    }

    int grade = item->get_grade();
    if (item->GetItemType() == 9)
        grade = 3;

    unsigned int selectIdx = sig->m_selectNo;
    ARAD::SCRIPT::AvatarConvert::ConvertResult* result =
        getConvertResult(grade);
    if (result->m_field20 != true)
    {
        // upper_bound(cumWeights, rand(totalWeight)) - begin
        // P0/P183：ORIG 无 cw 引用局部——每处使用点内联 result->m_cumWeights。
        // ORIG 亦无 count 局部：`mov %eax,-0x40(%ebp)` 直接写回 selectIdx。
        int randVal = get_rand_int(result->m_totalWeight);
        std::vector<int>::iterator up =
            std::upper_bound(result->m_cumWeights.begin(),
                             result->m_cumWeights.end(), randVal);
        selectIdx = (unsigned int)(up - result->m_cumWeights.begin());
    }
    // P157：ORIG 0x08192888/0x081928a5 每字段重调 operator[]（无 info 引用缓存），
    // 且后续统一复用已写回的 sig->m_resultItemId。
    sig->m_resultItemId = result->m_infos[selectIdx].m_key;
    sig->m_resultCount = result->m_infos[selectIdx].m_count;

    CItem* resultItem = G_CDataManager()->find_item(sig->m_resultItemId);
    if (resultItem == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x11);
        ARAD::PRINT_ERROR::find_item("AVATAR CONVERT", user, sig->m_itemId);
        return false;
    }

    // ORIG 0x08192919-0x0819294e：newItem 先构造并写 m_addInfo（+0x2），
    // stackable 校验通过后才写 m_addInfo2（+0x7，真语义 bug：原实现漏写两字段）。
    Inven_Item newItem;
    newItem.m_addInfo = sig->m_resultItemId;
    // 结果物品入包（stackable 校验 → make_item → tryInsertItemIntoInventory）
    if (!resultItem->is_stackable())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x11);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x322,
            "AVATAR CONVERT : is_stackable() fail [item_id:%d] [charac_no:%d]",
            sig->m_resultItemId, (unsigned int)user->getCurCharacNo());
        return false;
    }
    newItem.m_addInfo2 = sig->m_resultCount;
    resultItem->make_item(newItem);          // vtable slot2
    // P0/P160：ORIG 无 invW 具名局部——5 处使用点各自重新取
    // user->getCurCharacInvenW()（p167 差集 MISSING×4）。
    int insertSlot =
        user->getCurCharacInvenW()->tryInsertItemIntoInventory(newItem);
    if (insertSlot < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 4);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x329,
            "AVATAR CONVERT : tryInsertItemIntoInventory() fail [item_id:%d] [charac_no:%d]",
            sig->m_resultItemId, (unsigned int)user->getCurCharacNo());
        return false;
    }
    // 释放原 avatar（type 2 槽删除 → UnRegistItem → SendAvatarEvent → DB history）
    // ORIG 0x08192b14/0x08192b28：读路径走 const 版
    // getCurCharacInvenR()->GetInvenRef（p167 MISSING const 版两符号）。
    const Inven_Item* oldRef =
        user->getCurCharacInvenR()->GetInvenRef(2, (int)sig->m_slot);
    unsigned int uiId = oldRef->m_addInfo2;
    if (user->getCurCharacInvenW()->delete_item(INVEN_TYPE(2),
                                                (int)sig->m_slot, 1,
                                                eItemDelReason(6), true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x16);
        ARAD::PRINT_ERROR::delete_item("AVATAR CONVERT", user, sig->m_slot);
        return false;
    }
    if (!user->getCurCharacInvenW()->GetAvatarItemMgrW()
             ->UnRegistItem((int)uiId))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x16);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x33c,
            "AVATAR CONVERT : UnRegistItem() fail [ui_id:%d] [charac_no:%d]",
            uiId, (unsigned int)user->getCurCharacNo());
        return false;
    }
    user->getCurCharacInvenW()->SendAvatarEvent(0x36, (int)uiId,
                                                (int)sig->m_slot + 10, 5, 0);
    DB_UpdateAvatarHistory::makeRequest(user->getCurCharacNo(), (int)uiId,
                                        (char*)"AVATAR CONVERT");

    // 结果物品经事件发放 + 通知包
    std::vector<std::pair<int, int> > items;
    std::vector<int> outSlots;
    // ORIG 0x08192cf2：make_pair<unsigned long&, short&>（两成员均为左值，
    // 字段 m_resultItemId 为 unsigned long）→ pair<int,int> 转换构造。
    items.push_back(std::make_pair(sig->m_resultItemId, sig->m_resultCount));
    int retSlots = user->getCurCharacInvenW()->insert_event_items(
        items, outSlots, eMoneyAddReason(0), eItemAddReason(10), false,
        "AVATAR CONVERT", "AVATAR CONVERT");

    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x24f);
    packet.put_byte(1);
    packet.put_short(sig->m_slot);
    packet.put_short(retSlots);
    // ORIG 用 const_iterator（p167 MISSING ppEi/deEv 的 const 族）+
    // 语句位后置 ++（4.4 保留 ppEi）。
    for (std::vector<int>::const_iterator s = outSlots.begin();
         s != outSlots.end(); s++)
    {
        int slotIdx = *s;
        packet.put_short(slotIdx);
        s++;
        int slotOut = *s;
        packet.put_int(slotOut);
        s++;
        int extra = *s;
        packet.put_int(extra);
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x368, 0);
        trace("AVATAR CONVERT : success [slot:%d] [selectNo:%d] [resultIdx:%d] "
              "[charac_no:%d]",
              (int)sig->m_slot, (int)sig->m_selectNo, slotOut,
              (unsigned int)user->getCurCharacNo());
    }
    packet.finalize(true);
    user->Send(packet);
    // b28c-9 已把原成员 m_script 改为首个非主基类（ARAD::SCRIPT::AvatarRecharge
    // 落 +0x04），此处改走基类引用（ORIG 0x08190200 布局：+0x00 vptr / +0x04 脚本体）。
    bool isRechargeStone =
        static_cast<ARAD::SCRIPT::AvatarRecharge&>(
            *ARAD::Singleton<AvatarRechargeServer>::Get())
            .IsEmblemRechargeStone(sig->m_resultItemId);
    StatisticsAvatarDisjoint(item, isRechargeStone ? 1 : 0);

    // ORIG 0x0819301d：统计实参复用 items（无独立 created vector——
    // p167 EXTRA 的 vector<pair> C1Ev/D1Ev + push_back 即该多建对象）。
    StatisticsEmblemCreate(items);
    return true;
}

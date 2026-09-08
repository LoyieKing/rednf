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
AvatarConvert::ConvertResult::ConvertResult()
    : m_infos(), m_cumWeights()
{
}

AvatarConvert::ConvertResult::~ConvertResult()
{
}

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
// [b28d-D33 P216/N83 封顶] ORIG 0x088b1b36：189 vs 186 条；p167 差集 =
// _ZdlPv/eq MISSING vs _ZnwjPv/_ZdlPvS_/ne EXTRA（placement new 前端内联 vs
// 4.4 extern call，N83；P216 throw() 边界）+ 帧 0x5c vs 0x6c。
int AvatarConvert::loadScript()
{
    if (!loadRDARScriptFile("Etc/EmblemRenewal/", "avatar_convert_jpn.etc"))
        return -1;

    std::string token;
    bool isType = false;
    bool isInt = false;
    while (ScanType(token, true))
    {
        int totalWeight = 0;   // ORIG -0x40：每条记录重置
        // ORIG：operator new(0x24) → memset(0) 0x24 字节（标量预清零：无 info 条目
        // 的空记录 back()/m_totalWeight 读 0）→ ConvertResult::C1。
        ConvertResult* convertResult =
            static_cast<ConvertResult*>(::operator new(sizeof(ConvertResult)));
        memset(convertResult, 0, sizeof(ConvertResult));
        new (convertResult) ConvertResult();
        convertResult->m_grade = ScanInt((bool*)0);
        convertResult->m_field20 = (ScanInt((bool*)0) == 1);
        for (;;)
        {
            int key = ScanTypeOrInt(isType, token, isInt, true);
            if (isType || !isInt)
                break;   // TYPE → 插入分支；EOF/非法 → 丢弃该记录（由 isType 区分）
            GetInfo info;
            info.m_key = key;
            info.m_itemIndex = static_cast<short>(ScanInt((bool*)0));
            info.m_count = static_cast<short>(ScanInt((bool*)0));
            convertResult->m_infos.push_back(info);
            totalWeight += info.m_count;
            convertResult->m_cumWeights.push_back(totalWeight);
        }
        if (isType)
        {
            std::map<int, ConvertResult*>::iterator endIt = m_convertMap.end();
            if (m_convertMap.find(convertResult->m_grade) != endIt)
            {
                printf("%s duplicate grade:%d", token.c_str(), convertResult->m_grade);
                return -2;
            }
            convertResult->m_totalWeight = convertResult->m_cumWeights.back();
            m_convertMap.insert(
                std::map<int, ConvertResult*>::value_type(convertResult->m_grade,
                                                          convertResult));
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
// DB_UpdateAvatarHistory —— ORIG makeRequest T @0x0844536a。
// 权威定义在 CompoundDeathLCB.cpp（LinkClosureB.CompoundDeath 批），
// 此处仅经 DB_UpdateAvatarHistory.h 引用，避免多重定义。
#include "DB_UpdateAvatarHistory.h"

namespace {

// StatisticsEmblemCreate / StatisticsAvatarDisjoint 共用的统计包发送。
CStatisticServerProxy* getStatisticProxy0()
{
    return GlobalData::s_statistic_proxy_mgr->GetServerProxy(
        (ENUM_SERVER_GROUP)0);
}

}  // namespace

    // [b28d-D33 复验] N7 封顶：唯一残差=虚调用 callee 寄存器互换（ORIG
    // 0x0819319b `mov (%eax),%edx; call *%edx` vs 4.4 `mov (%eax),%eax; call *%eax`），
    // 助记符序列/长度全等（N7，探针 /tmp/l22p/vprobe*.cpp）。P282 已核槽位 +0xc
    // 一致，非调用目标还原错。
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

// [b28d-D33 P167/N83/N86 封顶] ORIG 0x081931dc：117 vs 110 条；p167 差集 =
// Packet C1Ev/GetServerProxy MISSING vs memset/helper EXTRA（P124/P83 结构赋值 vs
// memset；helper 已存在的 TU-local 转发）+ 帧 0x74 vs 0x64（N86/N82）。
void AvatarConvertServer::StatisticsEmblemCreate(
    const std::vector<std::pair<int, int> >& created)
{
    unsigned char pkt[0x36];
    std::memset(pkt, 0, sizeof(pkt));   //【ORIG 未清零区域为栈残留；置 0 等价】
    pkt[0] = (unsigned char)(0x17a4 & 0xff);
    pkt[1] = (unsigned char)((0x17a4 >> 8) & 0xff);
    pkt[2] = 0x36;

    if (created.size() > 5)
        return;

    size_t i = 0;
    std::vector<std::pair<int, int> >::const_iterator it = created.begin();
    for (; it != created.end(); ++it)
    {
        const int itemId = it->first;
        const int addInfo = it->second;
        CItem* item = G_CDataManager()->find_item(itemId);
        if (item == 0)
            return;                              // ORIG 中途 return
        if (item->GetItemType() != 0x14)
            continue;                            // 非头像物品不计入
        *reinterpret_cast<int*>(&pkt[0xe + i * 4]) = item->get_grade();
        *reinterpret_cast<int*>(&pkt[0x22 + i * 4]) = addInfo;
        ++i;
        if (i > 4)
            break;                               // ORIG 计数上限 cmp $5
    }
    if (i != created.size())
        return;
    *reinterpret_cast<int*>(&pkt[0xa]) = static_cast<int>(i);
    getStatisticProxy0()->SendPacket(reinterpret_cast<char*>(pkt), 0x36);
}

// [b28d-D33 N82 封顶] ORIG 0x08192212：302 vs 293 条（帧/槽跨版本差）。
bool AvatarConvertServer::CheckCondition(CUser* user,
                                         arad::SigAradAvatarConvert* sig)
{
    // b21 verdict=DIFF（N 残余）：293/302。调用点全等（37=37：getCurCharacInvenR/W/
    //   GetInvenRef/isEmpty/CheckItemLock/find_item/IsAvatarItem/isEnableAvatarFunc/
    //   GetAvatarItemMgrR/GetExpireDate/get_grade/getConvertResult/GetInfo::size/
    //   SendCmdErrorPacket×n/PRINT_ERROR×2/logFormat×n/getCurCharacNo）。语义逐条核对
    //   （槽/ushort 零扩展、m_addInfo!=m_itemId、IsAvatarItem、isEnable 失败、期限检查、
    //   getConvertResult==0、isSelect/selectNo 判定）。残余仅帧差(0x44 vs 0x54→N82 槽号)、
    //   getCurCharacInvenR 前的 level 存 ebx(movzwl) vs cwtl(N 寄存器)、mgr/result 引用局
    //   部占槽。语义完成，byte 层 N 封顶。
    const CInventory* invR = user->getCurCharacInvenR();
    const Inven_Item* ref = invR->GetInvenRef(2, (int)sig->m_slot);
    if ((ref == 0 || ref->isEmpty()))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x16);
        ARAD::PRINT_ERROR::isEmpty("AVATAR CONVERT", user, sig->m_slot);
        return false;
    }
    if ((int)ref->m_addInfo != sig->m_itemId)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x11);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x2b5,
            "AVATAR CONVERT : [item_id1:%d] [item_id2:%d] [charac_no:%d]",
            (int)ref->m_addInfo, sig->m_itemId,
            (unsigned int)user->getCurCharacNo());
        return false;
    }
    if (user->CheckItemLock(2, (int)sig->m_slot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0xd5);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            700,
            "AVATAR CONVERT : lock [item_id1:%d] [item_id2:%d] [charac_no:%d]",
            (int)ref->m_addInfo, sig->m_itemId,
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
    // avatar 到期检查（ORIG getCurCharacInvenW → GetAvatarItemMgrR）
    const WongWork::CAvatarItemMgr* mgr =
        user->getCurCharacInvenW()->GetAvatarItemMgrR();
    if (mgr->GetExpireDate(ref->m_addInfo2) != 0)
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
    // 成功条件：isSelect 或 selectNo < infos.size()
    bool failed = result->m_field20 ||
                  result->m_infos.size() <= (std::vector<ARAD::SCRIPT::AvatarConvert::GetInfo>::size_type)sig->m_selectNo;
    if (failed)
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

// [b28d-D33 N82/N89 封顶] ORIG 0x081926de：609 vs 610 条（超大函数 >500，
// §6.2 三件套口径）。语义等价。
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
        int randVal = get_rand_int(result->m_totalWeight);
        std::vector<int>& cw = result->m_cumWeights;
        unsigned int count = 0;
        std::vector<int>::iterator up = std::upper_bound(cw.begin(), cw.end(),
                                                         randVal);
        count = (unsigned int)(up - cw.begin());
        selectIdx = count;
    }
    ARAD::SCRIPT::AvatarConvert::GetInfo& info = result->m_infos[selectIdx];
    sig->m_resultItemId = info.m_key;
    sig->m_resultCount = info.m_count;

    CItem* resultItem = G_CDataManager()->find_item(info.m_key);
    if (resultItem == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x11);
        ARAD::PRINT_ERROR::find_item("AVATAR CONVERT", user, sig->m_itemId);
        return false;
    }

    // 结果物品入包（stackable 校验 → make_item → tryInsertItemIntoInventory）
    if (!resultItem->is_stackable())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x11);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x322,
            "AVATAR CONVERT : is_stackable() fail [item_id:%d] [charac_no:%d]",
            info.m_key, (unsigned int)user->getCurCharacNo());
        return false;
    }
    Inven_Item newItem;
    resultItem->make_item(newItem);          // vtable slot2
    CInventory* invW = user->getCurCharacInvenW();
    int insertSlot = invW->tryInsertItemIntoInventory(newItem);
    if (insertSlot < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 4);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x329,
            "AVATAR CONVERT : tryInsertItemIntoInventory() fail [item_id:%d] [charac_no:%d]",
            info.m_key, (unsigned int)user->getCurCharacNo());
        return false;
    }
    // 释放原 avatar（type 2 槽删除 → UnRegistItem → SendAvatarEvent → DB history）
    Inven_Item* oldRef = invW->GetInvenRef(2, (int)sig->m_slot);
    unsigned int uiId = oldRef->m_addInfo2;
    if (invW->delete_item(INVEN_TYPE(2), (int)sig->m_slot, 1,
                          eItemDelReason(6), true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x16);
        ARAD::PRINT_ERROR::delete_item("AVATAR CONVERT", user, sig->m_slot);
        return false;
    }
    WongWork::CAvatarItemMgr* mgr = invW->GetAvatarItemMgrW();
    if (!mgr->UnRegistItem((int)uiId))
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
    invW->SendAvatarEvent(0x36, (int)uiId, (int)sig->m_slot + 10, 5, 0);
    DB_UpdateAvatarHistory::makeRequest(user->getCurCharacNo(), (int)uiId,
                                        (char*)"AVATAR CONVERT");

    // 结果物品经事件发放 + 通知包
    std::vector<std::pair<int, int> > items;
    std::vector<int> outSlots;
    items.push_back(std::make_pair(sig->m_resultItemId,
                                   (int)(short)sig->m_resultCount));
    int retSlots = invW->insert_event_items(items, outSlots,
                                            eMoneyAddReason(0),
                                            eItemAddReason(10), false,
                                            "AVATAR CONVERT", "AVATAR CONVERT");

    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x24f);
    packet.put_byte(1);
    packet.put_short(sig->m_slot);
    packet.put_short(retSlots);
    for (std::vector<int>::iterator s = outSlots.begin();
         s != outSlots.end(); ++s)
    {
        int slotIdx = *s;
        packet.put_short(slotIdx);
        ++s;
        int slotOut = *s;
        packet.put_int(slotOut);
        ++s;
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

    // 统计合成的 [resultItemId, addInfo] 对
    std::vector<std::pair<int, int> > created;
    created.push_back(std::make_pair((unsigned long)sig->m_resultItemId,
                                     (int)sig->m_resultCount));
    StatisticsEmblemCreate(created);
    return true;
}

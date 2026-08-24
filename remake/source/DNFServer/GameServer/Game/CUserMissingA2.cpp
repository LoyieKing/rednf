// ============================================================================
// CUserMissingA2.cpp —— CUser 未定义方法补齐（独占 TU；不改 CUser.cpp/CMake）。
//
// 逐份完整读取报告（## 汇编 + ## 反编译 C）后转写：
//   1. CUser::GetCharacHotKeyType            docs/.../CUser/GetCharacHotKeyType.md
//                                            ORIG T 0x08692ef2 _ZN5CUser19GetCharacHotKeyTypeEi
//   2. CUser::CheckLoadHotKeyCashedCharacType .../CUser/CheckLoadHotKeyCashedCharacType.md
//                                            ORIG T 0x08692e9c _ZN5CUser31CheckLoadHotKeyCashedCharacTypeEii
//   3. CUser::CalcMercenaryAvatarBonus       .../CUser/CalcMercenaryAvatarBonus.md
//                                            ORIG T 0x0868ea68 _ZNK5CUser24CalcMercenaryAvatarBonusEv
//   4. CUser::setAvatarTypeBonus             .../CUser/setAvatarTypeBonus.md
//                                            ORIG T 0x0868ebaa _ZN5CUser18setAvatarTypeBonusEc
//   5. UserMercenaryInfoMgr::FindIndexByCharacNo（4 的依赖，ORIG T 0x081b42ae）
//   6. UserMercenaryInfoMgr::CheckCharacIndex    （4 的依赖，ORIG T 0x081b4262）
//   7. CHARAC_LOAD_MERCENARY::Reset          .../CHARAC_LOAD_MERCENARY/Reset.md
//                                            ORIG T 0x081b410a（布局自证）
//   8. CFatigueBatteryHandle::ActiveFatigueBattery_EndLevel
//                                            .../CFatigueBatteryHandle/ActiveFatigueBattery_EndLevel.md
//                                            ORIG T 0x084a81f4
//   9. DB_LoadCharacHotKeyOption::makeRequest .../DB_LoadCharacHotKeyOption/makeRequest.md
//                                            ORIG T 0x0842f710
//
// eventDeliveryBuyItem（ORIG T 0x0866463c）已由 CUserDepsB.cpp 定义，本 TU 不重复。
// ============================================================================

#include <string.h>  // memset

#include "CUser.h"                  // CUser/UserMercenaryInfoMgr/CHARAC_LOAD_MERCENARY/_Charac_info
#include "CInventory.h"             // CInventory::GetInvenRef(int,int) const、INVEN_TYPE_INVEN=0
#include "CDataManager.h"           // G_CDataManager / find_item(int) const
#include "CEquipItem.h"             // CEquipItem 完整类型（dynamic_cast 目标 + IsAvatarItem 槽 4）
#include "CFatigueBatteryHandle.h"  // 本类声明
#include "GameTypes.h"              // cMyTrace(name, line, flag) + operator()
#include "GlobalData.h"             // s_stream_pool / s_msgq_mgr
#include "StreamPool.h"             // StreamPool::Acquire(char const*, int)
#include "CStreamGuard.h"           // CStreamGuard RAII（operator*/operator-> 返回 this）
#include "MsgQueueMgr.h"            // MsgQueueMgr::put(QUEUE_IDX, CStreamGuard&)
#include "SigTypes.h"               // SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION
#include "DB_LoadCharacHotKeyOption.h"

// ----------------------------------------------------------------------------
// 1. ORIG T 0x08692ef2 sz 0x17。
// cmpl $0xa,param; jne→0；相等→1。返回值 eax 全宽（头文件声明 int，调用点
// LoadCharacTypeHotKey 以 mov %eax 全宽取值后截 byte；语义 characJob==10）。
// ----------------------------------------------------------------------------
int CUser::GetCharacHotKeyType(int characJob)
{
    if (characJob == 10)
        return 1;
    return 0;
}

// ----------------------------------------------------------------------------
// 2. ORIG T 0x08692e9c sz 0x55。
// cashedCharacJob==-1 → 1（首次加载放行）；否则两次调 GetCharacHotKeyType 比较，
// 相同→0、不同→1（类型变化才允许重新加载热键）。
// ----------------------------------------------------------------------------
bool CUser::CheckLoadHotKeyCashedCharacType(int cashedCharacJob, int characJob)
{
    if (cashedCharacJob == -1)
        return true;

    int cachedType = GetCharacHotKeyType(cashedCharacJob);
    int currentType = GetCharacHotKeyType(characJob);
    if (cachedType == currentType)
        return false;
    return true;
}

// ----------------------------------------------------------------------------
// 3. ORIG T 0x0868ea68 sz 0x141。雇佣兵 avatar 等级加成 = 全身 avatar 装备的最低
// 品阶：初值 10；getCurCharacR/getCurCharacInvenR 为空→0；否则 i∈[0,7] 遍历
// GetInvenRef(INVEN, i)：空引用/find_item 失败/dynamic_cast<CEquipItem*> 失败/
// IsAvatarItem()==0（vtable+0x10 槽 4，两处 dtor 后第 5 槽）任一即返 0；
// 否则取 min(get_grade)；循环后 >3 归零。
// ----------------------------------------------------------------------------
int CUser::CalcMercenaryAvatarBonus() const
{
    int best = 10;

    if (getCurCharacR() == 0)
        return 0;

    const CInventory* inven = getCurCharacInvenR();
    if (inven == 0)
        return 0;

    for (int i = 0; i < 8; ++i)
    {
        const Inven_Item* ref = inven->GetInvenRef(INVEN_TYPE_INVEN, i);
        if (ref == 0)
            return 0;

        // *(int*)(ref+2)：m_addInfo（item idx）
        CEquipItem* equip;
        if (CItem* item = G_CDataManager()->find_item(ref->m_addInfo))
            equip = dynamic_cast<CEquipItem*>(item);
        else
            equip = 0;
        if (equip->IsAvatarItem())
        {
            if (equip->get_grade() < best)
                best = equip->get_grade();
        }
        else
        {
            return 0;
        }
    }

    if (3 < best)
        best = 0;

    return best;
}

// ----------------------------------------------------------------------------
// 4. ORIG T 0x0868ebaa sz 0xb1。
// bonus∈[0,3] 且 getCurCharacR() 非空时：characNo=*getCurCharacR()（+0 dword）→
// m_mercenaryMgr.FindIndexByCharacNo → !=-1 且 CheckCharacIndex((uchar)idx) →
// vector 元素 +0x32（m_avatarBonus）写 bonus。
// ----------------------------------------------------------------------------
void CUser::setAvatarTypeBonus(char bonus)
{
    if (bonus < 0 || 3 < bonus)
        return;

    if (getCurCharacR() == 0)
        return;

    unsigned int characNo =
        static_cast<unsigned int>(getCurCharacR()->m_characNo);
    int idx = m_mercenaryMgr.FindIndexByCharacNo(characNo);
    if (idx == -1)
        return;
    if (!m_mercenaryMgr.CheckCharacIndex(static_cast<unsigned char>(idx)))
        return;

    m_mercenaryMgr.m_characs[idx].m_avatarBonus =
        static_cast<unsigned char>(bonus);
}

// ----------------------------------------------------------------------------
// 5. ORIG T 0x081b42ae sz 0x51。按 m_characs[i].m_characNo（首 dword 无符号比较）
// 线性查找；未命中返回 0xffffffff。循环条件 seta（size()>idx 无符号大于）。
// ----------------------------------------------------------------------------
unsigned int UserMercenaryInfoMgr::FindIndexByCharacNo(unsigned int characNo) const
{
    size_t idx = 0;
    while (m_characs.size() > idx)
    {
        if (static_cast<unsigned int>(m_characs[idx].m_characNo) == characNo)
            return static_cast<unsigned int>(idx);
        ++idx;
    }
    return 0xffffffffU;
}

// ----------------------------------------------------------------------------
// 6. ORIG T 0x081b4262 sz 0x4b。empty→0；size>=index（jae 无符号）→1；
// size<index→0。注意 ORIG 边界为 index<=size（Ghidra 同口径），实际调用点
// FindIndexByCharacNo 命中下标恒 <size，语义等价安全。
// ----------------------------------------------------------------------------
bool UserMercenaryInfoMgr::CheckCharacIndex(unsigned char index) const
{
    // ORIG 形态：empty→flag=1；!empty 且 index<=size→flag=0；否则 flag=1；返 !flag。
    bool bInvalid;
    if (m_characs.empty())
        bInvalid = true;
    else if (static_cast<unsigned int>(index) <= m_characs.size())
        bInvalid = false;
    else
        bInvalid = true;
    return !bInvalid;
}

// ----------------------------------------------------------------------------
// 7. ORIG T 0x081b410a sz 0x85。字段清零/初始化顺序逐条对应汇编：
// +0 dword=0、+4 word=0、memset(+6,0,0x1e)、memset(+0x34,0,0x20)、
// +0x24 byte=0、+0x28 dword=0、+0x2c dword=0、+0x30=0xff、+0x31=0xff、+0x32=0xa。
// ----------------------------------------------------------------------------
void CHARAC_LOAD_MERCENARY::Reset()
{
    m_characNo = 0;
    m_level = 0;
    memset(m_name, 0, sizeof(m_name));                // 0x1e
    memset(m_avatarItems, 0, sizeof(m_avatarItems));  // 0x20
    m_field24 = 0;
    m_field28 = 0;
    m_field2c = 0;
    m_field30 = 0xff;
    m_field31 = 0xff;
    m_avatarBonus = 0xa;
}

// ----------------------------------------------------------------------------
// 8. ORIG T 0x084a81f4 sz 0xa9。
// used=(int)(short)user->GetFatigueBatteryCharging()（cwtl 符号扩展）；result=used；
// 二次读取与 result 不等时经 cMyTrace 记录 "FATIGUE BATTERY ACTIVE ERR :
// char no(%d), usedFatigue(%d)" 并重取；返回 result 截 short。
// ORIG 另有 G_CDataManager()+0x68 死存储（结果从不读取），按原样保留以对齐栈形。
// ----------------------------------------------------------------------------
short CFatigueBatteryHandle::ActiveFatigueBattery_EndLevel(CUser* user)
{
    int result = 0;
    int used =
        static_cast<int>(static_cast<short>(user->GetFatigueBatteryCharging()));

    // ORIG 死存储：call G_CDataManager(); add $0x68,%eax; mov %eax,-0xc(%ebp)，
    // 之后从未读取——保留以维持指令形态。
    stFatigueBatteryConstantTable_t* unusedTable =
        reinterpret_cast<stFatigueBatteryConstantTable_t*>(
            reinterpret_cast<unsigned char*>(G_CDataManager()) + 0x68);
    (void)unusedTable;

    result = used;
    if (static_cast<int>(static_cast<short>(user->GetFatigueBatteryCharging())) !=
        static_cast<short>(result))
    {
        cMyTrace trace(
            "short int CFatigueBatteryHandle::ActiveFatigueBattery_EndLevel(CUser*)",
            0x77, 0);
        trace("FATIGUE BATTERY ACTIVE ERR : char no(%d), usedFatigue(%d)",
              user->getCurCharacNo(), result);
        result = static_cast<int>(
            static_cast<short>(user->GetFatigueBatteryCharging()));
    }
    return static_cast<short>(result);
}

// ----------------------------------------------------------------------------
// 9. ORIG T 0x0842f710 sz 0xe3。DB 请求入队：
// Acquire("DBThread.cpp",0x749a) → CStreamGuard(stream,true) → opcode 800(0x320)
// → uid(int) → GetInBuffer<SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION> 写 +0 accId(uint)、
// +4 hotkeyType(int) → put(DB_Q=2, guard)。
// ----------------------------------------------------------------------------
void DB_LoadCharacHotKeyOption::makeRequest(int uid, unsigned int accId,
                                            int hotkeyType)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x749a), true);
    guard.operator*()->operator<<(800);   // SIG_LOAD_CHARAC_HOTKEY_OPTION opcode
    guard.operator*()->operator<<(uid);

    struct View
    {
        unsigned int m_accId;             // +0x00（dispatch 作 SQL where 键）
        int m_hotkeyType;                 // +0x04
    };
    View* v = reinterpret_cast<View*>(
        guard.operator->()->GetInBuffer<SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION>());
    v->m_accId = accId;
    v->m_hotkeyType = hotkeyType;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

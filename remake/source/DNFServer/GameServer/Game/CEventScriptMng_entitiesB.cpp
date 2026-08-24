// df_game_r Game/ EventClassify 第二组实体实现（group 2，FixEventEntitiesB）。
// 覆盖 ctor/dtor/脚本导入路径方法（set_event_data / get_db_table_name）。
// 每个实体在 CEventEntity（sizeof 0x1c）之上追加绑定脚本指针 m_script1c(+0x1c)，
// sizeof = 0x20。get_db_table_name 为虚方法（ORIG 虚表 slot 8 / +0x20）。
#include "CEventScriptMng.h"

namespace EventClassify
{

// ============ CFatigueQuantity ============
// ORIG ctor 0x08111030 W / D1 0x08114aac / D0 0x08114adc / sizeof 0x20。
CFatigueQuantity::CFatigueQuantity()
    : CEventEntity(), m_script1c(0)
{
}

CFatigueQuantity::~CFatigueQuantity()
{
}

// ORIG 0x0810f95e：虚调度 set_event_record_type（vtable+0x18）写 m_recordType=key，
// 然后把绑定脚本存到 +0x1c。
void CFatigueQuantity::set_event_data(int key, InGameEventScript* script)
{
    set_event_record_type(key);
    m_script1c = script;
}

// ORIG 0x0810f988：return *(int*)(this+0x1c) + 8，即绑定脚本的 db_table_name 字符串
// （InGameEventScript 的 m_dbTableName 位于 +0x08）。虚表 slot 8。
char* CFatigueQuantity::get_db_table_name()
{
    return reinterpret_cast<char*>(m_script1c) + 8;
}

// ============ CExchangeRandomItemReward ============
// ORIG ctor 0x08110ff8 W / D1 0x08114b48 / D0 0x08114b78 / sizeof 0x20。
CExchangeRandomItemReward::CExchangeRandomItemReward()
    : CEventEntity(), m_script1c(0)
{
}

CExchangeRandomItemReward::~CExchangeRandomItemReward()
{
}

// ORIG 0x0810eb06：同 CFatigueQuantity 模式。
void CExchangeRandomItemReward::set_event_data(int key, InGameEventScript* script)
{
    set_event_record_type(key);
    m_script1c = script;
}

// ============ CAvatarDisjointRandomReward ============
// ORIG ctor 0x08111014 W / D1 0x08114afa / D0 0x08114b2a / sizeof 0x20。
CAvatarDisjointRandomReward::CAvatarDisjointRandomReward()
    : CEventEntity(), m_script1c(0)
{
}

CAvatarDisjointRandomReward::~CAvatarDisjointRandomReward()
{
}

// ORIG 0x0810f37a：同 CFatigueQuantity 模式。
void CAvatarDisjointRandomReward::set_event_data(int key, InGameEventScript* script)
{
    set_event_record_type(key);
    m_script1c = script;
}

// ============ CObjectBringUp ============
// ORIG ctor 0x08111068 W / D1 0x08114a10 / D0 0x08114a40 / sizeof 0x20。
CObjectBringUp::CObjectBringUp()
    : CEventEntity(), m_script1c(0)
{
}

CObjectBringUp::~CObjectBringUp()
{
}

// ORIG 0x0810fcb2：同 CFatigueQuantity 模式。
void CObjectBringUp::set_event_data(int key, InGameEventScript* script)
{
    set_event_record_type(key);
    m_script1c = script;
}

// ORIG 0x0810fcdc：return *(int*)(this+0x1c) + 8。虚表 slot 8。
char* CObjectBringUp::get_db_table_name()
{
    return reinterpret_cast<char*>(m_script1c) + 8;
}

// ============ CPurchaseCashItemBonus ============
// ORIG ctor 0x0811104c W / D1 0x08114a5e / D0 0x08114a8e / sizeof 0x20。
CPurchaseCashItemBonus::CPurchaseCashItemBonus()
    : CEventEntity(), m_script1c(0)
{
}

CPurchaseCashItemBonus::~CPurchaseCashItemBonus()
{
}

// ORIG 0x0810f996：同 CFatigueQuantity 模式。
void CPurchaseCashItemBonus::set_event_data(int key, InGameEventScript* script)
{
    set_event_record_type(key);
    m_script1c = script;
}

// ORIG 0x0810f9c0：return *(int*)(this+0x1c) + 8。虚表 slot 8。
char* CPurchaseCashItemBonus::get_db_table_name()
{
    return reinterpret_cast<char*>(m_script1c) + 8;
}

// ============ CAccountFirstLoginDaily ============
// 派生自 CAccountFirstLogin（group1，FixEventEntitiesA）。
// ORIG ctor 0x0811110a W / D1 0x08114c80 / D0 0x08114cb0。
// 不新增成员，+0x1c 继承自 CAccountFirstLogin。
CAccountFirstLoginDaily::CAccountFirstLoginDaily()
    : CAccountFirstLogin()
{
}

CAccountFirstLoginDaily::~CAccountFirstLoginDaily()
{
}

// ORIG 0x0810dfc6：直接委托 CAccountFirstLogin::set_event_data(this, key, script)。
void CAccountFirstLoginDaily::set_event_data(int key, InGameEventScript* script)
{
    CAccountFirstLogin::set_event_data(key, script);
}

}  // namespace EventClassify

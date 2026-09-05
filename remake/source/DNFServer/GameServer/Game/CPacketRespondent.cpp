// ============================================================================
// df_game_r 还原 —— user_creature::CPacketRespondent。
// 逐方法依据 ORIG objdump 反汇编转写（地址见 CPacketRespondent.h 注释；
// ORIG 全部为 W 内联符号，本 TU 提供强定义）。基类 PacketGuard 由
// ctor/dtor 调 _ZN11PacketGuardC1/D1Ev 实证（0x0834032a/0x0834036a），
// 非直接 InterfacePacketBuf（InterfacePacketBuf 子对象经 PacketGuard 仍在 +0）。
// ============================================================================
#include <string.h>

#include "CPacketRespondent.h"

#include "CUser.h"
#include "CUserCharacInfo.h"   // getCurCharacVill
#include "GameWorld.h"         // G_GameWorld/send_all/send_to_area/getUserVillage
#include "Village.h"           // Village::get_gate_area
#include "LogManager.h"        // cMyTrace
#include "CGameManager.h"      // user_creature::CCreature 访问器
#include "CInventory.h"        // user_creature::CCreatureItem

namespace user_creature
{

// ORIG C1 @ 0x0834032a：PacketGuard 基类构造后依次写
// +0x0c=user、+0x10=header、+0x14=subHeader、+0x18=0、+0x1c(byte)=0。
CPacketRespondent::CPacketRespondent(CUser* user, int header, int subHeader)
    : PacketGuard()
{
    m_user = user;
    m_header = header;
    m_subHeader = subHeader;
    m_field18 = 0;
    m_autoSend = 0;
}

// ORIG D1 @ 0x0834036a：m_autoSend 非零则补发一次 Send，随后 ~PacketGuard()。
CPacketRespondent::~CPacketRespondent()
{
    if (m_autoSend != 0)
        Send();
}

// ORIG finalize @ 0x083403b8：+0x1c=1 后转发 InterfacePacketBuf::finalize(bool)。
void CPacketRespondent::finalize(bool encryptRequired)
{
    m_autoSend = 1;
    InterfacePacketBuf::finalize(encryptRequired);
}

// ORIG Send @ 0x083403e0：+0x1c=0；CUser::Send(*this) 结果透传（int）。
int CPacketRespondent::Send()
{
    m_autoSend = 0;
    return m_user->Send(*this);
}

// ORIG Broadcast @ 0x08340404：
//   +0x1c=0；vill==0 && area==0 → 返回 0；
//   IsGateArea(m_user, area) → m_user->Send(*this)；
//   否则 G_GameWorld()->send_to_area(vill, area, *this)；返回 1。
int CPacketRespondent::Broadcast(int vill, int area)
{
    m_autoSend = 0;
    if (vill == 0 && area == 0)
        return 0;  // ORIG 0x834041e mov $0,%eax（int 返回，非 %al）
    if (IsGateArea(m_user, area))
        m_user->Send(*this);
    else
        G_GameWorld()->send_to_area(vill, area, *this);
    return 1;      // ORIG 0x834047e mov $0x1,%eax
}

// ORIG SendAll @ 0x0834048a：+0x1c=0；G_GameWorld()->send_all(*this)；返回 1。
bool CPacketRespondent::SendAll()
{
    m_autoSend = 0;
    G_GameWorld()->send_all(*this);
    return true;
}

// ORIG IsGateArea @ 0x08340256：GameWorld/村空时按 __PRETTY_FUNCTION__ 打点并返 0；
// 否则比较 Village::get_gate_area(village) == area。
bool CPacketRespondent::IsGateArea(CUser* user, int area)
{
    GameWorld* world = G_GameWorld();
    if (world == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x281, 5);
        trace("G_GameWorld call failed");
        return false;
    }
    Village* village = world->getUserVillage(user);
    if (village == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x287, 5);
        trace("getUserVillage call failed");
        return false;
    }
    return village->get_gate_area() == area;
}

// ORIG SendAccordingToPlace @ 0x083404b8：
// state==5 直发本人；否则 area=get_area(false)、vill=(signed char)getCurCharacVill()
// 后 Broadcast(vill, area)。返回值未使用（void）。
void CPacketRespondent::SendAccordingToPlace()
{
    m_autoSend = 0;
    if (m_user->get_state() == 5)
    {
        m_user->Send(*this);
    }
    else
    {
        int area = m_user->get_area(false);
        int vill = (int)(signed char)m_user->getCurCharacVill();
        Broadcast(vill, area);
    }
}

// ORIG SetError @ 0x08340538：clear → put_header(+0x10,+0x14) → put_byte(0)
// → put_byte(errCode) → finalize(true)。
void CPacketRespondent::SetError(int errorCode)
{
    clear();
    put_header(m_header, m_subHeader);
    put_byte(0);
    put_byte(errorCode);
    finalize(true);
}

// ORIG SetOk @ 0x083405a2：clear → put_header(+0x10,+0x14) → put_byte(1)
// → finalize(true)。
void CPacketRespondent::SetOk()
{
    clear();
    put_header(m_header, m_subHeader);
    put_byte(1);
    finalize(true);
}

// ORIG MakeCmdpacketRenameCreature @ 0x083405fa：clear → put_header(1,0x67)
// → put_byte(1) → put_short(param1) → put_byte(param2) → finalize(true)。
void CPacketRespondent::MakeCmdpacketRenameCreature(int param1, int param2)
{
    clear();
    put_header(1, 0x67);
    put_byte(1);
    put_short(param1);
    put_byte(param2);
    finalize(true);
}

// ORIG MakeNotipacketRenameCreature @ 0x08340672：clear → put_header(0,0x65)
// → put_short(uid 低 16 位) → put_int(len) → put_str(name,len) → finalize(true)。
void CPacketRespondent::MakeNotipacketRenameCreature(const char* name, int len)
{
    clear();
    put_header(0, 0x65);
    unsigned short uid16 = (unsigned short)m_user->get_unique_id();
    put_short((int)uid16);
    put_int(len);
    put_str(name, len);
    finalize(true);
}

// ORIG MakeNotipacketDiedCreature @ 0x083406fe：clear → put_header(0,0x64)
// → put_short(uid16) → finalize(true)。
void CPacketRespondent::MakeNotipacketDiedCreature()
{
    clear();
    put_header(0, 0x64);
    unsigned short uid16 = (unsigned short)m_user->get_unique_id();
    put_short((int)uid16);
    finalize(true);
}

// ORIG MakeNotipacketGainExpCreature @ 0x08340760：clear → put_header(0,0x66)
// → put_byte(level)；
//   growFlag：put_byte(1)、put_int((int16)(exp>>16))、put_byte(10)、
//             put_byte(exp 低字节)；
//   else    ：put_byte(0)、put_int(exp)；
// 最后 finalize(true)。
void CPacketRespondent::MakeNotipacketGainExpCreature(int exp, int level,
                                                      bool growFlag)
{
    clear();
    put_header(0, 0x66);
    put_byte(level);
    if (growFlag)
    {
        put_byte(1);
        // movzwl 高半字 + cwtl：取 exp bit[16..31] 并作 16 位有符号扩展。
        unsigned short high16 =
            (unsigned short)(((unsigned int)exp >> 16) & 0xffff);
        put_int((int)(short)high16);
        put_byte(0xa);
        put_byte((int)((unsigned int)exp & 0xff));
    }
    else
    {
        put_byte(0);
        put_int(exp);
    }
    finalize(true);
}

// ORIG MakeNotipacketEvoluteCreature @ 0x08340842：clear → put_header(0,0x6a)
// → put_byte(slot) → put_short(uid16) → finalize(true)。
void CPacketRespondent::MakeNotipacketEvoluteCreature(int slot)
{
    clear();
    put_header(0, 0x6a);
    put_byte(slot);
    unsigned short uid16 = (unsigned short)m_user->get_unique_id();
    put_short((int)uid16);
    finalize(true);
}

// ORIG MakeNotipacketRevivalCreature @ 0x083408b6：clear → put_header(0,0x6b)
// → put_short(uid16) → finalize(true)。
void CPacketRespondent::MakeNotipacketRevivalCreature()
{
    clear();
    put_header(0, 0x6b);
    unsigned short uid16 = (unsigned short)m_user->get_unique_id();
    put_short((int)uid16);
    finalize(true);
}

// ORIG MakeNotipacketCreatureState @ 0x08340918：clear → put_header(0,0x67)
// → put_int(uid) → put_int(state) → finalize(true)。
void CPacketRespondent::MakeNotipacketCreatureState(int uid, int state)
{
    clear();
    put_header(0, 0x67);
    put_int(uid);
    put_int(state);
    finalize(true);
}

// ORIG MakeNotipacketResponseCreature @ 0x0834097e：clear → put_header(0,0x68)
// → put_short(uid16) → finalize(true)。
void CPacketRespondent::MakeNotipacketResponseCreature()
{
    clear();
    put_header(0, 0x68);
    unsigned short uid16 = (unsigned short)m_user->get_unique_id();
    put_short((int)uid16);
    finalize(true);
}

// ORIG MakeNotipacketCreatureItemList @ 0x083409e0：
//   clear → put_header(0,0x69)；index=get_index() 后 put_byte(0) 占位计数；
//   遍历 map 值（post-increment 迭代）：item 空或 GetCreatureItemType()!=1 跳过；
//   slot != -1 且 GetSlotNo()!=slot 跳过；否则依次
//     put_int(GetUid)、put_byte(CCreature::GetStomach)、
//     IsGrowCreature ? [put_byte(1), put_int((int16)(exp>>16)), put_byte(10),
//                       put_byte(exp&0xff)]
//                   : [put_byte(0), put_int(GetExp)]、
//     put_byte(GetLevel)、strlen(name)+put_int+put_str、put_byte(IsNoCharge)，
//     计数 ++；
//   循环结束 put_byte(index, count) 回填占位字节，finalize(true)。
//
// 注：ORIG 对 map 元素（CCreatureItem*）直接以 CCreature 访问器调用
// （GetStomach/IsGrowCreature/GetExp/GetLevel/GetName/IsNoCharge），单继承布局
// 同址；镜像类型间用 reinterpret_cast 保持同一调用符号与语义。
void CPacketRespondent::MakeNotipacketCreatureItemList(
    const std::map<int, CCreatureItem*>* items, int slot)
{
    clear();
    put_header(0, 0x69);

    int count = 0;
    int index = get_index();
    put_byte(0);  // 计数占位

    for (std::map<int, CCreatureItem*>::const_iterator it = items->begin();
         it != items->end(); it++)
    {
        CCreatureItem* item = it->second;
        if (item == 0)
            continue;
        if (item->GetCreatureItemType() != 1)
            continue;
        if (slot != -1 && item->GetSlotNo() != slot)
            continue;

        put_int(item->GetUid());

        // 同一对象继续按 CCreature 语义读取（ORIG 直呼 CCreature 访问器）。
        CCreature* creature = reinterpret_cast<CCreature*>(item);

        put_byte(creature->GetStomach());
        if (creature->IsGrowCreature())
        {
            put_byte(1);
            int exp = creature->GetExp();
            unsigned short high16 =
                (unsigned short)(((unsigned int)exp >> 16) & 0xffff);
            put_int((int)(short)high16);
            put_byte(0xa);
            put_byte((int)((unsigned int)exp & 0xff));
        }
        else
        {
            put_byte(0);
            put_int(creature->GetExp());
        }

        put_byte(creature->GetLevel());
        const char* name = creature->GetName();
        int nameLen = (int)strlen(name);
        put_int(nameLen);
        put_str(name, nameLen);
        put_byte((int)(unsigned char)creature->IsNoCharge());

        ++count;
    }

    put_byte(index, count);
    finalize(true);
}

}  // namespace user_creature

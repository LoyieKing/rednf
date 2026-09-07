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
    // ORIG 0x834025c：两指针槽 -0x10/-0xc 先置 0（探针实证：声明+后续独立赋值形才生成
    // 两条 init，`T* p=f()` 合并形缺 init）。条件的 sete/test/je 物化形系 4.1.2 O0
    // 对槽比零的产物（探针实证 4.4-O0 槽比恒出 cmp/jne 直跳，无法源形复现；
    // 不可改 call 内联比较——ORIG cmp 读槽、每 call 仅一次，内联=双调用真 bug）。
    GameWorld* world = 0;
    Village* village = 0;
    world = G_GameWorld();
    if (world == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x281, 5);
        trace("G_GameWorld call failed");
        return false;
    }
    village = world->getUserVillage(user);
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
        // ORIG 0x83404b8：area 结果 `mov %eax,%ebx` 寄存器缓存（跨两次 call 存活），
        // vill 从 getCurCharacVill 返回值直转实参（无槽）。Lane6 探针 /tmp/l6/probe2.cpp：
        // 普通内联实参（R→L 自然求值，get_area 先求值入 ebx）与 ORIG 逐条一致；
        // 语句表达式 `({int r=f(); r;})` 反落槽（h2 形）。实参装载序 area→0x8、vill→0x4。
        Broadcast((int)(signed char)m_user->getCurCharacVill(),
                  m_user->get_area(false));
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
    // ORIG 0x8340738：call 结果寄存器直取 movzwl %ax ⇒ 无具名 uid16 局部（P0 槽律）。
    put_short((int)(unsigned short)m_user->get_unique_id());
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
    // ORIG 0x8340738：call 结果寄存器直取 movzwl %ax ⇒ 无具名 uid16 局部（P0 槽律）。
    put_short((int)(unsigned short)m_user->get_unique_id());
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
        // ORIG 0x83407bd：int 全宽落槽 -0xc 后，movzwl -0xa(%ebp)（同址 u.w[1]）
        // + cwtl + 写回参数槽 0xc ⇒ 推断源为 union 拆半惯用形（P0：shr/独立 16 位槽均形不符）。
        union { int all; unsigned short w[2]; unsigned char b[4]; } u;
        u.all = exp;
        exp = (int)(short)u.w[1];
        put_int(exp);
        put_byte(0xa);
        put_byte((int)u.b[0]);
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
    // ORIG 0x8340738：call 结果寄存器直取 movzwl %ax ⇒ 无具名 uid16 局部（P0 槽律）。
    put_short((int)(unsigned short)m_user->get_unique_id());
    finalize(true);
}

// ORIG MakeNotipacketRevivalCreature @ 0x083408b6：clear → put_header(0,0x6b)
// → put_short(uid16) → finalize(true)。
void CPacketRespondent::MakeNotipacketRevivalCreature()
{
    clear();
    put_header(0, 0x6b);
    // ORIG 0x8340738：call 结果寄存器直取 movzwl %ax ⇒ 无具名 uid16 局部（P0 槽律）。
    put_short((int)(unsigned short)m_user->get_unique_id());
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
    // ORIG 0x8340738：call 结果寄存器直取 movzwl %ax ⇒ 无具名 uid16 局部（P0 槽律）。
    put_short((int)(unsigned short)m_user->get_unique_id());
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
            // ORIG 0x8340b1f：GetExp 结果全宽落槽 -0x28，movzwl -0x26（u.w[1]）+ movswl
            // 直扩进 %edx 作实参（无独立局部、无写回）⇒ union 拆半惯用形（同 GainExp，
            // 彼处因 exp= 赋参数槽而呈 cwtl+mov 0xc 形）。
            union { int all; unsigned short w[2]; unsigned char b[4]; } u;
            u.all = creature->GetExp();
            put_int((int)(short)u.w[1]);
            put_byte(0xa);
            put_byte((int)u.b[0]);
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

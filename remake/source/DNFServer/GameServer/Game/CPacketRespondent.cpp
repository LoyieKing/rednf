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

// ORIG Send @ 0x083403e0：+0x1c=0；CUser::Send(*this) 结果透传。
// [b28d-solo-CPacketRespondent P248档①] ORIG 尾 `call CUser::Send; leave; ret` 无 movzbl
// ⇒ 与 callee 同型 bool（CUser::Send 返回 bool，__PRETTY_FUNCTION__ 串 "bool CUser::Send"）。
// 声明 int 时 4.4 插 `movzbl %al,%eax`（4.1 不插）——改 bool 即 identical。
bool CPacketRespondent::Send()
{
    m_autoSend = 0;
    return m_user->Send(*this);
}

// ORIG Broadcast @ 0x08340404：
//   +0x1c=0；vill==0 && area==0 → 返回 0；
//   IsGateArea(m_user, area) → m_user->Send(*this)；
//   否则 G_GameWorld()->send_to_area(vill, area, *this)；返回 1。
// [b28d-solo-CPacketRespondent P248档①] 同 Send：gate 路径 `call Send; jmp 尾` 直通，
// 与 CUser::Send(bool) 同型；ORIG 0x08340404 尾 `mov $0x1,%eax` / `mov $0x0,%eax` 两臂。
bool CPacketRespondent::Broadcast(int vill, int area)
{
    m_autoSend = 0;
    if (vill == 0 && area == 0)
        return false;  // ORIG 0x834041e mov $0x0,%eax
    // ORIG 0x834045a：gate 臂 call Send 后直接 jmp 统一尾（+0x7f=add esp），
    // 跳过 mov $0x1,%eax ⇒ Send 返回值透传（正向 return 形）。
    if (IsGateArea(m_user, area))
        return m_user->Send(*this);
    G_GameWorld()->send_to_area(vill, area, *this);
    return true;   // ORIG 0x834047e mov $0x1,%eax
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
// b28d-40: P225 while 形复现 sete/test/je 物化 + P221 全表达式临时链翻 sibling
// trace 槽序（-0x30 深 / -0x20 浅）；ORIG 0x08340256，AE。
bool CPacketRespondent::IsGateArea(CUser* user, int area)
{
    // ORIG 0x834025c：两指针槽 -0x10/-0xc 先置 0（探针实证：声明+后续独立赋值形才生成
    // 两条 init，`T* p=f()` 合并形缺 init）。条件的 sete/test/je 物化形系 4.1.2 O0
    // 对槽比零的产物（探针实证 4.4-O0 槽比恒出 cmp/jne 直跳，无法源形复现；
    // 不可改 call 内联比较——ORIG cmp 读槽、每 call 仅一次，内联=双调用真 bug）。
    GameWorld* world = 0;
    Village* village = 0;
    world = G_GameWorld();
    while (world == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x281, 5)("G_GameWorld call failed");
        return false;
    }
    village = world->getUserVillage(user);
    while (village == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x287, 5)("getUserVillage call failed");
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
//   clear → put_header(0,0x69)；index=get_index() 后 put_byte(count) 占位计数；
//   遍历 map 值（post-increment 迭代）：仅当 item 非空且 GetCreatureItemType()==1
//   且 (slot==-1 || GetSlotNo()==slot) 时依次
//     put_int(GetUid)、put_byte(CCreature::GetStomach)、
//     IsGrowCreature ? [put_byte(1), put_int((int16)(exp>>16)), put_byte(10),
//                       put_byte(exp&0xff)]
//                   : [put_byte(0), put_int(GetExp)]、
//     put_byte(GetLevel)、strlen(name)+put_int+put_str、put_byte(IsNoCharge)，
//     计数 ++；
//   循环结束 put_byte(index, count) 回填占位字节，finalize(true)。
//
// [b28d-solo-CPacketRespondent P183/P0/P296] ORIG 0x083409e0 实证：
// ① 无 creature 具名局部——每处访问均 `mov -0xc(%ebp),%eax` 重读 item 后直呼
//    CCreature 访问器（单继承布局同址；镜像类型 reinterpret_cast 保持同一调用符号）；
// ② nameLen 在函数头预清（`movl $0x0,-0x14(%ebp)`）+ 循环内赋值（P198 两句式）；
// ③ 占位 put_byte 读 count 变量（`mov -0x10(%ebp),%edx`），非立即数 0；
// ④ 三跳过条件为正向包裹：4.1 产 `sete;test;je 尾` / 短路物化 `mov $1/$0;test;je 尾`；
//    写 `continue` 使 4.4 产 setne/jne + nop;jmp 中继岛（P296）。
void CPacketRespondent::MakeNotipacketCreatureItemList(
    const std::map<int, CCreatureItem*>* items, int slot)
{
    clear();
    put_header(0, 0x69);

    int nameLen = 0;
    int count = 0;
    int index = get_index();
    put_byte(count);  // 计数占位

    for (std::map<int, CCreatureItem*>::const_iterator it = items->begin();
         it != items->end(); it++)
    {
        CCreatureItem* item = it->second;
        if (item != 0)
        {
            if (item->GetCreatureItemType() == 1)
            {
                if (slot == -1 || item->GetSlotNo() == slot)
                {
                    put_int(item->GetUid());

                    // 同一对象继续按 CCreature 语义读取（ORIG 直呼 CCreature 访问器）。
                    put_byte(reinterpret_cast<CCreature*>(item)->GetStomach());
                    if (reinterpret_cast<CCreature*>(item)->IsGrowCreature())
                    {
                        put_byte(1);
                        // ORIG 0x8340b1f：GetExp 结果全宽落槽 -0x28，movzwl -0x26（u.w[1]）
                        // + movswl 直扩进 %edx 作实参（无独立局部、无写回）⇒ union 拆半
                        // 惯用形（同 GainExp，彼处因 exp= 赋参数槽而呈 cwtl+mov 0xc 形）。
                        union { int all; unsigned short w[2]; unsigned char b[4]; } u;
                        u.all = reinterpret_cast<CCreature*>(item)->GetExp();
                        put_int((int)(short)u.w[1]);
                        put_byte(0xa);
                        put_byte((int)u.b[0]);
                    }
                    else
                    {
                        put_byte(0);
                        put_int(reinterpret_cast<CCreature*>(item)->GetExp());
                    }

                    put_byte(reinterpret_cast<CCreature*>(item)->GetLevel());
                    // ORIG 0x834ba4/0x834bcc：GetName 调用两次（strlen 实参与 put_str 实参
                    // 各一次），无 name 具名局部（P183/P0：具名指针槽 -0xc 会使全帧深 4）。
                    nameLen = (int)strlen(reinterpret_cast<CCreature*>(item)->GetName());
                    put_int(nameLen);
                    put_str(reinterpret_cast<CCreature*>(item)->GetName(), nameLen);
                    put_byte((int)(unsigned char)
                             reinterpret_cast<CCreature*>(item)->IsNoCharge());

                    ++count;
                }
            }
        }
    }

    put_byte(index, count);
    finalize(true);
}

}  // namespace user_creature

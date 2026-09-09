// ==========================================================================
#include <vector>
#include <cstring>
#include "CPartyTelePort.h"
#include "CParty.h"
#include "CUser.h"
#include "GameWorld.h"
#include "CSystemTime.h"
#include "GameTypes.h"
#include "GlobalData.h"
#include "PacketGuard.h"

// [b28d-solo-CPartyTelePort] 类型名必须与 ORIG nm 一致：ORIG 的
// vector<effect_data> 相关符号为 `_ZNSt6vectorI11effect_dataSaIS0_EE*`
// （P0/N114：符号名不得自造）。
struct effect_data
{
    char village;
    int area;
    std::vector<unsigned short> users;
};
// [b28d-solo-CPartyTelePort][N89/N103] vector<effect_data> 扩容路径实例
// `_ZNSt20__uninitialized_copyILb0EE18uninitialized_copyISt13move_iteratorI
//  P11effect_dataES4_EE...`：ORIG(4.1 libstdc++) 内联 `::new` placement +
// `effect_dataC1ERKS_`（67 条，0x085c382e）；4.4 libstdc++ 改调
// `_ZSt10_ConstructI11effect_dataS0_EvPT_OT0_` helper（47 条）⇒ 库头实现差，
// 源侧不可复现（禁改 libstdc++ 头 / 禁发明替身符号）。语义等价。

// ============================================================================
// CPartyTelePort（sizeof 0x24）
// ============================================================================

// ============================================================================
// 实现
// ============================================================================
// ---- stPartyTelePort（ORIG 0x822d764，memset 0，清 4 字节状态头）----

// ORIG W 0x0822d764：memset(this,0,4)
void stPartyTelePort::reset()
{
    // P68：ORIG 0x0822d764 实参序「0x4→0x0→this」＝ 无 cast 形（cast 引发
    // 4.4 先载 this）。
    memset(this, 0, 4);
}

unsigned char stPartyTelePort::get_teleport_village() const
{
    return m_village;
}

unsigned char stPartyTelePort::get_teleport_area_index() const
{
    return m_areaIndex;
}

unsigned short stPartyTelePort::get_teleport_posx() const
{
    return m_posx;
}

unsigned short stPartyTelePort::get_teleport_posy() const
{
    return m_posy;
}

unsigned char stPartyTelePort::get_teleport_direction() const
{
    return m_direction;
}

CPartyTelePort::CPartyTelePort()
{
    m_tp.reset();
}

CPartyTelePort::~CPartyTelePort()
{
}

void CPartyTelePort::init(CParty* party)
{
    m_party = party;
}

void CPartyTelePort::set_teleport_data(unsigned char village, unsigned char area,
                                       short posx, short posy,
                                       unsigned char direction, char state)
{
    m_tp.reset();
    m_tp.m_village = village;
    m_tp.m_areaIndex = area;
    m_tp.m_posx = posx;
    m_tp.m_posy = posy;
    m_tp.m_direction = direction;
    m_tp.m_state = state;
    for (int i = 0; i <= 3; ++i)
    {
        set_teleport_member_state(i, 2);
    }
}

void CPartyTelePort::set_teleport_state(char state)
{
    m_tp.m_state = state;
}

char CPartyTelePort::get_teleport_member_state(int idx)
{
    if (m_party == 0)
    {
        return -1;
    }
    if (idx < 0 || idx > 3)
    {
        return -1;
    }
    // [b28-L14] ORIG 0x85bc26x 第三 null 守卫为 `test; je <尾常量块>`（主路径
    // fall-through）⇒ 正向包裹（P222 钥匙），非常规负极早返。
    if (m_party != 0)
    {
        return m_party->GetMember()[idx].m_field12;
    }
    return -1;
}

void CPartyTelePort::set_teleport_member_state(int idx, char state)
{
    if (m_party == 0)
    {
        return;
    }
    if (idx < 0)
    {
        return;
    }
    if (idx > 3)
    {
        return;
    }
    // ORIG 0x85bb94x：同上 cMember 成员直访（lea 基址+变址单段、0x12 折叠入 store disp）。
    m_party->GetMember()[idx].m_field12 = state;
}

unsigned char CPartyTelePort::get_teleport_village()
{
    return m_tp.m_village;
}

unsigned char CPartyTelePort::get_teleport_area_index()
{
    return m_tp.m_areaIndex;
}

short CPartyTelePort::get_teleport_posx()
{
    return m_tp.m_posx;
}

short CPartyTelePort::get_teleport_posy()
{
    return m_tp.m_posy;
}

unsigned char CPartyTelePort::get_teleport_direction()
{
    return m_tp.m_direction;
}

char CPartyTelePort::get_teleport_state()
{
    return m_tp.m_state;
}

void CPartyTelePort::send_teleport_status(unsigned char status)
{
    if (m_party == 0)
    {
        return;
    }

    PacketGuard pg;

    pg.clear();
    pg.put_header(0, 0x179);
    pg.put_byte((int)get_teleport_village());
    pg.put_byte((int)get_teleport_state());
    for (int i = 0; i <= 3; ++i)
    {
        short uid = -1;
        unsigned char mstate = 0;
        if (m_party->checkValidUser(i) != 0)
        {
            CUser* u = (CUser*)m_party->get_user(i);
            if (u != 0)
            {
                uid = (short)u->get_unique_id();
                mstate = (unsigned char)get_teleport_member_state(i);
            }
        }
        pg.put_short((int)uid);
        pg.put_byte((int)mstate);
    }
    pg.put_byte((int)status);
    pg.finalize(true);
    m_party->send_to_party(pg);

}

void CPartyTelePort::reset_teleport_data()
{
    for (int i = 0; i <= 3; ++i)
    {
        set_teleport_member_state(i, -1);
    }
    m_tp.reset();
    set_teleport_state(-1);
}

char CPartyTelePort::check_cur_teleport_state()
{
    // ORIG 0x85bbb1x：null 路径 mov $0,%eax 直返（无 result 槽写入）⇒ 源形为
    // 直接 return 0，result 局部仅存活于 else 域。
    if (m_party == 0)
    {
        return 0;
    }
    char result = 1;
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (m_party->checkValidUser(i) != 1)
            {
                continue;
            }
            if (get_teleport_member_state(i) == 4)
            {
                continue;
            }
            // [b28-L14] ORIG 0x85bbb6x：`cmp $0x1,%al; setne %al; test; je
            // <循环尾>` + fall-through `movb $0,result` ⇒ 正向 `!= 1` 物化包裹
            // （P222），负极 `== 1 → continue` 产 sete/jne 反演。
            if (get_teleport_member_state(i) != 1)
            {
                result = 0;
            }
        }
        if (result != 0)
        {
            set_teleport_state(1);
        }
    }
    return result;
}

// [b28d-solo-CPartyTelePort][N] 语义完成，byte 层跨版本封顶：
// ① 返回值归一化：ORIG 0x085bc034 `movzbl -0x35(%ebp),%eax`（4.1 直返 char），
//    4.4 对 `return <char>;`（函数返回 bool）恒发 `cmpb $0,+setne %al`
//    （/tmp/rp.cpp g1..g6 六形探针全归一化）；返回类型由 ORIG
//    __PRETTY_FUNCTION__ 串 "bool CPartyTelePort::process_teleport()"
//    （.rodata 0x08cbb220）锁定，不可改类型。
// ② 槽分配策略（N82附）：ORIG dir/posy 落深槽 -0x78/-0x74（第二循环
//    -0x70/-0x6c），4.4 复用于 -0x38/-0x34（-0x28/-0x24），cMyTrace 反向；
//    声明序/位置调整无效（4.4 聚合优先低槽 + 标量分区）。
// 其余指令序列（归一化地址/槽号后）与 ORIG 逐条一致；p167 调用点差集 CLEAR。
bool CPartyTelePort::process_teleport()
{
    char result;
    if (m_party == 0)
    {
        return 0;
    }

    result = check_cur_teleport_state();
    int moveResult = 0;
    int auraAvatarTime = 0;
    if (result != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (m_party->checkValidUser(i) != 1)
            {
                continue;
            }
            if (get_teleport_member_state(i) == 4)
            {
                continue;
            }
            CUser* user = (CUser*)m_party->get_user(i);
            if (user != 0)
            {
                // [b28d-solo-CPartyTelePort] ORIG 0x85bbc68：dir/posy 有槽
                // （-0x78/-0x74，P0 具名局部）；posx/area/village 无槽往返
                // ⇒ 源码在实参位内联 getter。
                int dir = (int)get_teleport_direction();
                int posy = (int)get_teleport_posy();
                moveResult = G_GameWorld()->check_move_area(user,
                    (int)get_teleport_village(), (int)get_teleport_area_index(),
                    (int)get_teleport_posx(), posy, dir, true, 0);
                if (m_party->getManager() == user)
                {
                    moveResult = user->is_equip_aura_avatar(0, auraAvatarTime);
                }
                if (moveResult > 0)
                {
                    set_teleport_member_state(i, 0);
                    set_teleport_state(2);
                    break;
                }
            }
        }

        if (get_teleport_state() == 1)
        {
            send_teleport_effect_at_different_place(1);
            for (int i = 0; i <= 3; ++i)
            {
                if (m_party->checkValidUser(i) != 1)
                {
                    continue;
                }
                if (get_teleport_member_state(i) == 4)
                {
                    continue;
                }
                CUser* user = (CUser*)m_party->get_user(i);
                if (user != 0)
                {
                    user->set_charac_visible_teleport_(true);
                    int dir = (int)get_teleport_direction();
                    int posy = (int)get_teleport_posy();
                    // [b28d-solo-CPartyTelePort] ORIG 0x85bbeb4：move_area 返回
                    // bool 直测（无 moved 槽）；trace 用 P227 全表达式临时链
                    // （实参 getCurCharacNo 在 ctor 前求值并跨 ctor 保活 %ebx）。
                    if (G_GameWorld()->move_area(user, (int)get_teleport_village(),
                            (int)get_teleport_area_index(), (int)get_teleport_posx(),
                            posy, dir, true, 1, 0, 0) != 1)
                    {
                        cMyTrace(__PRETTY_FUNCTION__, 0x43f9, 5)
                            ("party_teleport_error:(char_no:%d)", user->getCurCharacNo());
                    }
                    user->set_charac_visible_teleport_(false);
                }
            }
            send_teleport_effect_at_equal_place(2);
        }
        else
        {
            send_teleport_status((unsigned char)moveResult);
        }
    }

    if (get_teleport_state() == 1)
    {
        reset_teleport_data();
        CUser* manager = (CUser*)m_party->getManager();
        if (manager != 0)
        {
            // [b28d-solo-CPartyTelePort] ORIG 0x85bbf95：`movl $&GlobalData::
            // s_systemTime_,(%esp)`（对象直调，P304）；curSec/optValue/charNo
            // 均无槽往返 ⇒ 内联表达式 + P227 临时链。
            int newTime = GlobalData::s_systemTime_.getCurSec() + auraAvatarTime;
            manager->set_aura_avatar_option_value(0, newTime);
            manager->UpdateAuraAvatarOption(0, manager->get_aura_avatar_option_value(0));
            cMyTrace(__PRETTY_FUNCTION__, 0x4419, 0)
                ("party_teleport_success:(char_no:%d)", manager->getCurCharacNo());
        }
    }
    return result;
}

void CPartyTelePort::process_leave_user_at_teleport(CUser* user)
{
    if (m_party == 0)
    {
        return;
    }
    if (get_teleport_state() < 0)
    {
        return;
    }
    if (get_teleport_state() > 3)
    {
        return;
    }
    if (user->get_state() != 3)
    {
        return;
    }
    // [b28-L14] ORIG 0x85bc19x：`call get_teleport_state; cmp $0xff,%al;
    // setne %al; test %al,%al; je <统一尾>` ⇒ 正向 bool 物化包裹整段主体
    // （P222 钥匙 + P84 物化形）；负极 `== -1` 早返产 sete/jne 反演。
    if (get_teleport_state() != -1)
    {
        int slot = m_party->GetMemberSlotNo(user);
        if (slot < 0)
        {
            return;
        }
        if (slot > 3)
        {
            return;
        }
        if (m_party->checkValidUser(slot) != 1)
        {
            return;
        }

        if (m_party->getManager() == user)
        {
            set_teleport_member_state(slot, 0);
            set_teleport_state(2);
            send_teleport_status(8);
            reset_teleport_data();
        }
        else
        {
            set_teleport_member_state(slot, 4);
            check_cur_teleport_state();
            set_teleport_member_state(slot, 0);
            send_teleport_status(0);
            set_teleport_member_state(slot, 4);
            process_teleport();
        }
    }
}

// [b28d-solo-CPartyTelePort] AE（check_tu_routed 口径）：ORIG 0x085bc2b0。
// 修法：struct 名 TeleportEffect→effect_data（P0/N114 符号名）；内层迭代器
// begin/end 缓存（P13）；effect_data 逐字段赋值（P54，area 先于 village）；
// 第二个守卫改正向包裹（P222）消 4.4 多余 nop+jmp 岛；第二循环 int i（setg）。
// 残余仅重定位地址与 .o 阶段立即数显示（N124/P264）；p167 调用点差集 CLEAR。
void CPartyTelePort::send_teleport_effect_at_different_place(int param)
{
    if (m_party == 0)
    {
        return;
    }

    std::vector<effect_data> effects;
    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i) != 1)
        {
            continue;
        }
        CUser* user = (CUser*)m_party->get_user(i);
        if (user != 0)
        {
        // [b28d-solo-CPartyTelePort] ORIG 0x85bc31c：begin sret -0x38 / end
        // sret -0x3c 均在循环初始化区求值一次（P13 缓存 end）；found 的
        // `movb $0,-0x19` 落 end 之后（ORIG 0x85bc346）⇒ 声明在 it/e 之后。
        std::vector<effect_data>::iterator it = effects.begin(),
            e = effects.end();
        bool found = false;
        for (; it != e; ++it)
        {
            if (it->area == user->get_area(false) &&
                it->village == user->getCurCharacVill())
            {
                found = true;
                it->users.push_back(user->get_unique_id());
                break;
            }
        }
        if (!found)
        {
            // [b28d-solo-CPartyTelePort] ORIG 0x85bc157：effect_data 默认构造
            // 后逐字段写（area 先于 village，P54 逐字段赋值≠聚合初始化）。
            effect_data ed;
            ed.area = user->get_area(false);
            ed.village = user->getCurCharacVill();
            ed.users.push_back(user->get_unique_id());
            effects.push_back(ed);
            // [b28d-solo-CPartyTelePort] ORIG 0x85bc46c `movl $0x0,-0x18(%ebp)`
            // 死初始化局部（写后不读，P31/P232 槽基对齐；语义等价）。
            int pad0 = 0;
        }
        }
    }

    // [b28d-solo-CPartyTelePort] ORIG 0x85bc608 用 setg（有符号）⇒ 源码 int i。
    for (int i = 0; i < (int)effects.size(); ++i)
    {
        PacketGuard pg;

        pg.put_header(0, 0x17b);
        pg.put_byte((int)(char)param);
        int cnt = (int)effects[i].users.size();
        pg.put_byte((int)(char)cnt);
        for (int j = 0; j < cnt; ++j)
        {
            pg.put_short((int)effects[i].users[j]);
        }
        pg.finalize(true);
        G_GameWorld()->send_to_area(
                                   (int)effects[i].village,
                                   effects[i].area, pg);

    }
}

// [b28d-solo-CPartyTelePort] AE（check_tu_routed 口径）：ORIG 0x085bc64c。
// 修法：area/vill 去具名局部改实参内联（P0/P52，ORIG 用 %ebx/%esi 无槽）；
// 双 continue 守卫（P223 同层多 continue 岛）+ `!checkValidUser` xor 形；
// p167 调用点差集 CLEAR。
void CPartyTelePort::send_teleport_effect_at_equal_place(int param)
{
    if (m_party == 0)
    {
        return;
    }
    CUser* manager = (CUser*)m_party->getManager();
    if (manager == 0)
    {
        return;
    }
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        return;
    }

    PacketGuard pg;

    pg.put_header(0, 0x17b);
    pg.put_byte((int)(char)param);

    std::vector<unsigned short> users;
    for (int i = 0; i <= 3; ++i)
    {
        // [b28d-solo-CPartyTelePort] ORIG 0x85bc6a7：`call; xor $1; test;
        // jne <nop 岛>` + body 尾 jmp + nop = 4.1 同层多 continue 岛
        // （P223）：源码为两条独立 continue 守卫。
        if (!m_party->checkValidUser(i))
        {
            continue;
        }
        CUser* u = (CUser*)m_party->get_user(i);
        if (u != 0)
        {
            users.push_back(u->get_unique_id());
        }
    }

    char cnt = (char)users.size();
    pg.put_byte((int)cnt);
    for (int j = 0; j < (int)cnt; ++j)
    {
        pg.put_short((int)users[j]);
    }
    pg.finalize(true);

    if (manager->get_state() == 3)
    {
        // [b28d-solo-CPartyTelePort] ORIG 0x85bc7e3：get_area 结果留 %ebx、
        // getCurCharacVill 留 %esi（P0 无槽往返 / P52 链式 receiver 单语句）。
        G_GameWorld()->send_to_area((int)manager->getCurCharacVill(),
                                    manager->get_area(false), pg);
    }

}

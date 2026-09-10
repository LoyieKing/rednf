// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPartyTelePort/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
#include "CPartyTelePort/reset.inc"
{
    // P68：ORIG 0x0822d764 实参序「0x4→0x0→this」＝ 无 cast 形（cast 引发
    // 4.4 先载 this）。
    memset(this, 0, 4);
}

#include "CPartyTelePort/get_teleport_village.inc"
{
    return m_village;
}

#include "CPartyTelePort/get_teleport_area_index.inc"
{
    return m_areaIndex;
}

#include "CPartyTelePort/get_teleport_posx.inc"
{
    return m_posx;
}

#include "CPartyTelePort/get_teleport_posy.inc"
{
    return m_posy;
}

#include "CPartyTelePort/get_teleport_direction.inc"
{
    return m_direction;
}

#include "CPartyTelePort/CPartyTelePort.inc"
{
    m_tp.reset();
}

#include "CPartyTelePort/dtor_CPartyTelePort.inc"
{
}

#include "CPartyTelePort/init.inc"
{
    m_party = party;
}

#include "CPartyTelePort/set_teleport_data.inc"
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

#include "CPartyTelePort/set_teleport_state.inc"
{
    m_tp.m_state = state;
}

#include "CPartyTelePort/get_teleport_member_state.inc"
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

#include "CPartyTelePort/set_teleport_member_state.inc"
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

#include "CPartyTelePort/get_teleport_village_1.inc"
{
    return m_tp.m_village;
}

#include "CPartyTelePort/get_teleport_area_index_1.inc"
{
    return m_tp.m_areaIndex;
}

#include "CPartyTelePort/get_teleport_posx_1.inc"
{
    return m_tp.m_posx;
}

#include "CPartyTelePort/get_teleport_posy_1.inc"
{
    return m_tp.m_posy;
}

#include "CPartyTelePort/get_teleport_direction_1.inc"
{
    return m_tp.m_direction;
}

#include "CPartyTelePort/get_teleport_state.inc"
{
    return m_tp.m_state;
}

#include "CPartyTelePort/send_teleport_status.inc"
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

#include "CPartyTelePort/reset_teleport_data.inc"
{
    for (int i = 0; i <= 3; ++i)
    {
        set_teleport_member_state(i, -1);
    }
    m_tp.reset();
    set_teleport_state(-1);
}

#include "CPartyTelePort/check_cur_teleport_state.inc"
{
    // ORIG 0x85bbb1x：null 路径 mov $0,%eax 直返（无 result 槽写入）⇒ 源形为
    // 直接 return 0，result 局部仅存活于 else 域。
    if (m_party == 0)
    {
        return 0;
    }
    // [N6S2NegB 2026-09-11] ORIG 0x85bbb21 `movb $0x1,-0xd(%ebp)` / 0x85bbb7f
    // `movb $0x0,-0xd(%ebp)` / 尾 0x85bbbb1 `movzbl -0xd(%ebp),%eax` ⇒ 局部为
    // **bool**（char 形在 4.4 下 `return` 会被插 `cmpb; setne`，多 1 条）。
    // 配套：callee 返回类型已由父泳道 N6Free231to250 改 bool（.h:48）。
    bool result = true;
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
                result = false;
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
// [N6S2NegB 2026-09-11 复核] 真残差收敛为 1 条（n1_v: DIFF orig=305 our=306 gap=-1）：
//   ORIG 0x085bc034 尾 `movzbl -0x35(%ebp),%eax`，我方 `cmpb $0x0,-0x35(%ebp); setne %al`；
//   其余 305 条（槽号归一后）逐条全等，差集仅 3 处 .o 未重定位立即数显示伪差
//   （prett 串 / "party_teleport_*" / &GlobalData::s_systemTime_）。
//   根因链（探针 /tmp/probe_pt/w1..w7）：`char` 局部 return 到 bool 函数 ⇒ 4.4 恒插
//   cmpb+setne（w1/w4/w5）；`bool` 局部 + **bool callee** ⇒ 赋值仅 `mov %al,槽`、末尾
//   `movzbl 槽,%eax`（w6 = ORIG 形）；`char` 局部 + bool callee ⇒ 末尾仍 setne（w7）。
//   ⇒ ORIG 真相是 `bool check_cur_teleport_state()` + process_teleport 的 result 为 bool；
//   我方两者皆 char。[落地 2026-09-11] 父泳道 N6Free231to250 改 `CPartyTelePort.h:48`
//   为 bool，本 TU 同步三处：.inc 签名、`bool result = true/false`、本函数 `bool result;`。
//   落地读数（compare_tu_game_orig 口径）：本件 DIFF→**NEAR**（301=301；槽号归一后与 ORIG
//   逐条全等）。NEAR 已为口径上限：残差仅 ①clean 不归一栈槽号（我方 `-0x45..` vs ORIG
//   `-0x35..`，4.4 槽分配 +0x10 见 ②）②ORIG 侧符号化立即数 `$&_ZZ…PRETTY_FUNCTION__` /
//   `$"party_teleport_*"` / `$&_ZN10GlobalData13s_systemTime_E` vs 我方 .o 侧 `$<A>`
//   （clean 单侧归一，两侧永不相等）。TU 四数 near 0→1 / diff 3→2，其余不变；
//   check_cur_teleport_state 仍 AE 56=56。
#include "CPartyTelePort/process_teleport.inc"
{
    // [N6S2NegB 2026-09-11] ORIG 0x085bc034 `movzbl -0x35(%ebp),%eax`（bool 槽直取）
    // + 0x085bbbe2 `mov %al,-0x35(%ebp)`（callee 返回 bool 无转换）⇒ result 为 **bool**。
    bool result;
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

#include "CPartyTelePort/process_leave_user_at_teleport.inc"
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
#include "CPartyTelePort/send_teleport_effect_at_different_place.inc"
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
#include "CPartyTelePort/send_teleport_effect_at_equal_place.inc"
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

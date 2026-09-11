// [SweepDBLogin 本轮] verdict=DIFF gap=-14 ndiff=456（N82 帧 0x60↔0x70 + N1 常量拆分
// （ORIG `add $0xf0; movzwl 0xa(%eax)` vs 我方 `add $0xe0; ... 0x1a`）+ N115 outgoing；
// p167 仅 cMyTrace C1/C2 同址别名豁免）。
// 本轮修法：尾段 `bool ok` 链（`mov %al,-0x9; cmpb $0,-0x9; je` 六段槽物化）改
// **提前 continue**（`if (!dbBonus->fetch()) continue;` ×5）⇒ 复现 ORIG 的
// `call; xor $1,%eax; test %al,%al; jne <循环增量>`（ORIG-only xor 6→1、jne 6→1）。
// 三形并列读数（防回退）：bool ok 链 gap=-9/ndiff=447/xor 缺 6；嵌套正极性 if
// gap=-4/ndiff=508/xor 缺 6（4.4 对 `if(f())` 恒无 xor 直判）；goto 共享标签串
// 与 continue 形读数相同（gap=-14/ndiff=456/xor 缺 1）⇒ 取 xor 形匹配的 continue 形，
// gap 的 -5 差属 4.4 continue 出口实现（N 族）。
// [DBLogin-V3 消指令旋钮 M] orig=394 恒定（判据同上）：
//   baseline 407/-13/ndiff153 → M1(槽数组视图 P311 替 char* 指针算术，10 处) 394/**0**/92
//   → M2a(条件点同视图化) 394/0/90 → M3(早 continue 形) 395/-1/75 → M3b(包裹式 `>=`
//   书写序，slot 在左) 394/0/82。
// 取 M1+M2a+M3b：gap 归零（+13→0，最省指令），ndiff 153→82（-71）。M3 虽 ndiff 更低
// 但多 1 条（4.4 对 `short` 比较用 32 位 cwtl+`movzwl %ax,%ebx`，4.1 用 16 位 `cmp %ax,%bx`）。
// 关键实证：`sig->m_characData + i*0x94 [+off]`（char* 指针算术）令 4.4 把 `add $0xe0`
// 提前，并把 `i*0x94+0x1a` 退化为 shl/add 链（单点 +6 条）；换具名结构体数组后复现
// ORIG `imul $0x94; lea; add $0xe0` 三步（TU 头 probe_arr 结论在本函数 10 处全数成立）。
// 余 = `short` 比较宽度（cwtl+32 位 cmp vs 16 位 cmp）、载入序（sig/i 先后）、jmp/xor 各 1
// （4.4 出口实现）。
bool DB_Login::GetMercenarySystemInfo(SIG_LOGIN_DATA* sig)
{
    if (sig == 0)
    {
        return 0;
    }
    MySQL* dbBonus = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    MySQL* dbItems = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);

    // [DBLogin-V3 旋钮M1] P311 槽数组视图：ORIG 各点 `mov i,%eax; mov sig,%edx;
    // imul $0x94,%eax,%eax; lea (%edx,%eax,1),%eax; add $0xe0,%eax`（char* 指针算术形
    // 恒把 `add $0xe0` 提前并令 `i*0x94+0x1a` 退化为 shl/add 链，OURS-only shl×3/add×6）；
    // 具名槽结构数组成员直访问复现 ORIG 的 `imul; lea; add $0xe0` 三步（TU 头 probe_arr
    // f4 已实证；仅表达布局，不实例化、零符号、零 sizeof/偏移影响）。
    // [K2ADBLogin 2026-09-11] 局部 cast 视图（MercCharacSlot/MercCharacView）已删：
    //   改用权威数组视角 `sig->m_characView.m_slots[i]`（SigTypes_D.h
    //   SIG_CHARAC_SLOTS_VIEW；S216/P235 已在 GetGameDBInfo 实证 i-first 形，
    //   cast 视图恒 base-first）。视图已补 m_name(+0x04)/m_level(+0x1a) 具名字段
    //   （sizeof 仍 0x94、槽偏移 0xe0、SIG_LOGIN_DATA 布局不变）。
    //   读数：差异行 358→246、ours 393→392；四数不变（仍 DIFF，余 = `movzwl
    //   0xa(%eax),%ebx` 单段 vs 我方 `movzwl %ax,%ebx` 双段（16/32 位宽差，跨版本
    //   不可翻）+ 三处 cMyTrace 段对齐窗口漂移）。p167 仅 C1/C2 豁免项、CLEAR。
    // [pack 例检] 上述视图偏移经 c6root 4.4.7 -m32 + TU 实旗标探针硬断言：
    // m_characNo=0 / m_name=4 / m_level=0x1a / sizeof=0x94（0x1a 偶对齐 ⇒ 无插垫）；
    // m_slots=0xe0（0xe0%4==0）。
    // ORIG `movzwl 0xa8e0(%eax),%eax`（G_CDataManager 等级字段直 disp）——裸偏移形产
    // `add $0xa8e0,%eax; movzwl (%eax)`（+1 条）。
    struct CDataMgrLevelView { char m_pad[0xa8e0]; short m_level; };

    // [P0/P212 去槽·ORIG 直证] ORIG 0x08415670/0x08415677 `movl $0x0,-0x54(%ebp);
    // movl $0x0,-0x58(%ebp)` 在函数入口（for 之前）清零 ⇒ gotSlot/itemId 原码是
    // **函数头声明**（非循环体内声明），槽 -0x54/-0x58；旧循环内声明令两槽下沉到
    // -0x28/-0x24 并整体撑大帧。
    int gotSlot = 0;
    int itemId = 0;
    // [K9GameRestA-S322] ORIG 入口清零序 = -0x54/-0x58 **然后** -0x18 再 -0x14
    // （0x08415670/77/7e/85）⇒ outIdx 声明在 gotSlot/itemId **之后**（P331 同尺寸
    // 标量区声明序可控）。
    int outIdx = 0;
    for (int i = 0;
         i < sig->m_characCount && i < 0x24; ++i)
    {
        // [hc 收] sig->m_characData 为 SIG_LOGIN_DATA 角色视图数组（步长 0x94），
        // +0x1a 即槽内等级(short)字段裸偏移（§5.3 线格式序列化缓冲豁免）。
        // [判据①/P0] ORIG 0x084156a5 `movzwl 0xa(%eax),%ebx`（level 直入 %ebx，**无栈槽**；
        // 槽表无 -0x12 对应槽）⇒ 原码无 `short level` 具名局部，等级是右值内联表达式。
        // [DBLogin-V3 旋钮M3] ORIG +0x08d `movzwl 0xa(%eax),%ebx`（槽等级先取值）→
        // +0x0a9 call G_CDataManager → `cmp %ax,%bx; setl %al; test; jne <跳过体>`
        // ⇒ 原码条件为 `slotLevel >= mgrLevel`（操作数序 slot 在左=求值序；4.4 把
        // `a >= b` 规范成 `!(a < b)` ⇒ setl+jne 同形）。旧 `if (mgr <= slot)` 产
        // `setle/je` 反号（P196 折痕，本点位 4.4 不复现 `!(b >= a)` 折叠）。
        // [K9GameRestA-S322] ORIG 0x084156a5 `movzwl 0xa(%eax),%ebx` + `cmp %ax,%bx;
        // setl %al`（**16 位**有符号比较）vs 我方 `movzwl %ax,%ebx; cwtl; cmp %eax,%ebx`
        // （32 位，多 2 条）⇒ 试「两侧都显式 (short)」消掉整型提升（P248 档③）。
        // [PoolF 2026-09-11 试验] ORIG 0x084156a5 `movzwl 0xa(%eax),%ebx`（单段直落
        // callee-saved，跨 G_CDataManager 调用保活）；我方两段
        // `movzwl ...,%eax; mov %eax,%ebx`。按 P231/P307 试裸 `register`（非 asm 绑定，
        // P0 合规）使 4.4 直落 %ebx。实测无效即回退。
        register unsigned short slotLevel = sig->m_characView.m_slots[i].m_level;
        if ((short)slotLevel >=
            (short)reinterpret_cast<CDataMgrLevelView*>(G_CDataManager())->m_level)
        {
            // P183：ORIG 每次使用均由 sig 重新展开 charac_no（sig+0xe0 视图）与
            // mercenary 基址，禁具名缓存。
            dbBonus->set_query(
                "seLect mercenary_start_time,mercenary_finish_time,mercenary_area,mercenary_period from charac_link_bonus where charac_no=%u",
                sig->m_characView.m_slots[i].m_characNo);
            if (!dbBonus->exec(true))
            {
                return 0;
            }

            sig->m_mercenaries[outIdx].Reset();
            strncpy(sig->m_mercenaries[outIdx].m_name,
                    sig->m_characView.m_slots[i].m_name, 0x1e);  // 角色名
            sig->m_mercenaries[outIdx].m_characNo =
                sig->m_characView.m_slots[i].m_characNo;
            sig->m_mercenaries[outIdx].m_level =
                sig->m_characView.m_slots[i].m_level;

            dbItems->set_query(
                "seLect slot,it_id from user_items where charac_no=%u and expire_date>now() and stat=0 order by slot limit 8",
                sig->m_characView.m_slots[i].m_characNo);
            if (!dbItems->exec(true))
            {
                // ORIG（反编译 C）：exec 失败即跳过本角色的物品装载与
                // charac_link_bonus 处理，继续下一角色（最终仍 return 1），
                // 非提前 return 1 终结全函数。
                continue;
            }
            int nRows = dbItems->get_n_rows();
            if (nRows == 8)
            {
                for (int expectSlot = 0; expectSlot < nRows; ++expectSlot)
                {
                    if (!dbItems->fetch())
                    {
                        // [P227] ORIG：charac_no 在 cMyTrace ctor 前求值并跨 ctor 保活
                        // %ebx（`... mov (%eax),%ebx; ... ctor(-0x50); mov %ebx,0xc(%esp)`）
                        // ⇒ 单表达式临时链（两语句形把 C1 提到取值之前）。
                        cMyTrace(__PRETTY_FUNCTION__, 0x3ea2, 5)(
                            "DB_Login::GetMercenarySystemInfo %d ERROR Charac_no=%u",
                            0x3ea2,
                            sig->m_characView.m_slots[i].m_characNo);
                        break;
                    }
                    if (!dbItems->get_int(0, gotSlot))
                    {
                        // [P227] 同上（ORIG ctor 槽 -0x40，charac_no 跨 ctor 保活 %ebx）。
                        cMyTrace(__PRETTY_FUNCTION__, 0x3ea8, 5)(
                            "DB_Login::GetMercenarySystemInfo %d ERROR Charac_no=%u",
                            0x3ea8,
                            sig->m_characView.m_slots[i].m_characNo);
                        return 0;
                    }
                    if (!dbItems->get_int(1, itemId))
                    {
                        // [P227] 同上（ORIG ctor 槽 -0x30）。
                        cMyTrace(__PRETTY_FUNCTION__, 0x3eae, 5)(
                            "DB_Login::GetMercenarySystemInfo %d ERROR charac_no=%u",
                            0x3eae,
                            sig->m_characView.m_slots[i].m_characNo);
                        return 0;
                    }
                    if (gotSlot != expectSlot)
                    {
                        sig->m_mercenaries[outIdx].m_avatarBonus = 0;   // 连续槽位破坏标志（+0x32）
                        break;
                    }
                    sig->m_mercenaries[outIdx].m_avatarItems[expectSlot] = itemId;
                }
            }
            else
            {
                sig->m_mercenaries[outIdx].m_avatarBonus = 0;
            }
            if (dbBonus->getAffectedRowCount() == 0)
            {
                dbBonus->set_query(
                    "inSert into charac_link_bonus (charac_no,mercenary_start_time,mercenary_finish_time,mercenary_area,mercenary_period) values(%u,0,0,-1,-1)",
                    sig->m_characView.m_slots[i].m_characNo);
                if (dbBonus->exec(true))
                {
                    sig->m_mercenaries[outIdx].m_field28 = 0;      // mercenary_start_time(+0x28)
                    sig->m_mercenaries[outIdx].m_field2c = 0;      // mercenary_finish_time(+0x2c)
                    sig->m_mercenaries[outIdx].m_field30 = 0xff;   // mercenary_area(+0x30)
                    sig->m_mercenaries[outIdx].m_field31 = 0xff;   // mercenary_period(+0x31)
                    outIdx += 1;
                    sig->m_mercenaryCount += 1;
                }
            }
            else
            {
                // [SweepDBLogin 本轮] P198/P0/P223②：ORIG 无 ok 具名槽，五步各发
                // `call; xor $1,%eax; test %al,%al; jne <循环增量>`（= 提前 continue；
                // 旧 bool ok 链产 `mov %al,-0x9; cmpb $0,-0x9; je` 槽物化，正极性
                // 嵌套 if 产无 xor 的 `je`，goto 共享标签多 5 条出口跳转）。
                // get_int 走 _ZN5MySQL7get_intEiRj（unsigned int&，ORIG 0x80e22ae；
                // m_field28/m_field2c 同为 +0x28/+0x2c 的 4 字节）。
                if (!dbBonus->fetch()) continue;
                if (!dbBonus->get_int(0, reinterpret_cast<unsigned int&>(
                        sig->m_mercenaries[outIdx].m_field28))) continue;
                if (!dbBonus->get_int(1, reinterpret_cast<unsigned int&>(
                        sig->m_mercenaries[outIdx].m_field2c))) continue;
                if (!dbBonus->get_byte(2, reinterpret_cast<char&>(
                        sig->m_mercenaries[outIdx].m_field30))) continue;
                if (!dbBonus->get_byte(3, reinterpret_cast<char&>(
                        sig->m_mercenaries[outIdx].m_field31))) continue;
                outIdx += 1;
                sig->m_mercenaryCount += 1;
            }
        }
    }
    return 1;
}
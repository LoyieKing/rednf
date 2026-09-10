// ============================================================================
// GetMercenarySystemInfo（ORIG T 0x08415618）
// ============================================================================

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
bool DB_Login::GetMercenarySystemInfo(SIG_LOGIN_DATA* sig)
{
    if (sig == 0)
    {
        return 0;
    }
    MySQL* dbBonus = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    MySQL* dbItems = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);

    int outIdx = 0;
    for (int i = 0;
         i < sig->m_characCount && i < 0x24; ++i)
    {
        // [hc 收] sig->m_characData 为 SIG_LOGIN_DATA 角色视图数组（步长 0x94），
        // +0x1a 即槽内等级(short)字段裸偏移（§5.3 线格式序列化缓冲豁免）。
        // [判据①/P0] ORIG 0x084156a5 `movzwl 0xa(%eax),%ebx`（level 直入 %ebx，**无栈槽**；
        // 槽表无 -0x12 对应槽）⇒ 原码无 `short level` 具名局部，等级是右值内联表达式。
        if (*reinterpret_cast<short*>(reinterpret_cast<char*>(G_CDataManager()) + 0xa8e0) <=
            *reinterpret_cast<short*>(sig->m_characData + i * 0x94 + 0x1a))
        {
            // P183：ORIG 每次使用均由 sig 重新展开 charac_no（sig+0xe0 视图）与
            // mercenary 基址，禁具名缓存。
            dbBonus->set_query(
                "seLect mercenary_start_time,mercenary_finish_time,mercenary_area,mercenary_period from charac_link_bonus where charac_no=%u",
                *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94));
            if (!dbBonus->exec(true))
            {
                return 0;
            }

            sig->m_mercenaries[outIdx].Reset();
            strncpy(sig->m_mercenaries[outIdx].m_name,
                    sig->m_characData + i * 0x94 + 4, 0x1e);        // 角色名
            sig->m_mercenaries[outIdx].m_characNo =
                *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94);
            sig->m_mercenaries[outIdx].m_level =
                *reinterpret_cast<short*>(sig->m_characData + i * 0x94 + 0x1a);

            dbItems->set_query(
                "seLect slot,it_id from user_items where charac_no=%u and expire_date>now() and stat=0 order by slot limit 8",
                *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94));
            if (!dbItems->exec(true))
            {
                // ORIG（反编译 C）：exec 失败即跳过本角色的物品装载与
                // charac_link_bonus 处理，继续下一角色（最终仍 return 1），
                // 非提前 return 1 终结全函数。
                continue;
            }
            int nRows = dbItems->get_n_rows();
            int itemId = 0;
            if (nRows == 8)
            {
                for (int expectSlot = 0; expectSlot < nRows; ++expectSlot)
                {
                    if (!dbItems->fetch())
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__,
                                       0x3ea2, 5);
                        trace("DB_Login::GetMercenarySystemInfo %d ERROR Charac_no=%u", 0x3ea2,
                              *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94));
                        break;
                    }
                    int gotSlot = 0;
                    if (!dbItems->get_int(0, gotSlot))
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__,
                                       0x3ea8, 5);
                        trace("DB_Login::GetMercenarySystemInfo %d ERROR Charac_no=%u", 0x3ea8,
                              *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94));
                        return 0;
                    }
                    if (!dbItems->get_int(1, itemId))
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__,
                                       0x3eae, 5);
                        trace("DB_Login::GetMercenarySystemInfo %d ERROR charac_no=%u", 0x3eae,
                              *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94));
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
                    *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94));
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

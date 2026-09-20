// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e4066 orig/ours=180/150 ndiff-blocks=24
//   帧差 sub $3c vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | sub    $0x3c,%esp
// [SweepDispatchers 2026-09-09] **真语义 bug 修复**（149→180 = ORIG 180，p167 18/18 CLEAR）：
//   ① m_field29 是 4 位有符号位域：ORIG 0x081e4262 `movzbl 0x29; shl $4; sar $4,%al; movsbl`
//      旧码 `(short)(char)((raw<<4)>>4)` 用有符号 char 往返恒等于 raw（0x1F 应得 -1，旧得 31）
//      ⇒ 改 `(short)((signed char)((unsigned char)m_field29 << 4) >> 4)`；
//   ② 删 characNo/entry 引用/script 具名缓存（P183）：ORIG 7 次独立 vector::operator[]；
//   ③ 循环/比较改 int idx + `(int)monsterCount > i` 形；IsLoadStrikerSkill 返回 char→bool
//      （ORIG 0x086086aa 直返 bool 字段 + 调用点 0x081e4209 `xor $1;test;je` 无 setcc，
//      SkillSlot.h/.cpp 同步，mangled/布局不变，Main 已批准）。
int Dispatcher_SelectStriker::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return 0xffffffff;
    }
    if (user->get_state() != 3)
    {
        return 0x7fffffff;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0x7fffffff;
    }
    // ORIG 0x081e40d0/0x081e40df：get_charac_no 结果直接进实参槽（无具名局部，P0）。
    int idx = user->get_charac_idx(user->get_charac_no(-1));
    if (idx == -1)
    {
        return 0x39;
    }
    // ORIG 0x081e40d7 `mov 0x10(%ebp),%eax; mov %eax,-0x2c(%ebp)`：msg 引用在**首次使用点**才落槽（判据⑩ 后声明者深）。
    const MSG_SELECT_STRIKER& m = reinterpret_cast<const MSG_SELECT_STRIKER&>(msg);
    // ORIG 0x081e40f7..0x081e428c：sel 每次从 msg+0xd 重读（P183 去缓存），
    // m_characList[sel] 每次重发 vector::operator[]（共 7 次）。
    if ((char)m.m_0d == idx)
    {
        user->setCurCharacTagCharacNo(0);
        user->setCurCharacStrikerSkillIndex(0);
        user->m_characterView.enableSaveCharacView();           // +0x796f4
        user->setTagCharac(0);
        return 0x7fffffff;
    }
    if ((char)m.m_0d < 0 || (char)user->m_characList.size() <= (char)m.m_0d)
    {
        return 0x39;
    }
    short level = user->m_characList[(char)m.m_0d].m_level;     // +0x27
    if (level < G_CDataManager()->m_characLink.m_field0)        // +0x629c
    {
        return 0xe;
    }
    if (!user->m_characList[(char)m.m_0d].m_skillSlot.IsLoadStrikerSkill())
    {
        return 0x13;
    }
    // [DispA2 P0] ORIG 0x081e4239/0x081e426f/0x081e429b 均为 `movsbl` 后
    //   `mov %eax,-0x24/-0x20/-0x1c(%ebp)`（**4 字节槽**）⇒ 原码是 int 具名局部；
    //   旧码 char/short/unsigned char 令三槽各缩 3/2/3 字节（帧 0x34 vs ORIG 0x3c）。
    int firstGrow = user->m_characList[(char)m.m_0d].m_field26;   // +0x26（movsbl）
    // ORIG 0x081e4262 `movzbl 0x29; shl $4; sar $4,%al; movsbl` = 4 位有符号位域提取
    // （低 4 位符号扩展）；旧写法 (char)raw 直接往返会丢符号（真语义 bug）。
    int secondGrow = (int)((signed char)(
        (unsigned char)user->m_characList[(char)m.m_0d].m_field29.m_growthType << 4) >> 4);
    int deadField1239 =
        user->m_characList[(char)m.m_0d].m_field1239;           // +0x1239（ORIG 读取后未使用）
    (void)deadField1239;
    // ORIG 0x081e42a1..0x081e42ae：skillIdx 无槽（`movzbl 0xe; movzbl %al,%edi` 直入实参），
    //   firstGrow/secondGrow 到实参处才 `movswl` 截断 ⇒ 第三参直接内联。
    if (!G_CDataManager()->m_altarConfig.m_strikerScript->checkSkillIndex(
            (short)firstGrow, (short)secondGrow, (int)m.m_0e))
    {
        return 0x15;
    }
    return 0;
}
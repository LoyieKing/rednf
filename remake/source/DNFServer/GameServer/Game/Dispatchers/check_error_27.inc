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
    if (!reinterpret_cast<SkillSlot*>(reinterpret_cast<CharacInfoFields*>(
             &user->m_characList[(char)m.m_0d])->m_skill)
             ->IsLoadStrikerSkill())                                        // +0x892
    {
        return 0x13;
    }
    char firstGrow = user->m_characList[(char)m.m_0d].m_field26;   // +0x26（movsbl）
    // ORIG 0x081e4262 `movzbl 0x29; shl $4; sar $4,%al; movsbl` = 4 位有符号位域提取
    // （低 4 位符号扩展）；旧写法 (char)raw 直接往返会丢符号（真语义 bug）。
    short secondGrow = (short)((signed char)(
        (unsigned char)user->m_characList[(char)m.m_0d].m_field29.m_growthType << 4) >> 4);
    unsigned char deadField1239 =
        user->m_characList[(char)m.m_0d].m_field1239;           // +0x1239（ORIG 读取后未使用）
    (void)deadField1239;
    unsigned char skillIdx = m.m_0e;
    // ORIG 0x081e42b4 直接内联 G_CDataManager()->m_altarConfig.m_strikerScript（无局部槽）。
    if (!G_CDataManager()->m_altarConfig.m_strikerScript->checkSkillIndex(
            firstGrow, secondGrow, (int)skillIdx))
    {
        return 0x15;
    }
    return 0;

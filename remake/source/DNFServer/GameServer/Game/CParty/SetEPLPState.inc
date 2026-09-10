void CParty::SetEPLPState(char state)
{
    m_eplpState = state;
    if (state == 2)
    {
        		if (m_battleField.m_pDungeon != 0)
		{
			for (int i = 0; i <= 3; ++i)
			{
				if (!_checkValidUser(i))
					continue;
				// ORIG：idx 值直落 callee-saved %ebx（`mov %eax,%ebx;
                // mov %ebx,0x4(%esp)`），块作用域 register 复现。
                { register int idx = m_battleField.m_pDungeon->get_index();
                  GetInstanceDungeonEntranceLog()->DecrementDungeonEntrance(idx, false); }
            }
        }
    }

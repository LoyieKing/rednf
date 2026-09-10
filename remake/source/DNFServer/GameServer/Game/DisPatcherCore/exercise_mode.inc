bool DisPatcher_DeleteItem::exercise_mode(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    const MSG_DELETE_ITEM& req = *reinterpret_cast<const MSG_DELETE_ITEM*>(&msg);
    stDeleteItemParam& out = static_cast<stDeleteItemParam&>(param);
    // ORIG thunk 0x0822fe4e = CUser::GetPartyIndex()，结果作 GetParty(int) 实参
    CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
    if (party == 0)
    {
        return false;
    }

    WongWork::CDeathTower* tower = 0;
    if (user->get_state() == 10)
    {
        tower = user->getDeathTower();
    }

    bool limitNotOne = false;
    if (tower != 0)
    {
        // [b23S3C] P48 去缓存：ORIG +0x077/+0x09d 两次独立求值
        // getCDungeonMGr()->getDungeon()->limitOfStackableItemInTower（无 dungeon 局部）；
        // !=1 用 `if(x) r=true` 形（P194）给出 ORIG xor $0x1 编码。
        // [b28d-solo-DisPatcherCore] P292 档①：CDungeon::limitOfStackableItemInTower
        // 已 bool 化（Main 代落）⇒ 原码为裸 `!f()`（ORIG 0x081c1b68 `call; xor $0x1,%eax;
        // test %al,%al; je`）与裸 `f()`（0x081c1b8e `call; test %al,%al; je`）。
        if (!tower->getCDungeonMGr()->getDungeon()->limitOfStackableItemInTower())
        {
            limitNotOne = true;
        }
        if (tower->getCDungeonMGr()->getDungeon()->limitOfStackableItemInTower())
        {
            for (int i = 0; i < (int)(signed char)req.m_count; ++i)
            {
                CItem* item = G_CDataManager()->find_item(req.m_itemIdx[i]);
                if (item != 0 && item->get_index() == 0x1973)
                {
                    // ORIG lea 0xb58(tower) == &tower->m_timerKeys[7]（CDeathTower.h:254
                    // m_timerKeys[10]@+0xb3c；0xb3c+7*4=0xb58；同 CDeathTower.cpp:807 口径）
                    // ORIG 0x81c1bf0 调嵌套版 CDeathTower::stMapMonsterKillChecker_t
                    // ::setUseSkillMaterial（W 0x0823464c）——勿用全局版（布局不同）。
                    reinterpret_cast<WongWork::CDeathTower::stMapMonsterKillChecker_t*>(
                        &reinterpret_cast<CDeathTowerTimerKeyView*>(tower)->m_timerKeys[7])
                        ->setUseSkillMaterial(GlobalData::s_systemTime_.getCurTickCount());
                }
            }
        }
    }

    // [b23S3C] ORIG +0x151..+0x183 用分支岛 `if(cond){body;return 1;}return 0;`
    // （P56/P198），无 skip 布尔物化——禁中间局部，直接 if 包裹 body 并 return。
    if ((party->m_battleField.m_bloodState == 1) &&
        (user->get_state() == 5 || limitNotOne))
    {
        out.m_9 = 0;
        // [Lane5] P266 cast 形实测劣化（add $0xd + movzbl (%eax) 两步）⇒ 维持直读；
        // 残余 `mov %eax,%edx` 中转需 PacketTypes.h m_itemSpace char→unsigned char（S2 待 Main）。
        out.m_8 = req.m_itemSpace;
        for (int i = 0; i < (int)(signed char)req.m_count; ++i)
        {
            out.m_slotsD2[i] = req.m_position[i];
            out.m_counts19c[i] = req.m_countArr[i];
            out.m_slotsA[i] = req.m_opType[i];
        }
        return true;
    }
    return false;
}

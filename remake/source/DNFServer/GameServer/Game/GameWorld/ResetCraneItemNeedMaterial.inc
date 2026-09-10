void GameWorld::ResetCraneItemNeedMaterial(int param)
{
    G_CGameManager()->GetCraneMinigameManager()->updateCraneItemNeedMaterial(param);
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        // ORIG 0x86d1415-0x86d143d：`cmp $2; setle; test; jne SKIP` + body 尾
        // `jmp END` + SKIP 落点 nop ⇒ goto 形态钉块序（if/else 各写法均被 4.4
        // threading/朝向折叠，唯 goto 复现）；state>2 时 SendCraneItemNeedMaterial
        if (!(user->get_state() > 2))
        {
            goto CRANE_SKIP;
        }
        G_CGameManager()->GetCraneMinigameManager()->SendCraneItemNeedMaterial(user);
    CRANE_SKIP:
        ;
    }

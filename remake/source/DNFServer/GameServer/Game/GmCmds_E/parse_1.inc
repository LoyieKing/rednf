// ORIG parse 0x084ace8a：clear；sscanf(args,"%d%d",&mode,&count)；随后按发起者
// party 校验：无 party 提示；mode==1 清空 passed 列表后压入当前图并开启
// 地图保存；mode∈{2,3} 关闭地图保存。
// b28d-solo-GmCmds_E：P0/P251——ORIG 无 mode/passed/bf/cur 局部槽（m_mode 每次
// 重读成员 `cmp $1` 决策树；getPassedMapInfoList 两次独立调用；GetCurrentMapInfo
// 结果跨 call 保活 %ebx），故改 switch(m_mode) + 全表达式内联。
void CMonsterDropTest::parse()
{
    clear();
    sscanf(m_parser->m_args.c_str(), "%d%d",
           &m_mode, &m_count);

    CUser* user = GetUser();
    if (user != 0)
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party == 0)
        {
            CBoosterTest::SendChatMsg(user, const_cast<char*>("Party\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9"));
        }
        else
        {
            switch (static_cast<unsigned int>(m_mode))
            {
            case 0:
                break;
            case 1:
                static_cast<std::vector<MapInfo>*>(
                    party->getPassedMapInfoList())->clear();
                static_cast<std::vector<MapInfo>*>(
                    party->getPassedMapInfoList())->push_back(
                        *party->m_battleField.GetCurrentMapInfo());
                party->setDungeonMapSaving(true);
                break;
            case 2:
            case 3:
                party->setDungeonMapSaving(false);
                break;
            }
        }
    }
}
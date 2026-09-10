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
            CBoosterTest::SendChatMsg(user, const_cast<char*>("Party가 없습니다"));
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

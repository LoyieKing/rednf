// ==== Dispatcher_MonstercardBind::_bind_process (ORIG 0x081d85fc T) ====
// b28c-L20 SIGFIX: bool Dispatcher_MonstercardBind::_bind_process(...)（P174 串定）
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d85fc orig/ours=187/180 ndiff-blocks=17
//   帧差 sub $74 vs $78 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x74,%esp | movl   $0x0,-0x20(%ebp)
bool Dispatcher_MonstercardBind::_bind_process(
    ENUM_MONSTER_CARD_BIND_TYPE type, ENUM_RARITY rarity,
    IntPairVector& results)   // [P4-P139] 拼写对齐 ORIG prett 串（typedef 见 GameTypes.h:49）
{
    STExpertJobScript* script = static_cast<STExpertJobScript*>(
        G_CDataManager()->GetExpertJobScript(1));
    STExpertJobScript::stMonsterCardBindInfo* info;
    if (type == BIND_TYPE_SPECIAL)
    {
        info = script->GetSpecialBindInfo();
    }
    else if (type == BIND_TYPE_COMMON)
    {
        info = script->GetCommonBindInfo();
    }
    else
    {
        results.clear();
        return false;
    }
    const std::vector<stMonsterCardBindResult>& table = info->m_vec[rarity];
    const int total = info->m_count[rarity];
    // ORIG：table.size() 各次独立调用（不缓存 entryCount）
    if (table.size() == 0)
    {
        // [P4-P139] 手写串 → __PRETTY_FUNCTION__：ORIG prett 直证
        //   _ZZN26Dispatcher_MonstercardBind13_bind_processENS_27ENUM_MONSTER_CARD_BIND_TYPEE11ENUM_RARITYRSt6vectorISt4pairIiiESaIS4_EEE19__PRETTY_FUNCTION__
        //   @0x8bd25e0 size=0x85，串与手写串逐字节相同；换后符号形态与 ORIG 一致。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x44b1, 5);
        trace("MonstercardBind Error : size(%d)", (int)table.size());
        return false;
    }
    if (table.size() != 0 && total != 0)
    {
        unsigned long range = (unsigned long)(total - 1);
        const int pick = m_rand->randInt(range) + 1;
        int accWeight = 0;
        for (size_t i = 0; i < table.size(); ++i)
        {
            if (accWeight < pick && pick <= accWeight + (int)table[i].m_field4)
            {
                results.push_back(std::make_pair(table[i].m_field0, 1));
                break;
            }
            accWeight += (int)table[i].m_field4;
        }
        if (results.empty())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x44ca, 5);
            trace("Dispatcher_MonstercardBind::_bind_process ERROR : result "
                  "is empty");
            return false;
        }
        return true;
    }
    cMyTrace trace(__PRETTY_FUNCTION__, 0x44b6, 5);
    trace("MonstercardBind Error : size(%d), sum(%d)", (int)table.size(),
          total);
    return false;
}
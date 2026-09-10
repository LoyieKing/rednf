std::pair<int, int> CConditionEventManager::_getRequiredParameter(short step)
{
    // b28d-52：ORIG 0x083356a1 key 槽 -0x10 系 find 的匿名实参临时（P236 内联
    // 实参），非具名局部；去掉具名 key 后 it(-0x14)/end(-0xc) 槽序对齐。
    std::pair<int, int> result;
    std::map<int, std::pair<int, int> >::iterator it =
        G_CDataManager()->m_conditionEvent.m_map8.find((int)step);
    if (it != G_CDataManager()->m_conditionEvent.m_map8.end())
    {
        result.first = it->second.first;
        result.second = it->second.second;
        return result;
    }
    result.first = -1;
    result.second = -1;
    return result;

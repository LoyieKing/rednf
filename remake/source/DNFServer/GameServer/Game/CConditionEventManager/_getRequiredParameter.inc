// ORIG T 0x083356a4 _ZN22CConditionEventManager21_getRequiredParameterEs
// sret pair<int,int>：查 m_conditionEvent.m_map8（G_CDataManager()+0x4da0，
// map<int,pair<int,int>>）键=step；命中 → {second.first, second.second}；
// 未命中 → {-1,-1}。ret $0x4（sret 指针随 ret 清栈）。
std::pair<int, int> CConditionEventManager::_getRequiredParameter(short step)
{
    // b28d-52：ORIG 0x083356a1 key 槽 -0x10 系 find 的匿名实参临时（P236 内联
    // 实参），非具名局部；去掉具名 key 后 it(-0x14)/end(-0xc) 槽序对齐。
    std::pair<int, int> result;
    std::map<int, std::pair<int, int> >::iterator it =
        G_CDataManager()->m_conditionEvent.m_map8.find((int)step);
    if (it != G_CDataManager()->m_conditionEvent.m_map8.end())
    {
        // [W15BNearHunt2 2026-09-11 符号轴落地] ORIG 0x083356a4 段发
        // `_ZNKSt17_Rb_tree_iteratorISt4pairIKiS0_IiiEEEdeEv`（operator*），
        // 非 `ptEv`（operator->）⇒ ORIG 源码为 `(*it).second` 形。
        result.first = (*it).second.first;
        result.second = (*it).second.second;
        return result;
    }
    result.first = -1;
    result.second = -1;
    return result;
}
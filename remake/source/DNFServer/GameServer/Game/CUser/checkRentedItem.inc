// ORIG T 0x08677188：在 m_rentalInfo[param] 视图中查找 characNo 的租借记录里
// 是否存在 itemIndex 匹配项。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08677188 | _ZN5CUser15checkRentedItemEjii
bool CUser::checkRentedItem(unsigned int characNo, int itemIndex, int param)
{
    std::map<unsigned int,
             std::vector<InstanceRentalSystem::RentalInfo> > rentalMap;
    GetInstanceRentalInfo(rentalMap, param);

    // b28d-solo-CUser p167：ORIG 0x08677188 用非 const iterator
    //   （vector::begin/end、find_if<iterator>、_Rb_tree_iterator::operator*）；
    //   我方 const_iterator 致 const 版符号 ⇒ 改非 const + `(*it).second`。
    std::map<unsigned int,
             std::vector<InstanceRentalSystem::RentalInfo> >::iterator it =
        rentalMap.find(characNo);
    if (it == rentalMap.end())
        return false;

    std::vector<InstanceRentalSystem::RentalInfo>& infos = (*it).second;
    // CuC P219 求值序旋钮（ORIG 0x08677188 直证）：ORIG 序 = pred(-0x1c) → end(-0x18)
    //   → begin(-0x14) → find_if(-0x20) → end(-0x10) → op!=，即**左操作数先于右**。
    //   4.4 O0 对 `A != B` 恒先求 B（右）；把 `infos.end()` 放左侧即复现 ORIG 全序
    //   （探针 /tmp/CuC/probe/p1.cpp f2 五调用槽 -24/-20/-16/-28/-12 与 ORIG 逐槽同向）。
    //   `!=` 两侧求值互换安全：find_if 只读 vector，end() 结果在 find_if 后仍有效。
    return infos.end() != std::find_if(infos.begin(), infos.end(),
                                       FindRentalInfoByItemIndex(itemIndex));
}
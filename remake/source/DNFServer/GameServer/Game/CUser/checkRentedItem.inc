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
    // b28d-solo-CUser 试形回退（P75 求值序）：具名 pred 局部实测把 map 槽整体平移
    //   （-0x3c→-0x38，ndiff 不减）⇒ 恢复内联实参形，槽/求值序差维持 N82/N86 封顶。
    return std::find_if(infos.begin(), infos.end(),
                        FindRentalInfoByItemIndex(itemIndex)) != infos.end();

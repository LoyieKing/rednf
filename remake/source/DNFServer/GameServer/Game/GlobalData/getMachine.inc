IRentalMachine* InstanceRentalSystem::getMachine(int idx)
{
    // ORIG 0x082703a0：`_Rb_tree_iterator::eq` + `test; je` 到 de 路径、
    // true 分支 `mov $0` 早返岛——源码 `if (it == end) return 0;` + 尾
    // `return it->second`（非三目 ne 形）。
    // ORIG find/eq 为非 const iterator 符号（4findERS5_/...eqERKS5_）——
    // const_iterator 声明会引入 iterator→const_iterator 转换调用。
    std::map<int, IRentalMachine*>::iterator it = m_impl->m_machines.find(idx);
    // ORIG 0x0827039a：eq 的 this = end() 临时槽(-0xc)、实参 = it(-0x10)——
    // 源码写 `end() == it`。
    if (m_impl->m_machines.end() == it)
    {
        return 0;
    }
    // [b28d-solo-GlobalData] P167：ORIG 发 deEv ⇒ `(*it).second`。
    return (*it).second;

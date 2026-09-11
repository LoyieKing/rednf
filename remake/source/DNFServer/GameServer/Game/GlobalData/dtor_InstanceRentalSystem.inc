// [b28d-18 IDENTICAL_AE] 68=68；唯一残差=尾对齐 nop（§6.1 豁免） | ORIG 0x08270110
InstanceRentalSystem::~InstanceRentalSystem()
{
    // ORIG 0x08270110：遍历机器表虚析构；impl 由 auto_ptr D1 释放。
    // ORIG 0x08270162：每轮 delete 后 `it->second = 0`（de 重调 + movl $0）。
    for (std::map<int, IRentalMachine*>::iterator it = m_impl->m_machines.begin();
         it != m_impl->m_machines.end(); ++it)
    {
        // [b28d-solo-GlobalData] P167：ORIG 0x0827016x 发 `_ZNK...deEv`（operator*）
        // 非 `ptEv` ⇒ 源码写 `(*it).second`（it->second 选错实例化，p167 MISSING deEv）。
        delete (*it).second;
        (*it).second = 0;
    }
}
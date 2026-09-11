// Ready（ORIG T 0x082701dc）：遍历机器表；任一为空或其槽3 Ready() 返回非 1
// 即返回 false，全部就绪（含空表）返回 true。
// [b28 判词 N7] 残差=call *%edx↔%eax 换名+对调，主体 56 行同形封顶。
// [b28d-18 IDENTICAL_AE] 56=56 逐条同形；残差仅间接 call 寄存器名（N7 已判） | ORIG 0x082701dc
bool InstanceRentalSystem::Ready()
{
    for (std::map<int, IRentalMachine*>::iterator it = m_impl->m_machines.begin();
         it != m_impl->m_machines.end(); ++it)
    {
        // ORIG 0x08270214：判空条件极性 `jne body` + fall-through `mov $0;jmp
        // end`（技巧 17：if(x==0) return false; 直落形）；`!Ready()` 独立 if
        // （ORIG 0x08270233 xor+test+je 续循环，非 || 短路合并形）。
        // [b28d-solo-GlobalData] P167：ORIG 发 deEv ⇒ `(*it).second`。
        IRentalMachine* machine = (*it).second;
        if (machine == 0)
        {
            return false;
        }
        if (!machine->Ready())
        {
            return false;
        }
    }
    return true;
}
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

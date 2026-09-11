// ORIG _ZN12TutorialData12getDialogStrEjj @0x08ab852e
// ORIG 反汇编：map.find(0x10=param2)、vector.at(0xc=param1)、
// 越界比较 param1 < size —— 即 param1 为 vector 索引、param2 为 map key。
char const* TutorialData::getDialogStr(unsigned int idx, unsigned int key)
{
    DialogMap::iterator it = m_dialogs.find(static_cast<int>(key));
    if (it == m_dialogs.end()) {
        return 0;
    }
    // ORIG 0x08ab85a4：`cmp 0xc(%ebp),%eax; seta %al; test %al,%al; je 0-岛`
    // —— 正极性 `if (idx < size)` 才让 4.4 出 seta + 成功路径 fall-through
    // （写 !(idx<size) 早返出 setbe 且块序相反，P220）。
    if (idx < it->second.size()) {
        return it->second.at(idx)->m_str.c_str();
    }
    return 0;
}
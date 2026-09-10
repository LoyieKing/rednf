void NpcDialogue::clear()
{
    m_field0 = 0;                 // movl $0,(%eax)
    for (int i = 0; i <= 1; ++i)  // ORIG 循环头：jmp 到尾部 cmpl $0x1,setle
        m_field4[i] = 0;          // movl $0,0x4(%eax,%edx,4)
    m_str0c = "";                 // string::operator=(char const*)
    m_str10 = "";                 // string::operator=(char const*)
    m_vec14.clear();              // vector<int>::clear()

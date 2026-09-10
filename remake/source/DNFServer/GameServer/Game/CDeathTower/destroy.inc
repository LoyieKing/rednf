void CAICharacterList::destroy()
{
    // [b28d-D13] ORIG 0x0834a59a：begin→-0x10、end→-0x14 各一次（P13 缓存 end），
    // p@-0xc 后仅 delete 自带守卫（P141，无显式 if(p!=0) 二次判断）。
    std::map<int, CAICharacter*>::iterator it = m_charMap.begin();
    std::map<int, CAICharacter*>::iterator e = m_charMap.end();
    for (; it != e; ++it)
    {
        // ORIG 此处调用 CAICharacter::~CAICharacter()（0x08373b32 W）后
        // operator delete；CAICharacter 在本工程尚未建模（仅前向声明），
        // 其析构当前无可执行语义，先直接释放【推断】。
        // ORIG 0x08373d60 单次 operator-> + `mov %eax,-0xc; mov -0xc,%ebx`
        // （具名局部缓存）。
        // [b28d-solo-CDeathTower] p167：ORIG 调 _Rb_tree_iterator::operator*（非 ->）。
        CAICharacter* p = (*it).second;
        delete p;                // 镜像类成员（vector/string）随析构释放
    }
    // [b28d-D13] ORIG 0x0834a59a 无 clear 调用（P171：ORIG 不写即删；map 由
    // ~CAICharacterList 的隐式成员析构清理）。

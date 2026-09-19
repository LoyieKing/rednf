// [b28d-solo-BlueMarble AE] 关键修复：`int zone` 具名局部改内联右值实参
// `mm.insert(std::make_pair(m_users[i].getBoardZone(), i))`——ORIG zone 槽
// -0x1c 是 make_pair<int,int&> 右值形参的匿名临时物化（非具名局部），4.4 据此
// 把临时区整体对齐 ORIG（zone -0x1c / sret -0x24 / pair -0x2c / insert 临时
// -0x30；179=179 gap 0）；并删除原 P190 float framePad（内联后自然对齐，
// pad 反致整体深移 4）。具名 zone 三形（块内/头部/先声明后赋值）恒落 -0x54
// ⇒ 原 N93/N82附 判词作废。P234 去 `std::move(zone)`（ORIG 无 out-of-line
// move call）保留。ORIG 0x080d7afc
int BlueMarble::setUserGrade(int grade)
{
    std::multimap<int, int, std::greater<int> > mm;
    m_users[grade].setGrade(BlueMarbleUserGrade::T_0);
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        // ORIG 0x080d7b5d/0x080d7b87：==0 物化 sete %al 后 test;jne 出口（body fall，
        // 范式23 goto）。b28c-1：P0/N96 清绑，P168a② 具名 bool + 正向 goto；
        // 声明前置（P198① 两句式）避免 goto 跨初始化。
        if (m_users[i].getUser() == 0)
            goto NEXT0;
        if (m_users[i].getGrade() == 0)
            goto NEXT0;
        // ORIG 0x080d7bb0..：zone 槽 -0x1c；Ghidra `make_pair<int,int&>` +
        // 无 out-of-line call ⇒ 原码是左值 make_pair（P61/P234），
        // `std::move(zone)` 会多发一次 `call std::move<int>`（+5 条）。
        mm.insert(std::make_pair(m_users[i].getBoardZone(), i));
    NEXT0:
        ;
    }
    std::multimap<int, int, std::greater<int> >::iterator it = mm.begin();
    int curGrade = 0;
    int nextGrade = 1;
    int lastZone = 0;
    while (it != mm.end())
    {
        if (it->first == lastZone)
        {
            lastZone = it->first;
            m_users[it->second].setGrade((BlueMarbleUserGrade::T)curGrade);
        }
        else
        {
            lastZone = it->first;
            m_users[it->second].setGrade((BlueMarbleUserGrade::T)nextGrade);
            curGrade = nextGrade;
            ++nextGrade;
        }
        ++it;
    }
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}
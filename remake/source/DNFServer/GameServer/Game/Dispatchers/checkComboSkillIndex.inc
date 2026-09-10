int Dispatcher_ComboSkill::checkComboSkillIndex(short idx)
{
    // G_CDataManager() 返回对象 +0x14 处为组合技列表基址 char*
    // （成员访问形态 mov 0x14(%eax),%eax 与 ORIG 一致；镜像视图同 ComboSkill.cpp）
    struct DataManagerComboListView
    {
        char m_pad[0x14];
        char* m_pComboList;  // +0x14
    };
    std::vector<std::pair<int, int> >* comboList =
        (std::vector<std::pair<int, int> >*)(
            ((DataManagerComboListView*)G_CDataManager())->m_pComboList + 0x48fc);
    for (int i = 0; i <= 5; ++i)
    {
        // ORIG 0x0825f2b9：循环体首 `movswl -0x1c(%ebp),%ebx`（idx 符号扩展
        // 跨 operator[] call 缓存 %ebx），比较 `cmp %eax,%ebx; sete`（技巧22/32）。
        // [b28c-30] P0 清绑：原 register int probe asm("ebx") 绑定已删（N96），
        // 改 short 形参表达式直用（P200：短整表达式作普通实参/比较直传）。
        if (idx == (*comboList)[i].first)
        {
            return 1;
        }
    }
    return 0;

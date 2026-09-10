int CBingo::getQuizValue()
{
    struct QuizValueView
    {
        char m_pad[0xc];
        int m_value;
    };
    // P1：组件视图折叠单指令 disp（mov 0xc(%eax),%eax）。
    return reinterpret_cast<QuizValueView*>(this)->m_value;

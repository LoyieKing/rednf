// ORIG _ZN20GlobalScriptTutorial7getDataEi @0x08ab971e（报告 getData.md 已读）
// 越界返回 0；m_data[index].getDungeonIndex()==0 返回 0，否则返回该元素地址。
// N96 清绑：原 `register bool isZero asm("al")` 已删。ORIG isZero 无槽
// （test %eax;sete %al;test %al,%al;je）⇒ 按 P226扩展/P316① 用语句表达式包住条件。
// 清绑前后 25=25、与 ORIG 逐条全等（仅 .o 重定位地址差）⇒ 残差 0；
// 具名 bool 局部形为 26（+1 槽物化）。
TutorialData* GlobalScriptTutorial::getData(int index)
{
    if (!(index < 11 && index >= 0))
        return 0;
    {
        if (({ m_data[index].getDungeonIndex() == 0; }))
            return 0;
        return &m_data[index];
    }
}
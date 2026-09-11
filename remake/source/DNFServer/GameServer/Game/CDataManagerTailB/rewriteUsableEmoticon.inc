// ---------------------------------------------------------------------------
// rewriteUsableEmoticon(CUser const&, char*) —— ORIG T 0x0835fba2
//   扫描文本中 "\x1e <emotcode>" 三字节模式，表情 code 以 (signed char) 取得
//   后查 m_emoticonList(+0x28)；未找到或 emoticon->isUsable(user)（vtbl+0x08,
//   asm 0835fc31..0x9c 虚调用）为假则把第 3 字节改写 0xff 作废。
//   循环条件 strlen(text)-2 > i（有符号）。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：emo 具名指针提前到函数头声明（ORIG 槽
//   emo(-0x18)/len(-0x14)/i(-0x10)/emoCode(-0xc) 按「先声明深」复现）；
//   循环下标用 int（size_t 会令 text[i] 折成 lea 三连）。ORIG 0x0835fba2。
void CDataManager::rewriteUsableEmoticon(const CUser& user, char* text)
{
    ChattingEmoticonBase* emo;
    if (text == 0)
    {
        return;
    }
    const size_t len = strlen(text);
    for (int i = 0; (int)len - 2 > i; ++i)
    {
        if ((unsigned char)text[i] != 0x1e ||
            (unsigned char)text[i + 1] != ' ')
        {
            continue;
        }
        const int emoCode = (signed char)text[i + 2];  // asm movsbl
        emo = m_emoticonList->find_emoticon(emoCode);
        if (emo == 0)
        {
            text[i + 2] = (char)0xff;
        }
        else if (!emo->isUsable(user))
        {
            text[i + 2] = (char)0xff;
        }
    }
}
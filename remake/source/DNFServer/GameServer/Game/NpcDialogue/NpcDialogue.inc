// ORIG ctor 0x89fb688：+0x0c/+0x10 string 与 +0x14 vector 由编译器默认构造，
// 函数体仅调 clear()（089fb6ba mov this,(%esp); call clear）。
NpcDialogue::NpcDialogue()
{
    clear();
}
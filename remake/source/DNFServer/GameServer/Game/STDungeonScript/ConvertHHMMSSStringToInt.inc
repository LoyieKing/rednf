bool ConvertHHMMSSStringToInt(std::string s, unsigned char& h, unsigned char& m,
                              unsigned char& sec);

// ORIG nm 实证：Clear/C1/C2/D1/D2 全为 **W**（0x0836abc6/0x0836b5e2/0x0836bd5c，
// 尺寸 0x856/0x779/0x789 与我方逐位相同）⇒ 原码是 CDungeon.h 类内 inline 定义。
// 按 Main 已批准的 G_* 单点修法：.cpp 定义加 weak 属性对齐绑定（mangled/尺寸零变化）。
__attribute__((weak)) STDungeonScript::STDungeonScript()
{
    Clear();

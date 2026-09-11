int Arad_Script::GetInt(int row, int col)
{
    // ORIG 0x0817c6e7 lea 形（imul→mov%ecx→mov col→lea (%ecx,%eax,1)→shl→lea）
    // 复现写法：索引拆两乘积 `(cols*row)*0x80 + col*0x80`——4.4 强度归约后逐条
    // 同形（/tmp/l0_probe.cpp GetInt6 实证，batch-25c Lane0 撤销 N18a 封顶归因）。
    char* local_c;
    local_c = cells_ + (cols_ * row) * 0x80 + col * 0x80;
    return std::atoi(local_c);
}
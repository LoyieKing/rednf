// [b14 本波] 循环体第二条 `m_moveSpeed[i+2]=0`：ORIG 0x08a3dc28 为
// `add $0x8,%edx; movl $0,0x8(%eax,%edx,4)`（+2 折进 disp8），4.4 恒
// `lea 0x2(%eax),%edx; add $0x8,%edx; movl $0,(%eax,%edx,4)`（+2 折进索引）。
// 探针 /tmp/b14_probe3.cpp f1..f4（`[i+2]`/`[2+i]`/`(arr+2)[i]`/`*(arr+i+2)` 四形）
// 全部产出 lea 形（后两形更劣 9 条）⇒ 判 N4/P30 索引常量分配跨版本差，封顶。
// 其余 `m_moveSpeed[4]` 清零等本波已补（真语义缺口）。ndiff 5 / gap -1。
void PetScript::clear()
{
    m_idx0 = 0;
    m_name = "";
    m_width1 = 0;
    m_width2 = 0;
    m_floatingHeight = 0;
    m_layer = 1;
    m_recoveryTime = 0;
    m_directionCorrect = true;
    // [b28d-solo-OnlyOrigBatch14] ORIG 0x08a3dc18 循环体为 `m_moveSpeed[i]` +
    // `m_moveSpeed[i+2]`（无 *2 索引 ⇒ 无 lea 2i），且循环变量双初始化
    // （0x08a3dc08/0x08a3dc0f 两条 movl $0,-0xc，P191 配套）；ORIG 随后清
    // +0x30 = m_moveSpeed[4]（我方原缺失，真语义缺口），末尾才清 +0x34 m_gravity。
    int i = 0;
    for (i = 0; i <= 1; ++i)
    {
        m_moveSpeed[i] = 0;
        // [b28d-solo-OnlyOrigBatch14] ORIG 0x08a3dc28 形 `mov i,%edx; ...
        // add $0x8,%edx; movl $0,0x8(%eax,%edx,4)`（+2 折进 disp）；4.4 恒把
        // +2 折进索引 `lea 0x2(%eax),%edx`（P30/N4 索引常量分配差），
        // `(m_moveSpeed+2)[i]` 形更劣（73）。⇒ N 封顶。
        // [P1T1 本波穷举] i+2 索引五形探针（/tmp/p1/probe_small.cpp c1..c5：
        // `m[i+2]`/具名 `const int k=i+2`/`m[2+i]`/`*(m+i+2)`/register i）**全部**
        // 产 `lea 0x2(%eax),%edx`；ORIG 0x08a3dc2b 的 `mov i,%edx; add $0x8,%edx`
        // +disp 0x8 形属 4.1 index 常量分配 ⇒ 维持封顶。
        m_moveSpeed[i + 2] = 0;
    }
    m_moveSpeed[4] = 0;
    m_basicMotion = "";
    m_etcMotion.clear();
    m_attackInfo.clear();
    m_intData.clear();
    m_stringData.clear();
    m_directoryPath = "";
    m_gravity = 0;
}

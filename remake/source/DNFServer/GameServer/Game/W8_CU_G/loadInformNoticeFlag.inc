// b28d-solo-W8_CU_G: NEAR/N — P285 空 else{return;} 复现 ORIG 0x0868f900 尾
// `jmp;nop` 岛（ndiff 13→3）。b28d-W8 复验：28/29（ORIG 28），残 1 条 = 循环体
// ORIG 单段 `movzbl (%edx),%ecx`，4.4 恒 `movzbl (%edx),%edx; mov %edx,%ecx` 两段
//（N18a 反向）；b28d-W8b 复测三形（隐式转换 / 显式 (unsigned char) cast /
// P266 `*reinterpret_cast<unsigned char*>(src + i)`）同为 29 ⇒ 封 N，不再试形。
void CUser::loadInformNoticeFlag(char* src)
{
    resetInformNoticeFlag();
    if (src != 0)
    {
        for (int i = 0; i < 0x10; ++i)
        {
            // b28d-W8: ORIG 0x0868f929 单段 `movzbl (%edx),%ecx`；4.4 恒
            // `movzbl (%edx),%edx; mov %edx,%ecx` 两段（+1 条，N18a 反向）；
            // P266 uchar 左值读旋钮 / 隐式转换两形实测同为 29 ⇒ 封 N。
            // b28d-W8b 复测：显式 (unsigned char) cast 无效（同 29）⇒ P266 左值读旋钮。
            // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18a/N96）→ 试=**地址加法形**：
            //   ORIG 0x0868f92x `mov -0xc(%ebp),%edx; add 0xc(%ebp),%edx`（2 条：i 先入
            //   reg、再 `add src(内存)`）；源侧 `src + i`（指针+int）4.4 恒发
            //   `mov 0xc(%ebp),%ecx; mov -0xc(%ebp),%edx; lea (%ecx,%edx,1),%edx`（3 条）。
            //   改用 **int 算术形** `(int)src + i` 后 4.4 发 `mov 0xc(%ebp),%edx;
            //   add -0xc(%ebp),%edx`（2 条）⇒ **DIFF→NEAR**（28/29 不变但残差由
            //   lea 3 条降为 1 个 hunk：仅剩 `add` 的寄存器/内存操作数分配序
            //   （ORIG i→%edx + add src；我方 src→%edx + add i，交换律等价的 4.1/4.4
            //   选择差）⇒ 维持封顶）。试否：`(unsigned char)src[i]` 形实测劣化
            //   （29→30，+1 条 lea 索引链）已弃；`i + (int)src` 交换序实测与上式同码。
            // [LaneG 2026-09-11 复验] 交换书写序 `i + (int)src` 已再测：读数逐值
            // 不变（仍 `mov 0xc(%ebp),%edx; add -0xc(%ebp),%edx`）⇒ 与注①同码，
            // 4.4 对两书写序恒「src 先入目标寄存器 + add i 内存操作数」。
            // ORIG `mov -0xc(%ebp),%edx; add 0xc(%ebp),%edx`（i 先入 + add src）
            // 不可翻（N18a 反向/求值序族），维持封顶。
            m_informNoticeFlags[i] =
                *reinterpret_cast<unsigned char*>(reinterpret_cast<int>(src) + i);
        }
        sendInformNoticeFlag();
    }
    else
    {
        return;
    }
}
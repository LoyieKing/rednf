void WindowsExeFile::Sort()                                  // 0x0857415c
{
    // [b28c-lane17] ORIG 0x08574171：`test %eax; sete %al; test %al; jne <nop 岛>`
    // + body 尾 fall-through 到该岛 ⇒ 负极判定 + goto 尾空标签（P223）。
    if (m_commonSections.size() == 0)
        goto L_empty;
    qsort(&m_commonSections[0], m_commonSections.size(),
          sizeof(WindowsExeConstantSection*),
          CompareWindowsExeConstantSectionsForSort);
L_empty:;

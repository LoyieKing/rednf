// P223（b28）：ORIG `jg nop岛; body; jmp leave; nop; leave` 空块中继岛，
// 复现形=「if(越界) goto end; body; end:;」goto 尾空标签。
void CDataManager::set_original_dimensionInout(int idx, char value)
{
    if (idx > 5)
        goto end;
    m_originalDimensionInout[idx] = value;
end:
    ;
}
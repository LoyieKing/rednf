Inven_Item* CCargo::GetCargoRef(int idx)
{
    double framePad[2];              // ORIG sub $0x18：+0x10 帧撑（P190 double 对齐撑帧）
    (void)&framePad[0];
    if (!IsOperatorable(idx))
        return 0;
    return &m_pItems[idx];
}
// ORIG T 0x086438fc _ZN13TOD_UserState13setEnterLayerERK9TOD_Layer（0x12，
// LinkFinal2 核销）：仅写当日层 +0x14 = layer.m_layer（objdump 逐指令转写）。
void TOD_UserState::setEnterLayer(const TOD_Layer& layer)
{
    m_data.m_todayLayer = layer;    // ORIG movzwl (%edx); mov %dx,0x14(%eax)
}
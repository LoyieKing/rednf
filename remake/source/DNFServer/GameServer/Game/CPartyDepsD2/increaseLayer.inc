// ---------------------------------------------------------------------------
// TOD_Layer::increaseLayer() —— ORIG T 0x085fe824：m_layer + 1；超过 100 时
//   resetLayer()（m_layer 置 1）。
// ---------------------------------------------------------------------------
void TOD_Layer::increaseLayer()
{
    ++m_layer;
    if (m_layer > 100)
        resetLayer();
}
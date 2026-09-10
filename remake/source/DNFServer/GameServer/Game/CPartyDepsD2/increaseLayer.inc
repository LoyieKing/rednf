void TOD_Layer::increaseLayer()
{
    ++m_layer;
    if (m_layer > 100)
        resetLayer();

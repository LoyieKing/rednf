// [b28d-solo-CBossStage R2] 随 CDungeon.h 声明同步（uchar→bool，调用点 P292 判据）。
bool CDungeon::hasTowerItemDrop() const
{
    return m_hasTowerItemDrop;
}
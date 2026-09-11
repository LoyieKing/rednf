// 内联成员无调用点不产出符号；此处构造/析构/clear 强制实例化。
void force_emit()
{
    tagRecommendNewbieOption t;
    t.clear();
}
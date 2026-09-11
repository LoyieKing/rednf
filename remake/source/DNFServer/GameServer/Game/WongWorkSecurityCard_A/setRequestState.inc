void CSecurityCard::setRequestState(ENUM_REQUEST_STATE state)
{
    // P95 视图访问复现 ORIG「先 this 后值」装载序（探针 /tmp/setter_probe.cpp）。
    reinterpret_cast<CSecurityCard44View*>(this)->m_requestState44 =
        static_cast<int>(state);
}
int Arad_Mileage_Data::find(int ipgNo)
{
    // ORIG 0x81a3668：按 ipgNo 找 Data，命中返回 percent，未命中返回 -1。
    for (std::vector<Data>::iterator it = m_mileageList.begin();
         it != m_mileageList.end(); ++it)
        if (it->ipgNo == ipgNo)
            return it->percent;
    return -1;
}
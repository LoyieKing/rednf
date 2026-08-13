#ifndef NSL_IAREA_H_
#define NSL_IAREA_H_

#include <map>

namespace nsl {

class ISession;

typedef std::map<unsigned int, ISession*> MAP_OBJECTS;
typedef std::map<unsigned int, ISession*>::iterator MAP_OBJECTS_ITER;

class IArea
{
public:
    IArea(int mapIndex);
    virtual ~IArea() = 0;
    virtual void setBackServerKey(int key) = 0;
    virtual int getBackServerKey() = 0;
    virtual void unregist(unsigned int id);
    int getMemberNum();
    bool regist(unsigned int id, ISession* obj);
    void notifyAllAreaMember();
    bool isIterEnd(MAP_OBJECTS_ITER iter);
    MAP_OBJECTS_ITER getBeginIter();
    ISession* getValueFromIter(MAP_OBJECTS_ITER iter);
    ISession* getMemberAt(unsigned int id);
    ISession* getMemberFirst();

    MAP_OBJECTS mMapObj;
    MAP_OBJECTS_ITER mMapIter;
    int mMapIndex;
};

} // namespace nsl

#endif // NSL_IAREA_H_

#ifndef  TREE_INC
#define  TREE_INC

#include "GeometryObject.h"

/* \brief Represent a tree*/
class Tree : public GeometryObject
{
    public:
        /* \brief The constructor.*/
        Tree();

    private:
    	void addLeaf(GeometryObject* branch, float size);
    	void addBranch(GeometryObject* branch, float size, float lenght, float angle, float inclination, bool isEnd);
    	void addNode(GeometryObject* branch, float size, float lenght, int nbBranches, bool isEnd);   
};

#endif

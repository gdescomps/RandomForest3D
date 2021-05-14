#ifndef  TREE_INC
#define  TREE_INC

#include "GeometryObject.h"

/* \brief Represent a tree*/
class Tree : public GeometryObject
{
    public:
        /* \brief The constructor.*/
        Tree(float size, float height, int nbNode);

    private:
    	void addLeaf(GeometryObject* branch, float size, float parentHeight);
    	void addBranch(GeometryObject* branch, float size, float height, float parentHeight, float lenght, float angle, float inclination, bool isEnd);
    	void addNode(GeometryObject* branch, float size, float height, float parentHeight, float branchesLenght, int nbBranches, bool isEnd);

};

#endif

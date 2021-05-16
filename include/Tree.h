#ifndef  TREE_INC
#define  TREE_INC

#include "GeometryObject.h"

/* \brief Represent a tree*/
class Tree : public GeometryObject
{
    public:
        /* \brief Constructor.*/
        Tree(float size, float height, int nbNode);

        /* \brief Random constructor.*/
        Tree();

    private:
    	void addLeaf(GeometryObject* branch, float size, float parentHeight);
    	void addBranch(GeometryObject* branch, float size, float height, float parentHeight, float lenght, float angle, float inclination, bool isEnd, float leafSize);
    	void addNode(GeometryObject* branch, float size, float height, float parentHeight, float branchesLenght, int nbBranches, bool isEnd);
    	
    	void addRandomNode(GeometryObject* branch, float size, float height, float parentHeight, float branchesLenght, int nbBranches, bool isEnd);

};

#endif

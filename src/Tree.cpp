#include "Tree.h"

#include "Geometry.h"

#include "Sphere.h"
#include "Cylinder.h"

Tree::Tree(float size, float height, int nbNode)
{
	Cylinder cylinder(32);
	
	this->m_vao = generateVAO(cylinder);
    this->m_nbVertices = cylinder.getNbVertices();
    this->setTextureId(0);
    
    this->transform(local, rotate, glm::vec3(1, 0, 0), 90.0f);
    this->transform(local, scale, glm::vec3(0.2f, 0.2f, height));
    
    addNode(this, size*0.5f, height, height, 0.2f*height, 6, false); // Top node


    int nbIntermediateNode = nbNode-1;

    for (int i = 0; i < nbIntermediateNode; ++i)
    {
        addNode(this, size*(0.8f-(0.3*i/nbNode)), (height/2/nbIntermediateNode*i)+(height/2), height, 0.2f*height+(0.1f*i/nbNode), 6, false);
    }

    this->transform(relative, translate, glm::vec3(0.0f, height*0.5f, 0.0f));
}

Tree::Tree()
{

    float age = rand() % 3;

    float size;
    float height;
    int nbNode;

    if(age >= 1){

        size = 2.0f;
        height = 4.0f;
        nbNode = 3;

    }
    else{

        size = 1.0f;
        height = 2.0f;
        nbNode = 2;

    }

    float variation = (rand() % 10 + 5)*0.1;
    size=size*variation;
    height=height*variation;

    Cylinder cylinder(32);
    
    this->m_vao = generateVAO(cylinder);
    this->m_nbVertices = cylinder.getNbVertices();
    this->setTextureId(0);
    
    this->transform(local, rotate, glm::vec3(1, 0, 0), 90.0f);
    this->transform(local, scale, glm::vec3(0.2f, 0.2f, 1.0f));
    this->transform(local, scale, glm::vec3(size, size, height));
    
    int topNodeNbBranches = rand() % 6 + 1 ;

    addRandomNode(this, size*0.5f, height*0.9f, height, 0.2f*height, topNodeNbBranches, false); // Top node


    int nbIntermediateNode = nbNode-1;

    for (int i = 0; i < nbIntermediateNode; ++i)
    {
        int nodeNbBranches = rand() % 6 + 1 ;
        addRandomNode(this, size*(0.8f-(0.3*i/nbNode)), (height/2/nbIntermediateNode*i)+(height/2), height, 0.2f*height+(0.1f*i/nbNode), nodeNbBranches, false);
    }

    this->transform(relative, translate, glm::vec3(0.0f, height*0.5f, 0.0f));
}

void Tree::addNode(GeometryObject* branch, float size, float height, float parentHeight, float branchesLenght, int nbBranches, bool isEnd){
    float step = 360.0f/nbBranches;
    for (int i = 0; i < nbBranches; ++i)
    {
        addBranch(branch, size, height, parentHeight, branchesLenght, step*i, 45.0f, isEnd, 0.2f);
        
    }
}

void Tree::addRandomNode(GeometryObject* branch, float size, float height, float parentHeight, float branchesLenght, int nbBranches, bool isEnd){
    float step = 360.0f/nbBranches;
    for (int i = 0; i < nbBranches; ++i)
    {
        float leafSize = (rand() % 2 + size*4)*0.1;
        float inclination = (rand() % 60 + 15);
        float branchLenght = (rand() % (int)(50*branchesLenght))*0.01 + branchesLenght;
        float branchHeight = (rand() % (int)(10*height))*0.01 + height;

        addBranch(branch, size, branchHeight, parentHeight, branchLenght, step*i, inclination, isEnd, leafSize); 
    }
}

void Tree::addBranch(GeometryObject* branch, float size, float height, float parentHeight, float lenght, float angle, float inclination, bool isEnd, float leafSize){
    Cylinder cylinder(32);

    GeometryObject subBranch(cylinder);
    subBranch.setTextureId(0);

    subBranch.transform(local, rotate, glm::vec3(1, 0, 0), 90.0f);
    subBranch.transform(local, scale, glm::vec3(0.2f, 0.2f, 1.0f));
    subBranch.transform(local, scale, glm::vec3(size, size, lenght));
    subBranch.transform(relative, translate, glm::vec3(0.0f, -parentHeight*0.5f, 0.0f));
    subBranch.transform(relative, translate, glm::vec3(0.0f, height-(size*0.2f), 0.0f));
    subBranch.transform(relative, rotate, glm::vec3(0, 1, 0), angle);
    subBranch.transform(relative, rotate, glm::vec3(1, 0, 0), inclination);
    subBranch.transform(relative, translate, glm::vec3(0.0f, lenght*0.5f, 0.0f));

    if(isEnd){
        addLeaf(&subBranch, leafSize, lenght);
    }
    else{
        addRandomNode(&subBranch, size*0.5f, lenght, lenght, lenght*0.7f, 5, true);
    }

    branch->getChildren()->push_back(subBranch);
}


void Tree::addLeaf(GeometryObject* branch, float size, float parentHeight){
    Sphere sphere(32,32);
    GeometryObject leaf(sphere);
    leaf.setTextureId(1);

    leaf.transform(local, scale, glm::vec3(size, size, size));
    leaf.transform(relative, translate, glm::vec3(0.0f, parentHeight*0.5f, 0.0f));

    branch->getChildren()->push_back(leaf);
}

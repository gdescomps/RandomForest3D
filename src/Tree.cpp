#include "Tree.h"

#include "Geometry.h"

#include "Sphere.h"
#include "Cylinder.h"

Tree::Tree()
{
	Cylinder cylinder(32);
	
	this->m_vao = generateVAO(cylinder);
    this->m_nbVertices = cylinder.getNbVertices();

    
    this->transform(local, rotate, glm::vec3(1, 0, 0), 90.0f);
    this->transform(local, scale, glm::vec3(0.2f, 0.2f, 1.0f));
    
    addNode(this, 0.5f, 0.6f, 6, false);

}

void Tree::addNode(GeometryObject* branch, float size, float lenght, int nbBranches, bool isEnd){
    float step = 360.0f/nbBranches;
    for (int i = 0; i < nbBranches; ++i)
    {
        addBranch(branch, size, lenght, step*i, 45.0f, isEnd);
        
    }
}

void Tree::addBranch(GeometryObject* branch, float size, float lenght, float angle, float inclination, bool isEnd){
    Cylinder cylinder(32);

    GeometryObject subBranch(cylinder);
    subBranch.transform(local, rotate, glm::vec3(1, 0, 0), 90.0f);
    subBranch.transform(local, scale, glm::vec3(0.2f, 0.2f, 1.0f));
    subBranch.transform(local, scale, glm::vec3(size, size, lenght));
    subBranch.transform(relative, translate, glm::vec3(0.0f, lenght*0.5f, 0.0f));
    subBranch.transform(relative, rotate, glm::vec3(0, 1, 0), angle);
    subBranch.transform(relative, rotate, glm::vec3(1, 0, 0), inclination);
    subBranch.transform(relative, translate, glm::vec3(0.0f, lenght*0.5f, 0.0f));

    if(isEnd){
        addLeaf(&subBranch, 1.0f);
    }
    else{
        addNode(&subBranch, size*0.5, lenght*0.7, 5, true);
    }

    branch->getChildren()->push_back(subBranch);
}

void Tree::addLeaf(GeometryObject* branch, float size){
    Sphere sphere(32,32);
    GeometryObject feuille1(sphere);
    feuille1.transform(local, scale, glm::vec3(0.20f, 0.20f, 0.20f));
    feuille1.transform(relative, translate, glm::vec3(0.0f, 0.25f, 0.0f));

    branch->getChildren()->push_back(feuille1);
}
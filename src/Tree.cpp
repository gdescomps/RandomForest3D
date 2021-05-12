#include "Tree.h"

#include "Geometry.h"

#include "Sphere.h"
#include "Cylinder.h"

Tree::Tree()
{
	Cylinder cylinder(32);
	Sphere sphere(32,32);

	this->m_vao = generateVAO(cylinder);
    this->m_nbVertices = cylinder.getNbVertices();

    
    this->transform(local, rotate, glm::vec3(1, 0, 0), 90.0f);
    this->transform(local, scale, glm::vec3(0.2f, 0.2f, 1.0f));
    
    for (int nbBranches = 0; nbBranches < 4; ++nbBranches)
    {

        GeometryObject branche1(cylinder);
        branche1.transform(local, rotate, glm::vec3(1, 0, 0), 90.0f);
        branche1.transform(local, scale, glm::vec3(0.2f, 0.2f, 1.0f));
        branche1.transform(local, scale, glm::vec3(0.5f, 0.5f, 0.7f));
        branche1.transform(relative, rotate, glm::vec3(0, 1, 0), 90.0f*nbBranches);
        branche1.transform(relative, rotate, glm::vec3(1, 0, 0), 45.0f);
        branche1.transform(relative, translate, glm::vec3(0.0f, 0.4f, 0.0f));
        

        for (int nbBrindilles = 0; nbBrindilles < 4; ++nbBrindilles)
        {
            GeometryObject feuille1(sphere);
            feuille1.transform(local, scale, glm::vec3(0.20f, 0.20f, 0.20f));
            feuille1.transform(relative, translate, glm::vec3(0.0f, 0.25f, 0.0f));

            GeometryObject brindille1(cylinder);
            brindille1.transform(local, rotate, glm::vec3(1, 0, 0), 90.0f);
            brindille1.transform(local, scale, glm::vec3(0.2f, 0.2f, 1.0f));
            brindille1.transform(local, scale, glm::vec3(0.2f, 0.2f, 0.4f));
            brindille1.transform(relative, rotate, glm::vec3(0, 1, 0), 90.0f*nbBrindilles );
            brindille1.transform(relative, rotate, glm::vec3(1, 0, 0), 45.0f );
            brindille1.transform(relative, translate, glm::vec3(0.0f, 0.2f, 0.0f));

            brindille1.getChildren()->push_back(feuille1);

            branche1.getChildren()->push_back(brindille1);
        }

        this->getChildren()->push_back(branche1);
    }

}
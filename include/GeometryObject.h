#ifndef  GEOMETRYOBJECT_INC
#define  GEOMETRYOBJECT_INC

//OpenGL Libraries
#include <GL/glew.h>
#include <GL/gl.h>

//GML libraries
//Necessary includes for both using glm::<type> but also for the corresponding functions
//The documentation of glm is a bit hard to read because it is based on what is called
//,→ template. Send me an email if you can’t use it properly for a specific use.
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>


#include "Geometry.h"

#include <vector>

enum Transformation { translate, rotate, scale };
enum TransformationType { local, relative };

/* \brief Represent a geometry*/
class GeometryObject
{
    public:
        /* \brief The constructor.*/
        GeometryObject(const Geometry& geometry);

        /* \brief Get how many vertices this geometry contains
         * \return the number of vertices this geometry contains*/
        int getNbVertices() const {return m_nbVertices;}

        GLuint getVAO() const {return m_vao;}

        glm::mat4 getPropagatedMatrix() {return m_propagatedMatrix;}

        glm::mat4 getLocalMatrix() {return m_localMatrix;}    

        std::vector<GeometryObject>* getChildren() {return &m_children;}

        static GLuint generateVAO(const Geometry& geometry);

        void transform(TransformationType type, Transformation transformation, glm::vec3 vector, float angle=0);

    protected: 
        GeometryObject(){}

        GLuint m_vao;
        int m_nbVertices = 0;
        glm::mat4 m_propagatedMatrix = glm::mat4(1.0f);
        glm::mat4 m_localMatrix      = glm::mat4(1.0f);
        std::vector<GeometryObject> m_children;
};

#endif

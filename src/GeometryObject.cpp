#include "GeometryObject.h"

#define INDICE_TO_PTR(x) ((void*)(x))


GLuint GeometryObject::generateVAO(const Geometry& geometry){
    
    // GLuint VBO, VAO;
    // glGenVertexArrays(1, &VAO);
    // glGenBuffers(1, &VBO);

    // glBindVertexArray(VAO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // glBufferData(GL_ARRAY_BUFFER, 2 * geometry.getNbVertices() * sizeof(float)*3, NULL, GL_DYNAMIC_DRAW); 
    
    // glBufferSubData(GL_ARRAY_BUFFER, 0,                  geometry.getNbVertices()*sizeof(float)*3, geometry.getVertices());
    // glBufferSubData(GL_ARRAY_BUFFER, geometry.getNbVertices()*sizeof(float)*3,  geometry.getNbVertices()*sizeof(float)*3, geometry.getNormals());
    // //glBufferSubData(GL_ARRAY_BUFFER, 3*3sizeof(float)*nbVertices, 2*sizeof(float)*nbVertices, uvData);
    
    // glVertexAttribPointer(0, 3, GL_FLOAT, 0, 3*sizeof(float), 0); 
    // glEnableVertexAttribArray(0); //Enable "vPosition"
    
    // glVertexAttribPointer(1, 3, GL_FLOAT, 0, 3*sizeof(float), INDICE_TO_PTR(geometry.getNbVertices()*3*sizeof(float))); //Convert an indice to void* : (void*)(x)
    // glEnableVertexAttribArray(1); //Enable"vColor"
    GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, (2 + 3 + 3) * sizeof(float) * geometry.getNbVertices(), 0, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * geometry.getNbVertices(), geometry.getVertices());
        glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * geometry.getNbVertices(), 3 * sizeof(float) * geometry.getNbVertices(), geometry.getNormals());
        glBufferSubData(GL_ARRAY_BUFFER, 2 * 3 * sizeof(float) * geometry.getNbVertices(), 2 * sizeof(float) * geometry.getNbVertices(), geometry.getUVs());

        GLuint VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //GLint vPosition = glGetAttribLocation(shader->getProgramID(), "vPosition");

        glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);    //0 pour vPositions
        glEnableVertexAttribArray(0);

    //GLint vPosition = glGetAttribLocation(shader->getProgramID(), "vPosition");

        glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, INDICE_TO_PTR(3 * geometry.getNbVertices() * sizeof(float)));    //1 pour vNormals
        glEnableVertexAttribArray(1);


        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR((3 + 3) * sizeof(float) * geometry.getNbVertices()));
        glEnableVertexAttribArray(2);
        glBindTexture(GL_TEXTURE_2D, 2);
        // glVertexAttribPointer(2, 2, GL_FLOAT, 0, 0, INDICE_TO_PTR(2*3 * geometry.getNbVertices() * sizeof(float)));    //2 pour UVs
        // glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0); //Close the buffer

    glBindVertexArray(0);
    return VAO;
}


GeometryObject::GeometryObject(const Geometry& geometry) {
	
    this->m_vao = generateVAO(geometry);
    this->m_nbVertices = geometry.getNbVertices();

}

void GeometryObject::transform(TransformationType type, Transformation transformation, glm::vec3 vector, float angle){
	glm::mat4* matrix;
	switch (type) {
		
		case local:
			matrix = &this->m_localMatrix;
			break;

		case relative: 
			matrix = &this->m_propagatedMatrix;
			break;
	}

	switch (transformation) {
		case translate:
			*matrix = glm::translate(*matrix, vector);
			break;
		case rotate:
			*matrix = glm::rotate(*matrix, glm::radians(angle), vector);
			break;
		case scale:
			*matrix = glm::scale(*matrix, vector);
			break;
	}
	
}
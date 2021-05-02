//SDL Libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

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

#include <vector>
#include <stack>

#include "Shader.h"
#include "Cube.h"
#include "Sphere.h"


#include "logger.h"

#define WIDTH     800
#define HEIGHT    600
#define FRAMERATE 60
#define TIME_PER_FRAME_MS  (1.0f/FRAMERATE * 1e3)
#define INDICE_TO_PTR(x) ((void*)(x))

struct GeometryObject
{
    GLuint vao;
    int nbVertices;
    glm::mat4 propagatedMatrix = glm::mat4(1.0f);
    glm::mat4 localMatrix      = glm::mat4(1.0f);
    std::vector<GeometryObject*> children;
};

GLuint generateVAO(const Geometry& geometry){
    //We generate our buffer
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //We fill this buffer as a GL_ARRAY_BUFFER (buffer containing vertices (points) information). 
    //Remind to close this buffer for not misusing it(glBindBuffer(GL_ARRAY_BUFFER, 0);)
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //2 coordinates per UV, 3 per normal and 3 per position. We do not yet copy these data (hence the NULL)
    glBufferData(GL_ARRAY_BUFFER, 2 * geometry.getNbVertices() * sizeof(float)*3, NULL, GL_DYNAMIC_DRAW); 

    //Copy one by one the data (first positions, then normals and finally UV).
    //We remind that we do not necessarily need all of these variables, and that other variables may be needed for your usecase
    //parameters : Target, buffer offset, size to copy, CPU data.
    
    //We consider that each data are typed « float* » with sizeof(float)*nbVertices*nbCoordinate bytes where nbCoordinate = 2 or 3 following the number of components per value for this variable
    glBufferSubData(GL_ARRAY_BUFFER, 0,                  geometry.getNbVertices()*sizeof(float)*3, geometry.getVertices());
    glBufferSubData(GL_ARRAY_BUFFER, geometry.getNbVertices()*sizeof(float)*3,  geometry.getNbVertices()*sizeof(float)*3, geometry.getNormals());
    //glBufferSubData(GL_ARRAY_BUFFER, 3*3sizeof(float)*nbVertices, 2*sizeof(float)*nbVertices, uvData);
    

    //Work with vPosition
    //GLint vPosition = glGetAttribLocation(shader->getProgramID(), "vPosition");
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 3*sizeof(float), 0); //It is here that you
    // ,→ select how the Shader reads the VBO. Indeed the 5th parameter is called
    // ,→ "stride" : it is the distance in bytes between two values for the same
    // ,→ kind or variable. If the values are side-by-side, stride == 0. Here we
    // ,→ need to set to 3*sizeof(float) for the first version of the VBO seen in
    // ,→ class.
    glEnableVertexAttribArray(0); //Enable "vPosition"
    //Work with vColor
    //GLint vColor = glGetAttribLocation(shader->getProgramID(), "vNormal");
    //Colors start at 9*sizeof(float) (3*nbVertices*sizeof(float)) for the second
    // ,→ version of the VBO. For the first version of the VBO, both the stride
    // ,→ and the offset should be 3*sizeof(float) here
    glVertexAttribPointer(1, 3, GL_FLOAT, 0, 3*sizeof(float), INDICE_TO_PTR(geometry.getNbVertices()*3*sizeof(float))); //Convert an indice to void* : (void*)(x)
    glEnableVertexAttribArray(1); //Enable"vColor"


    glBindBuffer(GL_ARRAY_BUFFER, 0); //Close the buffer

    glBindVertexArray(0);
    return VAO;
}

void draw(Shader* shader, std::stack<glm::mat4>& mvpStack, GeometryObject object){

    // glBindBuffer(GL_ARRAY_BUFFER, myBuffer);

    
    

    glUseProgram(shader->getProgramID());
    glBindVertexArray(object.vao); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
            
            GLint uMVP = glGetUniformLocation(shader->getProgramID(), "uMVP"); //Get the "uScale" location (ID)
            glm::mat4 mvp = mvpStack.top() * object.propagatedMatrix * object.localMatrix;
            glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp)); //Set "uScale" at 0.5f. Remark : we use glUniform1f for sending a
            // ,→ (1) float (f). For other kinds of uniform (integers, vectors, etc.) see the
            // ,→ documentation

            glDrawArrays(GL_TRIANGLES, 0, object.nbVertices); //Draw the triangle (three points which
            // ,→ starts at offset = 0 in the VBO). GL_TRIANGLES tells that we are reading
            // ,→ three points per three points to form a triangle. Other kind of "
            // ,→ reading" exist, see glDrawArrays for more details.
            // glBindBuffer(GL_ARRAY_BUFFER, 0); //Close the VBO (not mandatory but recommended,→ for not modifying it accidently).
            
            mvpStack.push(mvpStack.top() * object.propagatedMatrix);
            for(GeometryObject* child : object.children)
                draw(shader, mvpStack, *child);
            mvpStack.pop(); 

            
    glBindVertexArray(0);
    glUseProgram(0); //Close the program. This is heavy for the GPU. In reality we do this
    // ,→ only if we have to CHANGE the shader (hence we cache the current in-use shader)
    // ,→ . For this course however the performances are not so important.

    // glBindBuffer(GL_ARRAY_BUFFER, 0); //Close the VBO (not mandatory but recommended,→ for not modifying it accidently).
}

int main(int argc, char *argv[])
{
    ////////////////////////////////////////
    //SDL2 / OpenGL Context initialization : 
    ////////////////////////////////////////
    
    //Initialize SDL2
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        ERROR("The initialization of the SDL failed : %s\n", SDL_GetError());
        return 0;
    }

    //Create a Window
    SDL_Window* window = SDL_CreateWindow("VR Camera",                           //Titre
                                          SDL_WINDOWPOS_UNDEFINED,               //X Position
                                          SDL_WINDOWPOS_UNDEFINED,               //Y Position
                                          WIDTH, HEIGHT,                         //Resolution
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); //Flags (OpenGL + Show)

    //Initialize OpenGL Version (version 3.0)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    //Initialize the OpenGL Context (where OpenGL resources (Graphics card resources) lives)
    SDL_GLContext context = SDL_GL_CreateContext(window);

    //Tells GLEW to initialize the OpenGL function with this version
    glewExperimental = GL_TRUE;
    glewInit();


    //Start using OpenGL to draw something on screen
    glViewport(0, 0, WIDTH, HEIGHT); //Draw on ALL the screen

    //The OpenGL background color (RGBA, each component between 0.0f and 1.0f)
    glClearColor(0.0, 0.0, 0.0, 1.0); //Full Black

    glEnable(GL_DEPTH_TEST); //Active the depth test


    //From here you can load your OpenGL objects, like VBO, Shaders, etc.

    Cube cube;
    Sphere sphere(10,10);


    GeometryObject head;
    head.nbVertices = sphere.getNbVertices();
    head.vao = generateVAO(sphere);
    head.propagatedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.60f, 0.0f));

    GeometryObject body;
    body.nbVertices = cube.getNbVertices();
    body.vao = generateVAO(cube);
    body.localMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.30f, 1.0f, 0.30f));
    body.children.push_back(&head);


    //Shaders
    FILE* vertFile = fopen("Shaders/color.vert", "r");
    FILE* fragFile = fopen("Shaders/color.frag", "r");
 
    Shader* shader = Shader::loadFromFiles(vertFile, fragFile); //Load the files and, create the GPU program
    fclose(vertFile);
    fclose(fragFile);
    if(shader == NULL)
    {
    //Print an error message (an error occured while compiling). The message is also,displayed on screen by loadFromFiles function
    return EXIT_FAILURE;
    }
      
    float angleSquare = 0;

    bool isOpened = true;
    //Main application loop
    bool xRay = false;
    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glm::mat4 cameraMatrix(1.0f);
    
    while(isOpened)
    {
        //Time in ms telling us when this frame started. Useful for keeping a fix framerate
        uint32_t timeBegin = SDL_GetTicks();

        glm::mat4 projectionMatrix(1.0f);
        

        //Fetch the SDL events
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_WINDOWEVENT:
                    switch(event.window.event)
                    {
                        case SDL_WINDOWEVENT_CLOSE:
                            isOpened = false;
                            break;
                        default:
                            break;
                    }
                    break;

                case SDL_KEYDOWN:
                    /* Check the SDLKey values and move change the coords */
                    switch (event.key.keysym.sym) {
                        case SDLK_x:
                            xRay ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                            xRay = !xRay;
                            break;
                        case SDLK_LEFT:
                            cameraMatrix = glm::translate(cameraMatrix, glm::vec3(-0.1f, 0.0f, 0.0f));
                            break;
                        case SDLK_RIGHT:
                            cameraMatrix = glm::translate(cameraMatrix, glm::vec3(0.1f, 0.0f, 0.0f));
                            break;
                        case SDLK_UP:
                            cameraMatrix = glm::translate(cameraMatrix, glm::vec3(0.f, 0.1f, 0.f));
                            break;
                        case SDLK_DOWN:
                            cameraMatrix = glm::translate(cameraMatrix, glm::vec3(0.f, -0.1f, 0.f));
                            break;
                        }
                    break;
                //We can add more event, like listening for the keyboard or the mouse. See SDL_Event documentation for more details
            }
        }

        //Clear the screen : the depth buffer and the color buffer
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // glm::mat4 cameraMatrix(1.0f); //Camera matrix. If you want a 3D projection matrix, look at
        // // ,→ glm::lookAt : glm::mat4 mat = glm::lookAt(EyePosition, Center, UpVector) where each
        // // ,→ parameters is typed glm::vec3 : glm::vec3 vec(x, y, z); (you can do directly in the
        // // ,→ parameters : glm::vec3(x, y, z) to create glm::vec3 on the fly)

        // glm::mat4 matrix(1.0f); //Defines an identity matrix
        // // 1 0 0 0
        // // 0 1 0 0
        // // 0 0 1 0
        // // 0 0 0 1

        //The most left transformation presented in equation (1) has to be done first
        // matrix = glm::translate(matrix, glm::vec3(transX, transY, transZ)); //We translate
        //  //We rotate via an
        // ,→ axis and an angle around this axis

        // matrix = glm::scale(matrix, glm::vec3(0.5f, 0.5f, 1.0f)); //And then we scale
        // matrix = glm::translate(matrix, glm::vec3(0.5f, 0, 0)); //We translate
        // matrix = glm::rotate(matrix, glm::radians(angleSquare), glm::vec3(0, 1, 0));
        // matrix = glm::rotate(matrix, glm::radians(angleSquare), glm::vec3(1, 0, 0));

        // angleSquare >= 360 ? angleSquare=0 : angleSquare++; 

        

        std::stack<glm::mat4> mvpStack;
        mvpStack.push(projectionMatrix * glm::inverse(cameraMatrix));

        draw(shader, mvpStack, body);
        
        //Display on screen (swap the buffer on screen and the buffer you are drawing on)
        SDL_GL_SwapWindow(window);

        //Time in ms telling us when this frame ended. Useful for keeping a fix framerate
        uint32_t timeEnd = SDL_GetTicks();

        //We want FRAMERATE FPS
        if(timeEnd - timeBegin < TIME_PER_FRAME_MS)
            SDL_Delay(TIME_PER_FRAME_MS - (timeEnd - timeBegin));
    }
    
    delete shader; //Delete the shader (usually at the end of the program)

    glDeleteBuffers(1, &head.vao); //Delete at the end the buffer

    //Free everything
    if(context != NULL)
        SDL_GL_DeleteContext(context);
    if(window != NULL)
        SDL_DestroyWindow(window);

    return 0;
}

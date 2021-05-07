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
    
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, 2 * geometry.getNbVertices() * sizeof(float)*3, NULL, GL_DYNAMIC_DRAW); 
    
    glBufferSubData(GL_ARRAY_BUFFER, 0,                  geometry.getNbVertices()*sizeof(float)*3, geometry.getVertices());
    glBufferSubData(GL_ARRAY_BUFFER, geometry.getNbVertices()*sizeof(float)*3,  geometry.getNbVertices()*sizeof(float)*3, geometry.getNormals());
    //glBufferSubData(GL_ARRAY_BUFFER, 3*3sizeof(float)*nbVertices, 2*sizeof(float)*nbVertices, uvData);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 3*sizeof(float), 0); 
    glEnableVertexAttribArray(0); //Enable "vPosition"
    
    glVertexAttribPointer(1, 3, GL_FLOAT, 0, 3*sizeof(float), INDICE_TO_PTR(geometry.getNbVertices()*3*sizeof(float))); //Convert an indice to void* : (void*)(x)
    glEnableVertexAttribArray(1); //Enable"vColor"

    glBindBuffer(GL_ARRAY_BUFFER, 0); //Close the buffer

    glBindVertexArray(0);
    return VAO;
}

void draw(Shader* shader, std::stack<glm::mat4>& mvpStack, GeometryObject object){

    glUseProgram(shader->getProgramID());
    glBindVertexArray(object.vao);
            GLint uMVP = glGetUniformLocation(shader->getProgramID(), "uMVP"); 
            glm::mat4 mvp = mvpStack.top() * object.propagatedMatrix * object.localMatrix;
            glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp)); 

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
    Sphere sphere(32,32);


    GeometryObject head;
    head.nbVertices = sphere.getNbVertices();
    head.vao = generateVAO(sphere);
    head.localMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.40f, 0.50f, 0.40f));
    head.propagatedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.8f, 0.0f));

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
      
    float moveAngle = 5;

    bool isOpened = true;
    bool xRay = false;

    glm::mat4 cameraMatrix(1.0f);
    cameraMatrix = glm::translate(cameraMatrix, glm::vec3(0.0f, 0.0f, 3.0f));
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);

    //Main application loop
    while(isOpened)
    {
        //Time in ms telling us when this frame started. Useful for keeping a fix framerate
        uint32_t timeBegin = SDL_GetTicks();

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
                        case SDLK_d:
                            body.propagatedMatrix = glm::rotate(body.propagatedMatrix, glm::radians(moveAngle), glm::vec3(0, 1, 0));
                            break;
                        case SDLK_q:
                            body.propagatedMatrix = glm::rotate(body.propagatedMatrix, glm::radians(-moveAngle), glm::vec3(0, 1, 0));
                            break;
                        case SDLK_z:
                            body.propagatedMatrix = glm::rotate(body.propagatedMatrix, glm::radians(-moveAngle), glm::vec3(1, 0, 0));
                            break;
                        case SDLK_s:
                            body.propagatedMatrix = glm::rotate(body.propagatedMatrix, glm::radians(moveAngle), glm::vec3(1, 0, 0));
                            break;
                        case SDLK_ESCAPE:
                            return 0;
                            break;
                        }
                    break;
            }
        }

        //Clear the screen : the depth buffer and the color buffer
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

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

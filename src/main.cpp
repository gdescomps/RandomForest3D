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
#include "Cylinder.h"

#include "GeometryObject.h"
#include "Tree.h"

#include "logger.h"

#define WIDTH     800
#define HEIGHT    600
#define FRAMERATE 60
#define TIME_PER_FRAME_MS  (1.0f/FRAMERATE * 1e3)
#define INDICE_TO_PTR(x) ((void*)(x))

void draw(Shader* shader, std::stack<glm::mat4>& mvpStack, GeometryObject object){

    glUseProgram(shader->getProgramID());
    glBindVertexArray(object.getVAO());
            GLint uMVP = glGetUniformLocation(shader->getProgramID(), "uMVP"); 
            glm::mat4 mvp = mvpStack.top() * object.getPropagatedMatrix() * object.getLocalMatrix();
            glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp)); 

            glDrawArrays(GL_TRIANGLES, 0, object.getNbVertices()); //Draw the triangle (three points which
            // ,→ starts at offset = 0 in the VBO). GL_TRIANGLES tells that we are reading
            // ,→ three points per three points to form a triangle. Other kind of "
            // ,→ reading" exist, see glDrawArrays for more details.
            // glBindBuffer(GL_ARRAY_BUFFER, 0); //Close the VBO (not mandatory but recommended,→ for not modifying it accidently).
            
            mvpStack.push(mvpStack.top() * object.getPropagatedMatrix());
            for(GeometryObject child : *object.getChildren())
                draw(shader, mvpStack, child);

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

    GeometryObject* tree1 = new Tree();


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
      
    bool isOpened = true;
    bool xRay = false;

    
    glm::mat4 view = glm::mat4(1.0f);

    int angle = 0;

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
                            view = glm::translate(view, glm::vec3(-0.1f, 0.0f, 0.0f));
                            break;
                        case SDLK_RIGHT:
                            view = glm::translate(view, glm::vec3(0.1f, 0.0f, 0.0f));
                            break;
                        case SDLK_UP:
                            view = glm::translate(view, glm::vec3(0.f, 0.0f, -0.1f));
                            break;
                        case SDLK_DOWN:
                            view = glm::translate(view, glm::vec3(0.f, 0.0f, 0.1f));
                            break;

                        // case SDLK_d:
                        //     body.propagatedMatrix = glm::rotate(body.propagatedMatrix, glm::radians(moveAngle), glm::vec3(0, 1, 0));
                        //     break;
                        // case SDLK_q:
                        //     body.propagatedMatrix = glm::rotate(body.propagatedMatrix, glm::radians(-moveAngle), glm::vec3(0, 1, 0));
                        //     break;
                        // case SDLK_z:
                        //     body.propagatedMatrix = glm::rotate(body.propagatedMatrix, glm::radians(-moveAngle), glm::vec3(1, 0, 0));
                        //     break;
                        // case SDLK_s:
                        //     body.propagatedMatrix = glm::rotate(body.propagatedMatrix, glm::radians(moveAngle), glm::vec3(1, 0, 0));
                        //     break;

                        case SDLK_ESCAPE:
                            return 0;
                            break;
                        }
                    break;
            }
        }

        //Clear the screen : the depth buffer and the color buffer
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


        // glm::mat4 view;
        // view = glm::lookAt(glm::vec3(0.0f, 0.0f, -3.0f), 
        //        glm::vec3(0.0f, 0.0f, 0.0f), 
        //        glm::vec3(0.0f, 1.0f, 0.0f));


        const float radius = 3.0f;
        const float speed = 0.01f;
        float camX = sin(angle*speed) * radius;
        float camZ = cos(angle*speed) * radius;
        
        view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);

        std::stack<glm::mat4> mvpStack;
        mvpStack.push(projectionMatrix * view);
        
        angle++;

        draw(shader, mvpStack, *tree1);
        

        //Display on screen (swap the buffer on screen and the buffer you are drawing on)
        SDL_GL_SwapWindow(window);

        //Time in ms telling us when this frame ended. Useful for keeping a fix framerate
        uint32_t timeEnd = SDL_GetTicks();

        //We want FRAMERATE FPS
        if(timeEnd - timeBegin < TIME_PER_FRAME_MS)
            SDL_Delay(TIME_PER_FRAME_MS - (timeEnd - timeBegin));
    }
    
    delete shader; //Delete the shader (usually at the end of the program)

    // glDeleteBuffers(1, &head.vao); //Delete at the end the buffer

    //Free everything
    if(context != NULL)
        SDL_GL_DeleteContext(context);
    if(window != NULL)
        SDL_DestroyWindow(window);

    return 0;
}

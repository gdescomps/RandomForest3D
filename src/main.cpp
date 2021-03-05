//SDL Libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

//OpenGL Libraries
#include <GL/glew.h>
#include <GL/gl.h>

//GML libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include "logger.h"

#define WIDTH     800
#define HEIGHT    600
#define FRAMERATE 60
#define TIME_PER_FRAME_MS  (1.0f/FRAMERATE * 1e3)
#define INDICE_TO_PTR(x) ((void*)(x))

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

    //TODO
    //From here you can load your OpenGL objects, like VBO, Shaders, etc.

    float vPosition[]={-1.0,-1.0,0.0,
                        1.0,-1.0,0.0,
                        0.0,1.0,0.0};

    float vColor[]={1.0,0.0,0.0,
                    0.0,1.0,0.0,
                    0.0,0.0,1.0};



    //We generate our buffer
    GLuint myBuffer;
    glGenBuffers(1, &myBuffer);

    //We fill this buffer as a GL_ARRAY_BUFFER (buffer containing vertices (points) information). 
    //Remind to close this buffer for not misusing it(glBindBuffer(GL_ARRAY_BUFFER, 0);)
    glBindBuffer(GL_ARRAY_BUFFER, myBuffer);
    //2 coordinates per UV, 3 per normal and 3 per position. We do not yet copy these data (hence the NULL)
    glBufferData(GL_ARRAY_BUFFER, 2 * 3 * sizeof(float)*3, NULL, GL_DYNAMIC_DRAW); 

    //Copy one by one the data (first positions, then normals and finally UV).
    //We remind that we do not necessarily need all of these variables, and that other variables may be needed for your usecase
    //parameters : Target, buffer offset, size to copy, CPU data.
    
    //We consider that each data are typed « float* » with sizeof(float)*nbVertices*nbCoordinate bytes where nbCoordinate = 2 or 3 following the number of components per value for this variable
    glBufferSubData(GL_ARRAY_BUFFER, 0,                  3*sizeof(float)*3, vPosition);
    glBufferSubData(GL_ARRAY_BUFFER, 3*sizeof(float)*3,  3*sizeof(float)*3, vColor);
    // glBufferSubData(GL_ARRAY_BUFFER, 3*3sizeof(float)*nbVertices, 2*sizeof(float)*nbVertices, uvData);
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Close the buffer

    //TODO do your thing

    FILE* vertFile = fopen("Shaders/color.vert", "r");
    FILE* fragFile = fopen("Shaders/color.frag", "r");
    //TODO test if the files are correct (testing NULL)
    Shader* shader = Shader::loadFromFiles(vertFile, fragFile); //Load the files and, create the GPU program
    fclose(vertFile);
    fclose(fragFile);
    if(shader == NULL)
    {
    //Print an error message (an error occured while compiling). The message is also,displayed on screen by loadFromFiles function
    return EXIT_FAILURE;
    }
    //....
    //TODO do something with your shader
   
    // float uScale;

    
    //TODO

    bool isOpened = true;

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
                //We can add more event, like listening for the keyboard or the mouse. See SDL_Event documentation for more details
            }
        }

        //Clear the screen : the depth buffer and the color buffer
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);



        glUseProgram(shader->getProgramID());
        { //The brackets are useless but help at the clarity of the code
            glBindBuffer(GL_ARRAY_BUFFER, myBuffer);

            GLint uScale = glGetUniformLocation(shader->getProgramID(), "uScale"); //Get the "uScale" location (ID)
            glUniform1f(uScale, 0.5f); //Set "uScale" at 0.5f. Remark : we use glUniform1f for sending a
            // ,→ (1) float (f). For other kinds of uniform (integers, vectors, etc.) see the
            // ,→ documentation


            //Work with vPosition
            GLint vPosition = glGetAttribLocation(shader->getProgramID(), "vPosition");
            glVertexAttribPointer(vPosition, 3, GL_FLOAT, 0, 3*sizeof(float), 0); //It is here that you
            // ,→ select how the Shader reads the VBO. Indeed the 5th parameter is called
            // ,→ "stride" : it is the distance in bytes between two values for the same
            // ,→ kind or variable. If the values are side-by-side, stride == 0. Here we
            // ,→ need to set to 3*sizeof(float) for the first version of the VBO seen in
            // ,→ class.
            glEnableVertexAttribArray(vPosition); //Enable "vPosition"
            //Work with vColor
            GLint vColor = glGetAttribLocation(shader->getProgramID(), "vColor");
            //Colors start at 9*sizeof(float) (3*nbVertices*sizeof(float)) for the second
            // ,→ version of the VBO. For the first version of the VBO, both the stride
            // ,→ and the offset should be 3*sizeof(float) here
            glVertexAttribPointer(vColor, 3, GL_FLOAT, 0, 3*sizeof(float), INDICE_TO_PTR(9*sizeof(float))); //Convert an indice to void* : (void*)(x)
            glEnableVertexAttribArray(vColor); //Enable"vColor"
            glDrawArrays(GL_TRIANGLES, 0, 3); //Draw the triangle (three points which
            // ,→ starts at offset = 0 in the VBO). GL_TRIANGLES tells that we are reading
            // ,→ three points per three points to form a triangle. Other kind of "
            // ,→ reading" exist, see glDrawArrays for more details.
            glBindBuffer(GL_ARRAY_BUFFER, 0); //Close the VBO (not mandatory but recommended,→ for not modifying it accidently).
        }
        glUseProgram(0); //Close the program. This is heavy for the GPU. In reality we do this
        // ,→ only if we have to CHANGE the shader (hence we cache the current in-use shader)
        // ,→ . For this course however the performances are not so important.
        //....


        //TODO rendering
        

        
        

        //Display on screen (swap the buffer on screen and the buffer you are drawing on)
        SDL_GL_SwapWindow(window);

        //Time in ms telling us when this frame ended. Useful for keeping a fix framerate
        uint32_t timeEnd = SDL_GetTicks();

        //We want FRAMERATE FPS
        if(timeEnd - timeBegin < TIME_PER_FRAME_MS)
            SDL_Delay(TIME_PER_FRAME_MS - (timeEnd - timeBegin));
    }
    
    delete shader; //Delete the shader (usually at the end of the program)

    glDeleteBuffers(1, &myBuffer); //Delete at the end the buffer

    //Free everything
    if(context != NULL)
        SDL_GL_DeleteContext(context);
    if(window != NULL)
        SDL_DestroyWindow(window);

    return 0;
}

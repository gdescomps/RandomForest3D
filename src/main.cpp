//SDL Libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

//OpenGL Libraries
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL_image.h>

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
#include "Circle.h"

#include "GeometryObject.h"
#include "Tree.h"

#include "logger.h"

#define WIDTH     800
#define HEIGHT    600
#define FRAMERATE 60
#define TIME_PER_FRAME_MS  (1.0f/FRAMERATE * 1e3)
#define INDICE_TO_PTR(x) ((void*)(x))

    struct Material{
        float ka = 0.20;
        float kd = 0.50;
        float ks = 0.40;
        float alpha = 20;
        glm::vec3 Color = glm::vec3(1.0, 0.0, 0.0);
    };


    struct Light {
        glm::vec3 lightPosition = glm::vec3(10.0, 10.0, 20.0);
        glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);
    };


void draw(glm::mat4 modelMatrix, glm::mat4 inv_modelMatrix, glm::vec3 camera, Shader* shader, std::stack<glm::mat4>& mvpStack, GeometryObject object, GLuint &texture1, GLuint &texture2, GLuint &texture3, Material& material, Light& light){

    // object.getTextureId()
    // = 0 -> écorce
    // = 1 -> feuilles

    glUseProgram(shader->getProgramID());
    glBindVertexArray(object.getVAO());

        GLint color = glGetUniformLocation(shader->getProgramID(), "color");
        glUniform3fv(color, 1, glm::value_ptr(material.Color));

        GLint ka = glGetUniformLocation(shader->getProgramID(), "ka");
        glUniform1f(ka, material.ka);

        GLint kd = glGetUniformLocation(shader->getProgramID(), "kd");
        glUniform1f(kd, material.kd);

        GLint ks = glGetUniformLocation(shader->getProgramID(), "ks");
        glUniform1f(ks, material.ks);

        GLint alpha = glGetUniformLocation(shader->getProgramID(), "alpha");
        glUniform1f(alpha, material.alpha);

        GLint lightcolor = glGetUniformLocation(shader->getProgramID(), "lightcolor");
        glUniform3fv(lightcolor, 1, glm::value_ptr(light.lightColor));

        GLint lightposition = glGetUniformLocation(shader->getProgramID(), "lightposition");
        glUniform3fv(lightposition, 1, glm::value_ptr(light.lightPosition));

        GLint modelmatrix = glGetUniformLocation(shader->getProgramID(), "modelmatrix");
        glUniformMatrix4fv(modelmatrix, 1, GL_FALSE, glm::value_ptr(object.getLocalMatrix()));

        GLint inv_modelmatrix = glGetUniformLocation(shader->getProgramID(), "inv_modelmatrix");
        glUniformMatrix3fv(inv_modelmatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(object.getLocalMatrix())));

        GLint cameraposition = glGetUniformLocation(shader->getProgramID(), "cameraposition");
        glUniform3fv(cameraposition, 1, glm::value_ptr(camera));

            GLint uMVP = glGetUniformLocation(shader->getProgramID(), "uMVP"); 
            glm::mat4 mvp = mvpStack.top() * object.getPropagatedMatrix() * object.getLocalMatrix();
            glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp)); 


            if (object.getTextureId() == 1){
                // active texture  1 :
                // glUniform1i(texture1,0);
                glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture1"),0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture1);

            }
            else if (object.getTextureId() == 0){
            // active texture  2 :
                // glUniform1i(texture2,0);
                glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture2"),0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture2);
            }
            else if (object.getTextureId() == 2){
            // active texture  3 :
                // glUniform1i(texture2,0);
                glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture3"),0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture3);
            }
        
        

            glDrawArrays(GL_TRIANGLES, 0, object.getNbVertices()); //Draw the triangle (three points which
            // ,→ starts at offset = 0 in the VBO). GL_TRIANGLES tells that we are reading
            // ,→ three points per three points to form a triangle. Other kind of "
            // ,→ reading" exist, see glDrawArrays for more details.
            // glBindBuffer(GL_ARRAY_BUFFER, 0); //Close the VBO (not mandatory but recommended,→ for not modifying it accidently).
            
            mvpStack.push(mvpStack.top() * object.getPropagatedMatrix());
            for(GeometryObject child : *object.getChildren())
                draw(modelMatrix, inv_modelMatrix, camera, shader, mvpStack, child, texture1, texture2, texture3, material, light);

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

    SDL_SetRelativeMouseMode(SDL_TRUE);

    //Tells GLEW to initialize the OpenGL function with this version
    glewExperimental = GL_TRUE;
    glewInit();


    //Start using OpenGL to draw something on screen
    glViewport(0, 0, WIDTH, HEIGHT); //Draw on ALL the screen

    //The OpenGL background color (RGBA, each component between 0.0f and 1.0f)
    glClearColor(0.5, 0.8, 0.73, 1.0); //Blue sky
    // glClearColor(0.2, 0.314, 0.361, 1.0); //Full Black

    glEnable(GL_DEPTH_TEST); //Active the depth test


    //From here you can load your OpenGL objects, like VBO, Shaders, etc.

    Circle circle(32);
    GeometryObject floor(circle);
    floor.transform(local, rotate, glm::vec3(1, 0, 0), 90.0f);
    floor.transform(local, scale, glm::vec3(100, 100, 0));
    floor.setTextureId(2);

     std::vector<GeometryObject*> forest;

    for (int z = 0; z < 5; ++z)
    {
        for (int x = 0; x < 5; ++x)
        {
            GeometryObject* tree1 = new Tree();
            tree1->transform(relative, translate, glm::vec3(x*4.0f,0.f,z*4.f));
            forest.push_back(tree1);
        }
    }
    
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

    const float cameraSpeed = 0.1f;
    const float horizontalMouseSpeed=0.1f;
    const float verticalMouseSpeed=0.1f;
    
    float yaw=270;
    float pitch=0;

    glm::mat4 view = glm::mat4(1.0f);

    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

    Material material;
    Light light;





    // texture 1:

    SDL_Surface* img = IMG_Load("textures/texturefeuille.jpg");
    SDL_Surface* rgbImg = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_RGBA32, 0);

    GLuint texture1;

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    {

        // glActiveTexture(GL_TEXTURE0);
        // glActiveTexture(GL_TEXTURE1);


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, rgbImg->w, rgbImg->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)rgbImg->pixels);

        glGenerateMipmap(GL_TEXTURE_2D);



    }
    SDL_FreeSurface(img);
    

        //texture 2 :

    SDL_Surface* img2 = IMG_Load("textures/textureTronc3.jpg");
    SDL_Surface* rgbImg2 = SDL_ConvertSurfaceFormat(img2, SDL_PIXELFORMAT_RGBA32, 0);


    GLuint texture2;

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    {


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, rgbImg2->w, rgbImg2->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)rgbImg2->pixels);

        glGenerateMipmap(GL_TEXTURE_2D);



    }
    SDL_FreeSurface(img2);



    //texture 3 :

    SDL_Surface* img3 = IMG_Load("textures/textureTronc2.jpg");
    SDL_Surface* rgbImg3 = SDL_ConvertSurfaceFormat(img3, SDL_PIXELFORMAT_RGBA32, 0);


    GLuint texture3;

    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    {


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, rgbImg3->w, rgbImg3->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)rgbImg3->pixels);

        glGenerateMipmap(GL_TEXTURE_2D);



    }
    SDL_FreeSurface(img3);





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

                case SDL_MOUSEMOTION:
                    yaw+=event.motion.xrel*horizontalMouseSpeed;
                    pitch-=event.motion.yrel*verticalMouseSpeed;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_x:
                            xRay ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                            xRay = !xRay;
                            break;
                        case SDLK_q:
                            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
                            break;
                        case SDLK_d:
                            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
                            break;
                        case SDLK_z:
                            cameraPos += cameraSpeed * cameraFront;
                            break;
                        case SDLK_s:
                            cameraPos -= cameraSpeed * cameraFront;
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

        if(pitch > 89.0f)
          pitch =  89.0f;
        if(pitch < -89.0f)
          pitch = -89.0f;
        
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);

        cameraPos.y=1.80f;

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);

        std::stack<glm::mat4> mvpStack;
        mvpStack.push(projectionMatrix * view);

        glm::mat4 modelMatrix(1.0f);
        glm::mat4 viewMatrix(1.0f);
        glm::mat4 projectionMatrix1(1.0f);

        // glm::mat4 mvp = projectionMatrix1 * glm::inverse(viewMatrix) * modelMatrix;

        glm::mat3 inv_modelMatrix = glm::inverse(glm::mat3(modelMatrix));

        glm::vec4 tmp = glm::inverse(projectionMatrix1 * glm::inverse(viewMatrix)) * glm::vec4(0, 0, -1, 1);
        glm::vec3 camera = glm::vec3(tmp) / tmp.w;

        draw(modelMatrix, inv_modelMatrix, camera, shader, mvpStack, floor, texture1, texture2, texture3, material, light);
        for(GeometryObject* tree : forest){
            draw(modelMatrix, inv_modelMatrix, camera, shader, mvpStack, *tree, texture1, texture2, texture3, material, light);
        }

        

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

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>


void render_a_obj(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer, GLuint elementbuffer, GLsizei indices_size){
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0,                  // attribute
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
            1,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
    );

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
            2,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
    );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

    // Draw the triangles !
    glDrawElements(
            GL_TRIANGLES,      // mode
            (GLsizei)indices_size,    // count
            GL_UNSIGNED_SHORT, // type
            (void*)0           // element array buffer offset
    );

}


int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Road", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE); // Not this time !

    glEnable(GL_CULL_FACE);








    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint BikeVertexArrayID;
    glGenVertexArrays(1, &BikeVertexArrayID);
    glBindVertexArray(BikeVertexArrayID);



    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "../VertexShader.vs", "../FragmentShader.fs" );

    // Get a handle for our "MVP" uniform
    GLint MatrixID = glGetUniformLocation(programID, "MVP");
    GLint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLint ModelMatrixID = glGetUniformLocation(programID, "M");

    // Load the texture
    GLuint Texture = loadDDS("../uvmap.DDS");

    // Get a handle for our "myTextureSampler" uniform
    GLint TextureID = glGetUniformLocation(programID, "myTextureSampler");




    // Read our .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadOBJ("../plane1.obj", vertices, uvs, normals);


    std::vector<glm::vec3> bike_vertices;
    std::vector<glm::vec2> bike_uvs;
    std::vector<glm::vec3> bikes_normals;
    bool bike_res = loadOBJ("../bike.obj", bike_vertices, bike_uvs, bikes_normals);

    std::vector<unsigned short> bike_indices;
    std::vector<glm::vec3> indexed_bike_vertices;
    std::vector<glm::vec2> indexed_bike_uvs;
    std::vector<glm::vec3> indexed_bike_normals;
    indexVBO(bike_vertices, bike_uvs, bikes_normals, bike_indices, indexed_bike_vertices, indexed_bike_uvs, indexed_bike_normals);



    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
    // Load it into a VBO

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

    // Generate a buffer for the indices as well
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);


    GLuint bike_vertexbuffer;
    glGenBuffers(1, &bike_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bike_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_bike_vertices.size() * sizeof(glm::vec3), &indexed_bike_vertices[0], GL_STATIC_DRAW);

    GLuint bike_uvbuffer;
    glGenBuffers(1, &bike_uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bike_uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_bike_uvs.size() * sizeof(glm::vec2), &indexed_bike_uvs[0], GL_STATIC_DRAW);

    GLuint bike_normalbuffer;
    glGenBuffers(1, &bike_normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bike_normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_bike_normals.size() * sizeof(glm::vec3), &indexed_bike_normals[0], GL_STATIC_DRAW);

    // Generate a buffer for the indices as well
    GLuint bike_elementbuffer;
    glGenBuffers(1, &bike_elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bike_elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bike_indices.size() * sizeof(unsigned short), &bike_indices[0], GL_STATIC_DRAW);







    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 BikeScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));


//    glm::vec3 SceneMyRotateAxis(0,1,0);
//    glm::mat4 SceneRotationMatrix = glm::rotate(glm::mat4(1.0f), 3.14f, SceneMyRotateAxis);
    glm::mat4 SceneRotationMatrix = glm::mat4(1.0f);
    glm::mat4 SceneTransformMatrix = glm::translate(SceneRotationMatrix, glm::vec3(0,-3,0));

//    glm::vec3 BikeRotateAxis(0,1,0);
//    glm::mat4 BikeRotateMatrix = glm::rotate(glm::mat4(1.0f), 3.14f, BikeRotateAxis);
    glm::mat4 BikeRotateMatrix = glm::mat4(1.0f);
    glm::mat4 BikeTransformMatrix = glm::translate(BikeRotateMatrix, glm::vec3(0,-2,0));

    do{

        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
            // printf and reset
            printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);


        glm::mat4 BikeModelMatrix = BikeTransformMatrix * BikeScaleMatrix;

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = SceneTransformMatrix;
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        glm::mat4 bike_MVP = ProjectionMatrix * ViewMatrix * BikeModelMatrix;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        glm::vec3 lightPos = glm::vec3(5, 20, 10);
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(TextureID, 0);
//
//        // 1rst attribute buffer : vertices
//        glEnableVertexAttribArray(0);
//        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//        glVertexAttribPointer(
//                0,                  // attribute
//                3,                  // size
//                GL_FLOAT,           // type
//                GL_FALSE,           // normalized?
//                0,                  // stride
//                (void*)0            // array buffer offset
//        );
//
//        // 2nd attribute buffer : UVs
//        glEnableVertexAttribArray(1);
//        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
//        glVertexAttribPointer(
//                1,                                // attribute
//                2,                                // size
//                GL_FLOAT,                         // type
//                GL_FALSE,                         // normalized?
//                0,                                // stride
//                (void*)0                          // array buffer offset
//        );
//
//        // 3rd attribute buffer : normals
//        glEnableVertexAttribArray(2);
//        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
//        glVertexAttribPointer(
//                2,                                // attribute
//                3,                                // size
//                GL_FLOAT,                         // type
//                GL_FALSE,                         // normalized?
//                0,                                // stride
//                (void*)0                          // array buffer offset
//        );
//
//        // Index buffer
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
//
//        // Draw the triangles !
//        glDrawElements(
//                GL_TRIANGLES,      // mode
//                (GLsizei)indices.size(),    // count
//                GL_UNSIGNED_SHORT, // type
//                (void*)0           // element array buffer offset
//        );


        render_a_obj(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, (GLsizei)indices.size());

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &bike_MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &BikeModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        render_a_obj(bike_vertexbuffer, bike_uvbuffer, bike_normalbuffer, bike_elementbuffer, (GLsizei)bike_indices.size());


        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);




        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}


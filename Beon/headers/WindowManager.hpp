#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

//Defaults
const int mWidth = 800;
const int mHeight = 600;
const std::string programName = "Beon Engine";

class WindowManager{
	public:
		static WindowManager* getInstance();
		GLFWwindow* getWindow();
		int initWindow(std::string name = programName, int width = mWidth, int height = mHeight);
		void setWindowDimensions(int width, int height);


		std::string mProgramName = programName;
		int width = mWidth;
		int height = mHeight;

	private:
		static WindowManager* instance;
		GLFWwindow* window;
		WindowManager();

};

WindowManager* WindowManager::instance = 0;

WindowManager* WindowManager::getInstance()
{
	if(instance == 0){
		instance = new WindowManager();
	}

	return instance;
}

GLFWwindow* WindowManager::getWindow(){
	if(window == NULL){
		std::cout << "[x] Window not initialized. Call function 'initWindow(std::string name, int width, int height) with defaults. '\n" << std::endl;
		WindowManager::initWindow();
	}
	return window;
}

int WindowManager::initWindow(std::string name, int width, int height){
	this->mProgramName = name;
	this->width = width;
	this->height = height;

	window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	if (window == NULL)
    {
        std::cout << "[x] Failed to create GLFW window\n" << std::endl;
        glfwTerminate();
    }
    //Set current window context
    glfwMakeContextCurrent(window);


    //Load OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "[x] Failed to initialize GLAD\n" << std::endl;
        return -1;
    }
    //Load glad
    gladLoadGL();
    fprintf(stderr, "[-] OpenGL %s\n", glGetString(GL_VERSION));

    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, mWidth/2, mHeight/2);


    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS); 

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    //Enable SRGB
    glEnable(GL_FRAMEBUFFER_SRGB); 
    return 0;
}


WindowManager::WindowManager(){
	// Initialise GLFW and configure
    if( !glfwInit() )
    {
        fprintf( stderr, "[x] Failed to initialize GLFW\n" );
        getchar();
        glfwTerminate();
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
#endif
}




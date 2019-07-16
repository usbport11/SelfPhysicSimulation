#include "stdafx.h"
#include "classes/image/TextureLoader.h"
#include "classes/system/Shader.h"
#include "classes/system/Scene.h"
#include "classes/buffer/ObjectBuffer.h"
#include "classes/simpleobject/PhysicWorld2.h"
#include "structures/FPSController.h"
#include "classes/simpleobject/Enemy2.h"

int WindowWidth = 800, WindowHeight = 600;
GLFWwindow* window;
MShader Shader;
MScene Scene;
MTextureLoader TextureLoader;
MPhysicWorld2 PhysicWorld2;
MPhysicObject2 MainObject;
MPhysicObject2 GroundBlock[6];
MObjectBuffer ObjectBuffer;
stTexture* txGround;
stTexture* txMainObject;
stTexture* txBullet;
stTexture* txEnemy;
unsigned int txOne_cnt;
glm::vec4 whiteColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
bool keys[1024] = {0};
stFPSController FPSController;

float ObjectRotation;
glm::vec2 ObjectStrafe;
glm::vec2 ObjectVelocity;
glm::vec2 ObjectCenter;
glm::vec2 ObjectDirection;

MObjectBuffer BulletsBuffer;
vector<MPhysicObject2*> Bullets;
glm::vec2 MouseSceneCoord;

MEnemy2 Enemy2;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}
    
    if(action == GLFW_PRESS)
    	keys[key] = true;
    else if (action == GLFW_RELEASE)
    	keys[key] = false;
}

static void mousepos_callback(GLFWwindow* window, double x, double y)
{
	MouseSceneCoord = Scene.WindowPosToWorldPos(x, y);
	ObjectDirection = glm::normalize(MouseSceneCoord - ObjectCenter);
	ObjectRotation = atan2(ObjectDirection.y, ObjectDirection.x);
	ObjectStrafe = (glm::vec2)glm::cross(glm::vec3(ObjectDirection.x, ObjectDirection.y, 0), glm::vec3(0, 0, 1)) * 0.02f;
}

static void mouseclick_callback(GLFWwindow* window, int button, int action, int mods)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS /*&& Bullets.size() < 3*/)
	{
		//create
		Bullets.push_back(new MPhysicObject2);
		BulletsBuffer.AddObject(Bullets.back(), ObjectCenter.x, ObjectCenter.y, 0.1, 0.1, 0,0,1,1, txBullet->Id);
		BulletsBuffer.DisposeAll();
		Bullets.back()->SetIsBullet(true);
		PhysicWorld2.AddDynamicObject(Bullets.back());
		//start move
		Bullets.back()->SetVelocity(ObjectDirection * 0.08f);
	}
}

void CheckBullets()
{
	int i = 0;
	while(i < Bullets.size() && Bullets.size() > 0)
	{
		if(Bullets[i]->GetCollisionsCount())
		{
			BulletsBuffer.RemoveObject(Bullets[i]);
			PhysicWorld2.RemoveDynamicObjectForce(Bullets[i]);
			delete Bullets[i];
			Bullets.erase(Bullets.begin() + i);
		}
		else i++;
	}
}

bool CheckOpenglSupport()
{
	//get opengl data (here was fall on suspisious notebook)
	string OpenGLVersion = (char*)glGetString(GL_VERSION);
	string OpenGLVendor = (char*)glGetString(GL_VENDOR);
	string OpenGLRenderer = (char*)glGetString(GL_RENDERER);
	string ShadersVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	LogFile<<"Window: OpenGL version: "<<OpenGLVersion<<endl;
	LogFile<<"Window: OpenGL vendor: "<<OpenGLVendor<<endl;
	LogFile<<"Window: OpenGL renderer: "<<OpenGLRenderer<<endl;
	LogFile<<"Window: Shaders version: "<<ShadersVersion<<endl;
	
	float VersionOGL, VersionSHD;
	sscanf(OpenGLVersion.c_str(), "%f", &VersionOGL);
	if(VersionOGL < 3.0f)
	{
		LogFile<<"Window: Old version of OpenGL. Sorry"<<endl;
		return false;
	}
	sscanf(ShadersVersion.c_str(), "%f", &VersionSHD);
	if(VersionSHD < 3.3f)
	{
		LogFile<<"Window: Old version of shaders. Sorry"<<endl;
		return false;
	}
}

void ControlsActions()
{
	ObjectVelocity = glm::vec2(0, 0);
	if(keys['A']) ObjectVelocity.x = -0.02;
	if(keys['D']) ObjectVelocity.x = 0.02;
	if(keys['W']) ObjectVelocity.y = 0.02;
	if(keys['S']) ObjectVelocity.y = -0.02;
	MainObject.SetVelocity(ObjectVelocity);
}

void RenderStep()
{
	//world
	CheckBullets();
	PhysicWorld2.Step();
	ObjectCenter = GetQuadCenter(MainObject.GetCollisionBox());
	MainObject.SetVertex(QuadRotate(MainObject.GetCollisionBox(), ObjectRotation));
	Enemy2.Follow();
	ObjectBuffer.UpdateAll();
	BulletsBuffer.UpdateAll();
	
	//clear
    glClear(GL_COLOR_BUFFER_BIT);
    
    //draw
    glUseProgram(Shader.ProgramId);
	glUniform1i(Shader.MainTextureId, 0);
	glUniform1i(Shader.LightTextureId, 1);
	glUniform2f(Shader.ResolutionId, WindowWidth, WindowHeight);
	glUniform4fv(Shader.AmbientColorId, 1, &whiteColor[0]);
	glUniformMatrix4fv(Shader.MVPId, 1, GL_FALSE, Scene.GetDynamicMVP());
	ObjectBuffer.Begin();
		ObjectBuffer.DrawAll();
		BulletsBuffer.DrawAll();
	ObjectBuffer.End();
}

void ClearApp()
{
	ObjectBuffer.Close();
	BulletsBuffer.Close();
	Bullets.clear();
	PhysicWorld2.Close();
	TextureLoader.DeleteTexture(txGround, txOne_cnt);
	TextureLoader.DeleteTexture(txMainObject, txOne_cnt);
	TextureLoader.DeleteTexture(txBullet, txOne_cnt);
	TextureLoader.DeleteTexture(txEnemy, txOne_cnt);
	TextureLoader.Close();
	Shader.Close();
}

bool InitApp()
{
	LogFile<<"Starting application"<<endl;    
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) return -1;
    window = glfwCreateWindow(WindowWidth, WindowHeight, "TestApp", NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback (window, mouseclick_callback);
	glfwSetCursorPosCallback(window, mousepos_callback);
    glfwMakeContextCurrent(window);
    if(glfwExtensionSupported("WGL_EXT_swap_control")) glfwSwapInterval(1);//0 - disable, 1 - enable
    
    LogFile<<"Window created: width: "<<WindowWidth<<" height: "<<WindowHeight<<endl;

	//glew
	GLenum Error = glewInit();
	if(GLEW_OK != Error)
	{
		LogFile<<"Window: GLEW Loader error: "<<glewGetErrorString(Error)<<endl;
		return 0;
	}
	LogFile<<"GLEW initialized"<<endl;
	
	if(!CheckOpenglSupport()) return false;

	//shaders
	if(!Shader.CreateShaderProgram("shaders/main2.vertexshader.glsl", "shaders/main2.fragmentshader.glsl")) return false;
	if(!Shader.PrepareShaderValues()) return false;
	LogFile<<"Shaders loaded"<<endl;

	//randomize
    srand(time(NULL));
    LogFile<<"Randomized"<<endl;

	//load textures
	if(!(txMainObject = TextureLoader.LoadTexture("textures/tex09.png", 1, 1, 0, txOne_cnt, GL_NEAREST, GL_REPEAT, false))) return false;
	if(!(txGround = TextureLoader.LoadTexture("textures/tex07.png", 1, 1, 0, txOne_cnt, GL_NEAREST, GL_REPEAT, false))) return false;
	if(!(txBullet = TextureLoader.LoadTexture("textures/tex04.png", 1, 1, 0, txOne_cnt, GL_NEAREST, GL_REPEAT, false))) return false;
	if(!(txEnemy = TextureLoader.LoadTexture("textures/tex10.png", 1, 1, 0, txOne_cnt, GL_NEAREST, GL_REPEAT, false))) return false;
	LogFile<<"Textures loaded"<<endl;
	
	//scene
	if(!Scene.Initialize(&WindowWidth, &WindowHeight)) return false;
	LogFile<<"Scene initialized"<<endl;
	
	//object buffer and movable object
	if(!BulletsBuffer.Initialize(GL_STREAM_DRAW)) return false;
	if(!BulletsBuffer.DisposeAll()) return false;
	
	if(!ObjectBuffer.Initialize(GL_STREAM_DRAW)) return false;
	//4 walls
	if(!ObjectBuffer.AddObject(&GroundBlock[0], 0, 0, 8, 0.32, 0,0,1,1, txGround->Id)) return false;
	if(!ObjectBuffer.AddObject(&GroundBlock[1], 0, 5.68, 8, 0.32, 0,0,1,1, txGround->Id)) return false;
	if(!ObjectBuffer.AddObject(&GroundBlock[2], 0, 0.32, 0.32, 5.36, 0,0,1,1, txGround->Id)) return false;
	if(!ObjectBuffer.AddObject(&GroundBlock[3], 7.68, 0.32, 0.32, 5.36, 0,0,1,1, txGround->Id)) return false;
	//planes
	if(!ObjectBuffer.AddObject(&GroundBlock[4], 1.28, 1.28, 0.64, 0.64, 0,0,1,1, txGround->Id)) return false;
	if(!ObjectBuffer.AddObject(&GroundBlock[5], 2.56, 2.56, 0.64, 0.64, 0,0,1,1, txGround->Id)) return false;
	//main object & Enemy
	if(!ObjectBuffer.AddObject(&MainObject, 1.28, 2.56, 0.64, 0.64, 0,0,1,1, txMainObject->Id)) return false;
	if(!ObjectBuffer.AddObject(&Enemy2, 4.84, 4.84, 0.64, 0.64, 0,0,1,1, txEnemy->Id)) return false;
	if(!ObjectBuffer.DisposeAll()) return false;
	LogFile<<"Buffers initialized"<<endl;
	
	PhysicWorld2.SetGravity(glm::vec2(0, 0));
	PhysicWorld2.AddStaticObject(&GroundBlock[0]);
	PhysicWorld2.AddStaticObject(&GroundBlock[1]);
	PhysicWorld2.AddStaticObject(&GroundBlock[2]);
	PhysicWorld2.AddStaticObject(&GroundBlock[3]);
	PhysicWorld2.AddStaticObject(&GroundBlock[4]);
	PhysicWorld2.AddStaticObject(&GroundBlock[5]);
	PhysicWorld2.AddDynamicObject(&MainObject);
	PhysicWorld2.AddDynamicObject(&Enemy2);
	
	Enemy2.SetTarget(&MainObject);
		
	return true;
}

int main(void)
{
	if(!InitApp())
	{
		ClearApp();
		glfwTerminate();
		return 0;
	}

	//main cycle
	LogFile<<"Main cycle start"<<endl;
	
	FPSController.Initialize(glfwGetTime());
    while(!glfwWindowShouldClose(window))
    {
    	//calculate frame time
    	FPSController.FrameStep(glfwGetTime());
    	FPSController.FrameCheck();
    	
    	ControlsActions();
    	RenderStep();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    LogFile<<"Closing application. Free memory"<<endl;
    
    ClearApp();
    glfwTerminate();
    
    return 0;
}

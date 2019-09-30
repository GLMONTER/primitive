//
// Created by monter on 6/2/19.
//


#include "Core.hpp"

//camera variables
glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
static float pitch, yaw;

//used to scale different speeds to higher or lower framerates
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float camSpeed = 10.0f;
//window width and height.
static unsigned int width, height;
//represents the current model selected for indexing.
static int currentItem;

//these are used together to determine whether the camera can move with the mouse in editor mode
static int mouseState;
static bool isMouse = true;

void GLFW_MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	static double lastX = static_cast<float>(width), lastY = static_cast<float>(height);
			
	if (isMouse)
	{
		lastX = xpos;
		lastY = ypos;
		isMouse = false;
	}
	double xoffset = xpos - lastX;
	double yoffset = lastY -  ypos;

	lastX = xpos;
	lastY = ypos;

	float sens = 0.05f;

	xoffset *= sens;
	yoffset *= sens;

	yaw += xoffset;
	pitch += yoffset;
	
	if(pitch > 89.0f)
		pitch = 89.0f;
	if(pitch < -89.0f)
		pitch = -89.0f;
	
	if (mouseState == GLFW_PRESS)
	{
		camFront.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		camFront.y = sin(glm::radians(pitch));
		camFront.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	}
}

void Core::updateCamera(unsigned int wid, unsigned int hei)
{
    //update camera values
    mainCamera.widthH = wid;
    mainCamera.heightH = hei;
		
    //check if they are not zero so glm doesn't assert
    if(mainCamera.widthH == 0 || mainCamera.heightH == 0)
        return;
    //recalculate the projection matrix.
    mainCamera.calc();
}

//a callback for windows resizing, also updates the OpenGL viewport
void Core::glfwFramebufferSizeCallback(GLFWwindow* wind, int w, int h)
{
    glViewport(0, 0, w, h);
	
    width = static_cast<unsigned int>(w);
    height = static_cast<unsigned int>(h);
}

void Core::Init()
{
    //init the sound system
    SoundSystem::init();

    //create the window and set the camera width and height to have the correct camera matrix without resizing.
    window.createWindow(false);
	
    //change camera parameters for matrix calculations
    mainCamera.widthH = window.width;
    mainCamera.heightH = window.height;
	
    //recalculate the camera matrix
    mainCamera.calc();
	
    //init GLEW
    if(glewInit() != GLEW_OK)
        throw std::runtime_error("GLEW FAILED TO INIT.");

    //set the window resize callback to change things like the camera matrix
    glfwSetWindowSizeCallback(window.window, Core::glfwFramebufferSizeCallback);
	glfwSetCursorPosCallback(window.window, GLFW_MouseCallback);

    //init the input system.
    input.init(window.window);

    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    //setup imgui Context
    ImGui::CreateContext();

    //setup imgui io and enable docking, not needed right now.
    
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //enable window docking
    io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
     

    //Setup imgui theme or style
	ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window.window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    //enable vsync by default
    glfwSwapInterval(0);

    //make the default shaders for basic rendering.
	defaultVert.createShader(Shader::VertexShader, "rec/basic.vert");
	defaultFrag.createShader(Shader::FragmentShader, "rec/basic.frag");

    //enable face culling for better performance
    glEnable(GL_CULL_FACE);

    //do some research on this...
    glEnable(GL_DEPTH_TEST);
		
	//enable msaa
    glEnable(GL_MULTISAMPLE);
	
	//generate Framebuffer and bind it
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	
	//generate the texture that the framebuffer will render to
	glGenTextures(1, &FBTexture);
	glBindTexture(GL_TEXTURE_2D, FBTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.width, window.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBTexture, 0);
	
	//texture params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window.width, window.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, FBTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("Framebuffer creation failed");
	
	//start the renderloop after init is done.
	renderLoop();
}

void Core::loadClear(char (&cur)[1024])
{
    //clear
    memset(cur, 0, sizeof(cur));

    //if we have no models, don't worry about reloading the buffer.
    if(models.size() == 0)
        return;

    //actually load the selected model name into the buffer.
    for(unsigned int i = 0;i != models[static_cast<size_t>(currentItem)].modelName.length(); i++)
    {
        cur[i] = models[static_cast<size_t>(currentItem)].modelName[i];
    }
}

void Core::loadOrUnloadModel(float (&selectedPos)[3], float (&selectedRot)[3], float (&selectedScl)[3], bool load)
{
    if(!load)
    {
        //position
        selectedPos[0] = models[static_cast<size_t>(currentItem)].position.x;
        selectedPos[1] = models[static_cast<size_t>(currentItem)].position.y;
        selectedPos[2] = models[static_cast<size_t>(currentItem)].position.z;

        //rotation
        selectedRot[0] = models[static_cast<size_t>(currentItem)].EulerAngle.x;
        selectedRot[1] = models[static_cast<size_t>(currentItem)].EulerAngle.y;
        selectedRot[2] = models[static_cast<size_t>(currentItem)].EulerAngle.z;

        //scale
        selectedScl[0] = models[static_cast<size_t>(currentItem)].scale.x;
        selectedScl[1] = models[static_cast<size_t>(currentItem)].scale.y;
        selectedScl[2] = models[static_cast<size_t>(currentItem)].scale.z;
    }
    else 
	{
        //position
        models[static_cast<size_t>(currentItem)].position.x = selectedPos[0];
        models[static_cast<size_t>(currentItem)].position.y = selectedPos[1];
        models[static_cast<size_t>(currentItem)].position.z = selectedPos[2];

        //rotation
        models[static_cast<size_t>(currentItem)].EulerAngle.x = selectedRot[0];
        models[static_cast<size_t>(currentItem)].EulerAngle.y = selectedRot[1];
        models[static_cast<size_t>(currentItem)].EulerAngle.z = selectedRot[2];

        //scale
        models[static_cast<size_t>(currentItem)].scale.x = selectedScl[0];
        models[static_cast<size_t>(currentItem)].scale.y = selectedScl[1];
        models[static_cast<size_t>(currentItem)].scale.z = selectedScl[2];
    }
}


//draws the main menu with all of the model objects like position and name ect, also allows changing of position and name
//and other variables of the model
void Core::drawMenu()
{
    //a boolean to check if the initial model scale has been set.
    static bool first = false;

	static bool modelError = false;

    static float selectedPos[3];
    static float selectedRot[3];
    static float selectedScl[3];

    if(!first)
    {
        selectedScl[0] = 1.0f;
        selectedScl[1] = 1.0f;
        selectedScl[2] = 1.0f;
        first = true;
    }
	
    std::string temp = "Driver vendor : ";
    std::string glString = ((char*)glGetString(GL_VENDOR));
    temp.append(glString);
    ImGui::Text(temp.c_str());

    static char curModelName[1024];
    ImGui::Text("%.1fps", static_cast<double>(ImGui::GetIO().Framerate));
	
	mainCamera.widthH = window.width;
	mainCamera.heightH = window.height;
    ImGui::SliderFloat("FOV", &mainCamera.fov, 30, 110);
	
    mainCamera.calc();

    //model position input
    static float spawnPos[3];
    ImGui::InputFloat3("Spawn Position", spawnPos);

	//model path input
	static char buf[1024] = "rec/hatka_local_.obj";

    ImGui::InputText("Model Path", buf, IM_ARRAYSIZE(buf));
    if(ImGui::Button("Load Model"))
    {
        if(currentItem == -1)
            currentItem = 0;

        Model temp;
        //if model loading was successful then process the model further
		if (temp.loadModel(buf, defaultVert, defaultFrag))
		{
			idCounter++;
			temp.position.x = spawnPos[0];
			temp.position.y = spawnPos[1];
			temp.position.z = spawnPos[2];
			temp.id = idCounter;
			models.push_back(temp);
			modelNames.push_back(temp.modelName);
			modelError = false;
		}
		else
			modelError = true;
    }

	if (modelError)
	{ 
		ImGui::SameLine();
		ImGui::Text("Model failed to load.");
	}

    //if no models are loaded, don't worry about anything related to modifying models or else... segfault.
    if(models.size() == 0)
        return;

    ImGui::InputFloat3("Position", selectedPos);
    ImGui::InputFloat3("Rotation", selectedRot);
    ImGui::InputFloat3("Scale", selectedScl);

    //basically iterates through every model to make sure none of them have the same name,
    //if they do have the same name then append the id of model to the model name.
    for(std::vector<Model>::iterator i = models.begin(); i != models.end(); i++)
    {
        for(std::vector<Model>::iterator mod = models.begin(); mod != models.end(); mod++)
        {
            //if both loops are on the same model, then ignore it.
            if(std::distance(models.begin(), i) == std::distance(models.begin(), mod))
                continue;
            //if it happens to find 2 models with the same name then append the id of the model to prevent naming conflicts.
            if(i->modelName == mod->modelName)
            {
                //actually append the id to the model string
                mod->modelName.append(std::to_string(mod->id));
            }
        }
    }


    //clear and reload the modelNames array to reflect name changes made in the models array
    modelNames.clear();
    for(Model m : models)
    {
        modelNames.push_back(m.modelName);
    }

    //convert all of the model names (std::string) to const char* array so imgui may use it
	std::vector<const char*> array(modelNames.size());

    for (unsigned int i = 0; i != modelNames.size(); i++) {
        array[i] = modelNames[i].c_str();
    }

    //if the selected item in the listbox has changed, clear the change name array and reload it with the selected model name
    if(ImGui::ListBox("Models", &currentItem, array.data(), static_cast<int>(array.size())))
    {
        loadClear(curModelName);
        //update the input float positions to the current model positions
        loadOrUnloadModel(selectedPos, selectedRot, selectedScl, false);

    }

    //if the selected item hasen't changed then check for changes in position and check for
    //the button press to change the model name.
    else
    {
        //the input text box for model name changing.
        ImGui::InputText("Current Model Name", curModelName, IM_ARRAYSIZE(curModelName));

        //if the change name button is pressed, change the model name to the input text box data.
        if(ImGui::Button("Change Name"))
        {
            models[static_cast<size_t>(currentItem)].modelName = curModelName;
        }

        //update model position and rotation from float input.
        loadOrUnloadModel(selectedPos, selectedRot, selectedScl, true);

        //if the delete button is pressed, then delete the current model and exit the menu drawing function
        if(ImGui::Button("Delete Model"))
        {
            //if the user trys to delete the last model in the array, pull back the current item index so the program
            //doesn't try and reference an object that doesn't exist
            if(models.size() == 1)
            {
				models.begin()->deleteBuffers();
				models.begin()->cleanup();
                models.erase(models.begin());

                currentItem = 0;
                loadClear(curModelName);
                return;
            }
            if(static_cast<unsigned long>(currentItem+1) == models.size()) 
			{
				(models.begin() + currentItem)->deleteBuffers();
				(models.begin() + currentItem)->cleanup();
                models.erase(models.begin() + currentItem);
				
                currentItem--;
			}
			else
			{
				(models.begin() + currentItem)->deleteBuffers();
				(models.begin() + currentItem)->cleanup();
				models.erase(models.begin() + currentItem);
			}

            /*this is here to reload the model changing box when a model is deleted*/
            loadClear(curModelName);
        }
    }
}

void Core::renderLoop()
{
    //render flags
    static bool wireFrame = false;
    //if we want to render objects (doesn't include imgui).
    static bool toRender = true;
    static bool vsync = true;

    //render loop :)
    while(!glfwWindowShouldClose(window.window))
    {
		if (width > 0)
		{
			window.width = width;
			window.height = height;
		}
		updateCamera(window.width, window.height);

        glfwSwapInterval(vsync);

        if(wireFrame)
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        else
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        glfwPollEvents();

        //Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //clear both framebuffers
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		static bool hasSet = false;
		static bool temp;
		ImGui::SetNextWindowSize(ImVec2(window.width, window.height));
		if (!hasSet)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			hasSet = true;
		}
		
		
		ImGui::Begin("view", &temp, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove);
		{
		    //draw the image on the screen
			ImGui::Image((void*)FBTexture, ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::End();

        ImGui::Begin("Control Panel");
        drawMenu();

        //setup checkboxes for the render flags
        ImGui::Checkbox("Wireframe Mode", &wireFrame);
        ImGui::Checkbox("Render?", &toRender);
        ImGui::Checkbox("Vsync", &vsync);

        ImGui::End();


		
		ImGui::Begin("tester");
		static ImGuiID id = ImGui::GetID("DOCK");
		ImGui::DockSpace(id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::End();

		ImGui::SetNextWindowDockID(id, ImGuiCond_::ImGuiCond_Once);
		
		if (ImGui::Begin("Dockable Window"))
		{
		}
		ImGui::End();
        ImGui::Render();

        //iterate through all of models and draw with the main camera
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        if(toRender)
        {
            if (models.size() > 0)
            {
                for (Model m : models)
                {
                    m.draw(m.position, m.EulerAngle, m.scale, mainCamera);
                }
            }
        }
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window.width, window.height);

		mainCamera.camFront = glm::normalize(camFront);	
		if (input.isKeyPressed(GLFW_KEY_W))
		{
			mainCamera.position += (camSpeed * deltaTime) * mainCamera.camFront;
		}
		if (input.isKeyPressed(GLFW_KEY_S))
		{
			mainCamera.position -= (camSpeed * deltaTime) * mainCamera.camFront;
		}
		//DO SOME RESEARCH ON HOW THIS WORKS
		if (input.isKeyPressed(GLFW_KEY_A))
		{
			mainCamera.position -= glm::normalize(glm::cross(mainCamera.camFront, mainCamera.camUp)) * (camSpeed * deltaTime);
		}
		if (input.isKeyPressed(GLFW_KEY_D))
		{
			mainCamera.position += glm::normalize(glm::cross(mainCamera.camFront, mainCamera.camUp)) * (camSpeed * deltaTime);
		}

		//show and hide the cursor if we are holding the right mouse button or not.
		mouseState = glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_RIGHT);
		if (mouseState == GLFW_PRESS)
			glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
		{
			isMouse = true;
			glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		
		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window.window);
    }
    //delete all of the heap allocated models and clear all of the opengl objects associated with them.
    for (Model m : models)
    {
		m.deleteBuffers();
        m.cleanup();
    }
	//destroy the FMOD sound system.
    soundSystem.destroySS();
}

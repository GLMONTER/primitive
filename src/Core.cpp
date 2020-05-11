//
// Created by monter on 6/2/19.
//


#include "Core.hpp"
#include<algorithm>
//for file dialog
#include <shobjidl.h>
#include <locale>

static int collisionModelID = 0;

//camera variables
glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
static float pitch, yaw;

static float selectedPos[3];
static float selectedRot[3];
static float selectedScl[3];

static float selectedCollisionPos[3];
static float selectedCollisionScl[3];

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

extern void update(std::vector<Model>& modelArray);
extern void start(GLFWwindow* window);

extern bool inputFlag;

extern bool editorEnable;
bool cameraGameEnable = false;
//button toggle variables
static bool buttonPressed = false;
static bool buttonToggle = false;

static char curModelName[1024];

char workingDir[256];

Model* Core::findObject(std::string name)
{
	if (modelNames.size() == 0)
		return nullptr;
	for (unsigned int i = 0; i != modelNames.size(); i++)
	{
		if (modelNames[i] == name)
			return &models[i];
	}
}

void Core::deleteModel(unsigned int index, char (&modelNamesArray)[1024])
{
	static bool colDeleted = false;
	//checking if erasing any collider models is required.
	int tempID = 0;
	if (!models[index].col.isNull)
	{
		tempID = models[index].col.id;
		(collisionModels.begin() + models[index].col.id)->deleteBuffers();
		collisionModels.erase(collisionModels.begin() + models[index].col.id);
		colDeleted = true;
	}

	//if the user trys to delete the last model in the array, pull back the current item index so the program
	//doesn't try and reference an object that doesn't exist
	if (models.size() == 1)
	{
		models.begin()->deleteBuffers();
		modelNames.clear();
		models.begin()->cleanup();
		models.erase(models.begin());

		currentItem = 0;
		loadClear(modelNamesArray);
		return;
	}
	std::vector<std::string>::iterator iter;
	if (static_cast<unsigned long>(index) + 1 == models.size())
	{
		(models.begin() + index)->deleteBuffers();
		(models.begin() + index)->cleanup();
		iter = std::find(modelNames.begin(), modelNames.end(), (models.begin() + index)->modelName);
		modelNames.erase(iter);

		models.erase(models.begin() + index);

		index--;
	}
	else
	{
		(models.begin() + index)->deleteBuffers();
		(models.begin() + index)->cleanup();
		iter = std::find(modelNames.begin(), modelNames.end(), (models.begin() + index)->modelName);
		modelNames.erase(iter);
		models.erase(models.begin() + index);
	}
	/*this is here to reload the model changing box when a model is deleted*/
	loadClear(modelNamesArray);

	//rearrange array id's for collider models if a collider is deleted.
	if (colDeleted)
	{
		for (Model& colModel : models)
		{
			if (colModel.col.id > tempID)
			{
				colModel.col.id--;
			}
		}
		colDeleted = false;
		collisionModelID--;
	}
}

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
	if (editorEnable)
	{
		if (mouseState == GLFW_PRESS)
		{
			camFront.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
			camFront.y = sin(glm::radians(pitch));
			camFront.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		}
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
	mainCamera.calc(nullptr);
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
	GetCurrentDirectoryA(256, workingDir);

	std::ifstream f("config.txt");
	if (f.get() == '0')
		editorEnable = false;
	else
		editorEnable = true;
	//init the sound system
	SoundSystem::init();

	//create the window and set the camera width and height to have the correct camera matrix without resizing.
	if(editorEnable)
		window.createWindow(false);
	else
		window.createWindow(false);


	//change camera parameters for matrix calculations
	mainCamera.widthH = window.width;
	mainCamera.heightH = window.height;
	
	//recalculate the camera matrix
	mainCamera.calc(nullptr);
	
	//init GLEW
	if(glewInit() != GLEW_OK)
		throw std::runtime_error("GLEW FAILED TO INIT.");

	//set the window resize callback to change things like the camera matrix
	glfwSetWindowSizeCallback(window.window, Core::glfwFramebufferSizeCallback);
	glfwSetCursorPosCallback(window.window, GLFW_MouseCallback);

	//init the input system.
	input.init(window.window);

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	//setup imgui Context
	ImGui::CreateContext();

	//setup imgui io and enable docking, not needed right now.
	
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//enable window docking
	io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
	 

	//Setup imgui theme or style
	ImGui::StyleColorsClassic();

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
	
	start(window.window);
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
static bool modelError = false;
void Core::loadScene(std::string scenePath)
{

	std::ifstream ifs(scenePath);

	std::string line;
	while (std::getline(ifs, line))
	{
		//vector of strings to represent all loaded data
		std::vector<std::string> strings;

		//the string to push to the vector representing a piece of the parsed data(model name, position ect.)
		std::string push;

		//load a part of the strings
		static bool flag = false;
		for (std::string::iterator i = line.begin(); i != line.end(); i++)
		{
			//until we hit a space keep loading data
			if (*i != ' ')
			{
				push += *i;
			}
			//if a space is hit then load the element
			else
			{
				strings.push_back(push);
				push.clear();
				continue;
			}
			//if the iterator is at the end of string then push back the last element and leave.
			if (i == line.end() - 1)
			{
				strings.push_back(push);
				break;
			}
		}
		//load a model with the loaded data
		Model mod;
		//if model loading was successful then process the model further
		if (mod.loadModel(strings[0], defaultVert, defaultFrag))
		{
			
			idCounter++;
			mod.position.x = std::stof(strings[2]);
			mod.position.y = std::stof(strings[3]);
			mod.position.z = std::stof(strings[4]);
			mod.spawnPosition = mod.position;
			mod.EulerAngle.x = std::stof(strings[5]);
			mod.EulerAngle.y = std::stof(strings[6]);
			mod.EulerAngle.z = std::stof(strings[7]);
			mod.modelName = strings[1];
		
			mod.scale.x = std::stof(strings[8]);
			mod.scale.y = std::stof(strings[9]);
			mod.scale.z = std::stof(strings[10]);
			mod.id = idCounter;

			if (strings.size() >11)
			{
				mod.col.isNull = false;
				if (collisionModels.size() == 0)
				{
					mod.col.id = 0;
					collisionModelID = 0;
				}
				else
				{
					collisionModelID++;

					mod.col.id = collisionModelID;
				}
				externalModel tempModel;
				//if model loading was successful then process the model further
				tempModel.loadModel(workingDir + std::string("/rec/cube.fbx"), defaultVert, defaultFrag);

				tempModel.position.x = std::stof(strings[11]);
				tempModel.position.y = std::stof(strings[12]);
				tempModel.position.z = std::stof(strings[13]);

				collisionModels.push_back(tempModel);
			}

			models.push_back(mod);
			if (models.size() == 1)
			{
				//position
				selectedPos[0] = models[0].position.x;
				selectedPos[1] = models[0].position.y;
				selectedPos[2] = models[0].position.z;

				//rotation
				selectedRot[0] = models[0].EulerAngle.x;
				selectedRot[1] = models[0].EulerAngle.y;
				selectedRot[2] = models[0].EulerAngle.z;

				//scale
				selectedScl[0] = models[0].scale.x;
				selectedScl[1] = models[0].scale.y;
				selectedScl[2] = models[0].scale.z;
			}
			modelNames.push_back(mod.modelName);
			modelError = false;
		}
		else
			modelError = true;

		strings.clear();
	}
}

//draws the main menu with all of the model objects like position and name ect, also allows changing of position and name
//and other variables of the model
void Core::drawMenu()
{
	//a boolean to check if the initial model scale has been set.
	static bool first = false;

	if (!first)
	{
		selectedScl[0] = 1.0f;
		selectedScl[1] = 1.0f;
		selectedScl[2] = 1.0f;
		first = true;
	}

	std::string vendor = "Driver vendor : ";
	std::string glString = ((char*)glGetString(GL_VENDOR));
	vendor.append(glString);
	ImGui::Text(vendor.c_str());

	
	ImGui::Text("%.1fps", static_cast<double>(ImGui::GetIO().Framerate));

	mainCamera.widthH = window.width;
	mainCamera.heightH = window.height;
	ImGui::SliderFloat("FOV", &mainCamera.fov, 30, 110);

	mainCamera.calc(nullptr);

	//model position input
	static float spawnPos[3];
	ImGui::InputFloat3("Spawn Position", spawnPos);

	//buffer to hold the model path
	static char buffer[1024];

	if (ImGui::Button("Load Model"))
	{
#ifdef WIN32
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
			COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileOpenDialog* pFileOpen;

			// Create the FileOpenDialog object.
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
				IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				hr = pFileOpen->Show(NULL);

				// Get the file name from the dialog box.
				if (SUCCEEDED(hr))
				{
					IShellItem* pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						wchar_t* pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						//convert the wide string given by windows to a normal string
						wcstombs(buffer, pszFilePath, sizeof(buffer));						// Display the file name to the user.

						if (SUCCEEDED(hr))
						{
							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pFileOpen->Release();
			}
			CoUninitialize();
		}
#endif
		if (currentItem == -1)
			currentItem = 0;

		Model tempModel;
		std::string conv = buffer;
		//if model loading was successful then process the model further
		if (tempModel.loadModel(conv, defaultVert, defaultFrag))
		{
			idCounter++;
			tempModel.position.x = spawnPos[0];
			tempModel.position.y = spawnPos[1];
			tempModel.position.z = spawnPos[2];
			tempModel.id = idCounter;
			models.push_back(tempModel);
			modelNames.push_back(tempModel.modelName);
			modelError = false;
		}
		else
			modelError = true;
	}
	//if the model failed to load, write text with imgui.
	if (modelError)
	{
		ImGui::SameLine();
		ImGui::Text("Model failed to load.");
	}
	//the char array that holds the scene path.
	static char scenePath[1024] = "test.txt";

	if (ImGui::Button("Save Scene"))
	{
		std::ofstream ofs;
		//open the scene file and clear any data in it to prepare for writing.
		ofs.open(scenePath, std::ofstream::out | std::ofstream::trunc);

		for (Model m : models)
		{
			//write all model information to scene file
			ofs << m.path << ' ' << m.modelName
				//write position
				<< ' ' << std::to_string(m.position.x) << ' ' << std::to_string(m.position.y) << ' ' << std::to_string(m.position.z)
				//write rotation
				<< ' ' << std::to_string(m.EulerAngle.x) << ' ' << std::to_string(m.EulerAngle.y) << ' ' << std::to_string(m.EulerAngle.z)
				//write scale
				<< ' ' << std::to_string(m.scale.x) << ' ' << std::to_string(m.scale.y) << ' ' << std::to_string(m.scale.z);
			
			if (!m.col.isNull)
			{
				ofs << ' ' << std::to_string(m.col.colPosition.x) << ' ' << std::to_string(m.col.colPosition.y) << ' ' << std::to_string(m.col.colPosition.z);
			}
			ofs << std::endl;

		}
		ofs.close();
	}

	ImGui::SameLine();
	if (ImGui::Button("Load Scene"))
	{
		loadScene(scenePath);
	}

	ImGui::InputText("Scene Path", scenePath, IM_ARRAYSIZE(scenePath));

	//if no models are loaded, don't worry about anything related to modifying models or else... segfault.
	if (models.size() == 0)
	{
		//the setting is to prevent new models from taking the scales, positions, and rotations of older deleted models.
		selectedScl[0] = 1;
		selectedScl[1] = 1;
		selectedScl[2] = 1;

		//position
		selectedPos[0] = 0;
		selectedPos[1] = 0;
		selectedPos[2] = 0;

		//rotation
		selectedRot[0] = 0;
		selectedRot[1] = 0;
		selectedRot[2] = 0;

		return;
	}

	ImGui::InputFloat3("Position", selectedPos);
	ImGui::InputFloat3("Rotation", selectedRot);
	ImGui::InputFloat3("Scale", selectedScl);

	if (!models[currentItem].col.isNull)
	{
		ImGui::InputFloat3("Collider Position", selectedCollisionPos);
		//ImGui::InputFloat3("Collider Scale", selectedCollisionScl);
	}

	//basically iterates through every model to make sure none of them have the same name,
	//if they do have the same name then append the id of model to the model name.
	for (std::vector<Model>::iterator i = models.begin(); i != models.end(); i++)
	{
		for (std::vector<Model>::iterator mod = models.begin(); mod != models.end(); mod++)
		{
			//if both loops are on the same model, then ignore it.
			if (std::distance(models.begin(), i) == std::distance(models.begin(), mod))
				continue;
			//if it happens to find 2 models with the same name then append the id of the model to prevent naming conflicts.
			if (i->modelName == mod->modelName)
			{
				//actually append the id to the model string
				mod->modelName.append(std::to_string(mod->id));
			}
		}
	}

	//clear and reload the modelNames array to reflect name changes made in the models array
	modelNames.clear();
	for (Model m : models)
	{
		modelNames.push_back(m.modelName);
	}

	//convert all of the model names (std::string) to const char* array so imgui may use it
	std::vector<const char*> array(modelNames.size());

	for (unsigned int i = 0; i != modelNames.size(); i++) {
		array[i] = modelNames[i].c_str();
	}

	//if the selected item in the listbox has changed, clear the change name array and reload it with the selected model name
	if (ImGui::ListBox("Models", &currentItem, array.data(), static_cast<int>(array.size())))
	{
		loadClear(curModelName);
		//update the input float positions to the current model positions
		loadOrUnloadModel(selectedPos, selectedRot, selectedScl, false);

		if (!models[currentItem].col.isNull)
		{
			selectedCollisionPos[0] = collisionModels[models[currentItem].col.id].abstractMeshes[0]->offset.x;
			selectedCollisionPos[1] = collisionModels[models[currentItem].col.id].abstractMeshes[0]->offset.y;
			selectedCollisionPos[2] = collisionModels[models[currentItem].col.id].abstractMeshes[0]->offset.z;

			selectedCollisionScl[0] = collisionModels[models[currentItem].col.id].abstractMeshes[0]->Scale.x;
			selectedCollisionScl[1] = collisionModels[models[currentItem].col.id].abstractMeshes[0]->Scale.y;
			selectedCollisionScl[2] = collisionModels[models[currentItem].col.id].abstractMeshes[0]->Scale.z;
		}
	}

	//if the selected item hasen't changed then check for changes in position and check for
	//the button press to change the model name.
	else
	{
		static unsigned int sizeofModels;
		static unsigned int sizeofCollisions;
		//the input text box for model name changing.
		ImGui::InputText("Current Model Name", curModelName, IM_ARRAYSIZE(curModelName));

		//if the change name button is pressed, change the model name to the input text box data.
		if (ImGui::Button("Change Name"))
		{
			models[static_cast<size_t>(currentItem)].modelName = curModelName;
		}

		//if the user has deleted a model then we need to update the imgui inputs to match
		//the new model data so we don't overwrite it with the previous model data
		if (sizeofModels != models.size() || sizeofCollisions != collisionModels.size())
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

			if (!models[currentItem].col.isNull)
			{
				selectedCollisionPos[0] = collisionModels[models[currentItem].col.id].abstractMeshes[0]->offset.x;
				selectedCollisionPos[1] = collisionModels[models[currentItem].col.id].abstractMeshes[0]->offset.y;
				selectedCollisionPos[2] = collisionModels[models[currentItem].col.id].abstractMeshes[0]->offset.z;

				selectedCollisionScl[0] = collisionModels[models[currentItem].col.id].abstractMeshes[0]->Scale.x;
				selectedCollisionScl[1] = collisionModels[models[currentItem].col.id].abstractMeshes[0]->Scale.y;
				selectedCollisionScl[2] = collisionModels[models[currentItem].col.id].abstractMeshes[0]->Scale.z;
			}
		}
		
		//update model position and rotation from float input.
		loadOrUnloadModel(selectedPos, selectedRot, selectedScl, true);
		if (!models[currentItem].col.isNull)
		{
			collisionModels[models[currentItem].col.id].abstractMeshes[0]->offset.x = selectedCollisionPos[0];
			collisionModels[models[currentItem].col.id].abstractMeshes[0]->offset.y = selectedCollisionPos[1];
			collisionModels[models[currentItem].col.id].abstractMeshes[0]->offset.z = selectedCollisionPos[2];

			collisionModels[models[currentItem].col.id].abstractMeshes[0]->Scale.x = selectedCollisionScl[0];
			collisionModels[models[currentItem].col.id].abstractMeshes[0]->Scale.y = selectedCollisionScl[1];
			collisionModels[models[currentItem].col.id].abstractMeshes[0]->Scale.z = selectedCollisionScl[2];
		}
	

		sizeofCollisions = collisionModels.size();
		sizeofModels = models.size();
		//if the delete button is pressed, then delete the current model and exit the menu drawing function
		if (ImGui::Button("Delete Model"))
		{
			deleteModel(currentItem, curModelName);
		}

		if (models[currentItem].col.isNull == true)
		{
			if (ImGui::Button("Add Box Collider"))
			{
				models[currentItem].col.isNull = false;
				if (collisionModels.size() == 0)
				{
					models[currentItem].col.id = 0;
					collisionModelID = 0;
				}
				else
				{
					collisionModelID++;

					models[currentItem].col.id = collisionModelID;
				}
				externalModel tempModel;
				//if model loading was successful then process the model further
				tempModel.loadModel(workingDir + std::string("/rec/cube.fbx"), defaultVert, defaultFrag);

				tempModel.position.x = models[currentItem].position.x;
				tempModel.position.y = models[currentItem].position.y;
				tempModel.position.z = models[currentItem].position.z;

				collisionModels.push_back(tempModel);


				ImGui::SameLine();
				ImGui::Button("Add Sphere Collider");
			}
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

		if (editorEnable)
		{
			//Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		//clear both framebuffers
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		static bool hasSet = false;
		static bool temp;
		
		if (editorEnable)
		{
			ImGui::SetNextWindowSize(ImVec2(window.width, window.height));
			if (!hasSet)
			{
				ImGui::SetNextWindowPos(ImVec2(0, 0));
				hasSet = true;
			}
			//Master dock window
			ImGui::Begin("Master", &temp, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);
			{
				//draw the image on the screen
				static ImGuiID dockspaceID = 0;
				// Declare Central dockspace
				dockspaceID = ImGui::GetID("HUB_DockSpace");
				ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_::ImGuiDockNodeFlags_PassthruCentralNode);

				ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
			}
			ImGui::End();
			//draw the viewport as an imgui image
			ImGui::Begin("Viewport", &temp, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);
			{
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
			

			ImGui::Render();
		}

		//iterate through all of models and draw with the main camera
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		if (editorEnable)
		{
			cameraGameEnable = false;
			if (toRender)
			{
				if (models.size() > 0)
				{
					for (int i = 0; i != models.size(); i++)
					{
						models[i].draw(models[i].position, models[i].EulerAngle, models[i].scale, mainCamera);
						if (!models[currentItem].col.isNull)
						{
							collisionModels[models[currentItem].col.id].draw(collisionModels[models[currentItem].col.id].position, collisionModels[models[currentItem].col.id].EulerAngle,
								collisionModels[models[currentItem].col.id].scale, mainCamera);
						}
					}
				}
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		if(!editorEnable)
		{
			if (toRender)
			{
				if (models.size() > 0)
				{
					for (int i = 0; i != models.size(); i++)
					{
						models[i].draw(models[i].position, models[i].EulerAngle, models[i].scale, mainCamera);
						if (!models[currentItem].col.isNull)
						{
							collisionModels[models[currentItem].col.id].draw(collisionModels[models[currentItem].col.id].position, collisionModels[models[currentItem].col.id].EulerAngle,
								collisionModels[models[currentItem].col.id].scale, mainCamera);
						}
					}
				}
			}
		}
		glViewport(0, 0, window.width, window.height);

		mainCamera.camFront = glm::normalize(camFront);	
		if (editorEnable)
		{
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
			if (input.isKeyPressed(GLFW_KEY_E))
			{
				glm::vec3 cameraRight = glm::cross(mainCamera.camUp, direction);
				mainCamera.position += glm::cross(direction, cameraRight) * (camSpeed * deltaTime);
			}
			if (input.isKeyPressed(GLFW_KEY_Q))
			{
				glm::vec3 cameraRight = glm::cross(mainCamera.camUp, direction);
				mainCamera.position -= glm::cross(direction, cameraRight) * (camSpeed * deltaTime);
			}
		}
		//button toggle system to toggle the editor on and off
		if (input.isKeyPressed(GLFW_KEY_ESCAPE))
		{
			if (!buttonPressed)
			{
				buttonToggle = 1 - buttonToggle;
				buttonPressed = 1;

				//the iteration of models resets the scene when swtiching back to the editor
				if (editorEnable == 1)
				{
					for (Model m : models)
					{
						m.spawnPosition = m.position;
					}
				}
				else
				{
					for (Model m : models)
					{
						m.position = m.spawnPosition;
					}
				}
			}
		}
		else
			buttonPressed = 0;

		if (buttonToggle)
		{
			editorEnable = 0;
		}
		else
			editorEnable = 1;

		//show and hide the cursor if we are holding the right mouse button or not.
		if (editorEnable)
		{
			mouseState = glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_RIGHT);
			if (mouseState == GLFW_PRESS)
				glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			else
			{
				isMouse = true;
				glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}
		
		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (editorEnable)
		{
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window.window);
		if (!editorEnable)
		{
			update(models);
		}

		//update collision boxes relative to there models
		for (Model m : models)
		{
			if (!m.col.isNull)
			{
				
				collisionModels[m.col.id].position = collisionModels[m.col.id].abstractMeshes[0]->offset + m.position;
				
				for (int i = 0; i != collisionModels[m.col.id].abstractMeshes[0]->vertices.size(); i++)
				{
					collisionModels[m.col.id].abstractMeshes[0]->vertices[i].Position = (collisionModels[m.col.id].abstractMeshes[0]->vertices[i].staticPosition + collisionModels[m.col.id].abstractMeshes[0]->offset + collisionModels[m.col.id].position) * collisionModels[m.col.id].abstractMeshes[0]->Scale;
				}
			}
		}
	}
	//delete all of the heap allocated models and clear all of the opengl objects associated with them.
	for (Model m : models)
	{
		m.deleteBuffers();
		m.cleanup();
	}
	//terminate GLFW
	glfwTerminate();

	//destroy the FMOD sound system.
	soundSystem.destroySS();
}

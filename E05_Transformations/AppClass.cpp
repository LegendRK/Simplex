#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(1.0f, C_GRAY);

	// populate the vector

	// row 1
	m_v3Positions.push_back(vector3(-3.0f, 4.0f, 0.0f));
	m_v3Positions.push_back(vector3(3.0f, 4.0f, 0.0f));

	// row 2
	m_v3Positions.push_back(vector3(-2.0f, 3.0f, 0.0f));
	m_v3Positions.push_back(vector3(2.0f, 3.0f, 0.0f));

	// row 3
	m_v3Positions.push_back(vector3(-3.0f, 2.0f, 0.0f));
	m_v3Positions.push_back(vector3(-2.0f, 2.0f, 0.0f));
	m_v3Positions.push_back(vector3(-1.0f, 2.0f, 0.0f));
	m_v3Positions.push_back(vector3(0.0f, 2.0f, 0.0f));
	m_v3Positions.push_back(vector3(1.0f, 2.0f, 0.0f));
	m_v3Positions.push_back(vector3(2.0f, 2.0f, 0.0f));
	m_v3Positions.push_back(vector3(3.0f, 2.0f, 0.0f));

	// row 4
	m_v3Positions.push_back(vector3(-4.0f, 1.0f, 0.0f));
	m_v3Positions.push_back(vector3(-3.0f, 1.0f, 0.0f));
	m_v3Positions.push_back(vector3(-1.0f, 1.0f, 0.0f));
	m_v3Positions.push_back(vector3(0.0f, 1.0f, 0.0f));
	m_v3Positions.push_back(vector3(1.0f, 1.0f, 0.0f));
	m_v3Positions.push_back(vector3(3.0f, 1.0f, 0.0f));
	m_v3Positions.push_back(vector3(4.0f, 1.0f, 0.0f));

	// row 5
	m_v3Positions.push_back(vector3(-5.0f, 0.0f, 0.0f));
	m_v3Positions.push_back(vector3(-4.0f, 0.0f, 0.0f));
	m_v3Positions.push_back(vector3(-3.0f, 0.0f, 0.0f));
	m_v3Positions.push_back(vector3(-2.0f, 0.0f, 0.0f));
	m_v3Positions.push_back(vector3(-1.0f, 0.0f, 0.0f));
	m_v3Positions.push_back(vector3(0.0f, 0.0f, 0.0f));
	m_v3Positions.push_back(vector3(1.0f, 0.0f, 0.0f));
	m_v3Positions.push_back(vector3(2.0f, 0.0f, 0.0f));
	m_v3Positions.push_back(vector3(3.0f, 0.0f, 0.0f));
	m_v3Positions.push_back(vector3(4.0f, 0.0f, 0.0f));
	m_v3Positions.push_back(vector3(5.0f, 0.0f, 0.0f));

	// row 6
	m_v3Positions.push_back(vector3(-5.0f, -1.0f, 0.0f));
	m_v3Positions.push_back(vector3(-3.0f, -1.0f, 0.0f));
	m_v3Positions.push_back(vector3(-2.0f, -1.0f, 0.0f));
	m_v3Positions.push_back(vector3(-1.0f, -1.0f, 0.0f));
	m_v3Positions.push_back(vector3(0.0f, -1.0f, 0.0f));
	m_v3Positions.push_back(vector3(1.0f, -1.0f, 0.0f));
	m_v3Positions.push_back(vector3(2.0f, -1.0f, 0.0f));
	m_v3Positions.push_back(vector3(3.0f, -1.0f, 0.0f));
	m_v3Positions.push_back(vector3(5.0f, -1.0f, 0.0f));

	// row 7
	m_v3Positions.push_back(vector3(-5.0f, -2.0f, 0.0f));
	m_v3Positions.push_back(vector3(-3.0f, -2.0f, 0.0f));
	m_v3Positions.push_back(vector3(3.0f, -2.0f, 0.0f));
	m_v3Positions.push_back(vector3(5.0f, -2.0f, 0.0f));

	// row 8
	m_v3Positions.push_back(vector3(-2.0f, -3.0f, 0.0f));
	m_v3Positions.push_back(vector3(-1.0f, -3.0f, 0.0f));
	m_v3Positions.push_back(vector3(1.0f, -3.0f, 0.0f));
	m_v3Positions.push_back(vector3(2.0f, -3.0f, 0.0f));

	// have it start more to the left
	for (uint i = 0; i < m_v3Positions.size(); i++)
		m_v3Positions[i] -= vector3(5, 0, 0);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	
	static float value = 0.0f;
	matrix4 m4Translate;
	matrix4 m4Model;

	for (uint i = 0;i < m_v3Positions.size();  i++)
	{
		m4Translate = glm::translate(IDENTITY_M4, m_v3Positions[i] + vector3(value, 0.0f, 0.0f));
		m4Model = m4Translate;
		m_pMesh->Render(m4Projection, m4View, m4Model);
	}

	value += 0.03f;

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}
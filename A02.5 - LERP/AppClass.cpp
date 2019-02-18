#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Rohit Kaushik - rgk8966@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits
	float fRadius = 0.95f;

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager

		// get each stop an add it to a list
		std::vector<vector3> listOfStops;
		float radianAngle = glm::radians((float)(360 / i));
		vector3 pointToAdd = vector3(fRadius, 0, 0);
		for (uint j = 0;  j < i;  j++)
		{
			listOfStops.push_back(pointToAdd);
			pointToAdd = fRadius * vector3(glm::cos(radianAngle * (j + 1)), glm::sin(radianAngle * (j + 1)),  0);
		}
		// add the list to a master list of all the lists
		m_masterStopList.push_back(listOfStops);

		fSize += 0.5f; //increment the size for the next orbit
		fRadius += 0.5f;
		uColor -= static_cast<uint>(decrements); //decrease the wavelength

		// push back default values to node and timers
		m_currentRouteNodeList.push_back(0);
		m_listOfTimers.push_back(0.0f);
	}
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	//Get a timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	float fTimer = m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer
	for (uint t = 0; t < m_listOfTimers.size(); t++)
	{
		m_listOfTimers[t] += fTimer; //add the delta time to each val in the collection
	}

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		// set start and end pos using values from vector
		vector3 v3StartPos;
		vector3 v3EndPos;
		if (m_currentRouteNodeList[i] < m_masterStopList[i].size() - 1)
		{
			v3StartPos = m_masterStopList[i][m_currentRouteNodeList[i]];
			v3EndPos = m_masterStopList[i][m_currentRouteNodeList[i] + 1];
		}
		else // end of vector
		{
			v3StartPos = m_masterStopList[i][m_masterStopList[i].size() - 1];
			v3EndPos = m_masterStopList[i][0];
		}

		// get the rate / percent
		float fRate = static_cast<float>(MapValue(m_listOfTimers[i], 0.0f, 0.5f, 0.0f, 1.0f));

		//calculate the current position
		vector3 v3CurrentPos = glm::lerp(v3StartPos, v3EndPos, fRate);

		// rest the timer
		if (fRate >= 1.0f)
		{
			m_currentRouteNodeList[i]++;
			m_listOfTimers[i] = m_pSystem->GetDeltaTime(uClock);
			m_currentRouteNodeList[i] %= m_masterStopList[i].size();
		}

		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

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
	//release GUI
	ShutdownGUI();
}
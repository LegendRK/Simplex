#include "MyOctant.h"
using namespace Simplex;

// vars
uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 3;
uint MyOctant::m_uIdealEntityCount = 5;

// constructor
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();

	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;

	m_pRoot = this;
	m_lChild.clear();

	std::vector<vector3> entityMinMax;

	uint entityCount = m_pEntityMngr->GetEntityCount();
	for (int i = 0; i < entityCount; i++)
	{
		MyEntity * entity = m_pEntityMngr->GetEntity(i);
		MyRigidBody * rb = entity->GetRigidBody();
		entityMinMax.push_back(rb->GetMinGlobal());
		entityMinMax.push_back(rb->GetMaxGlobal());
	}
	MyRigidBody * rb = new MyRigidBody(entityMinMax);

	vector3 entityHalf = rb->GetHalfWidth();
	float maxLength = entityHalf.x;
	for (int i = 1; i < 3; i++)
	{
		if (maxLength < entityHalf[i])
			maxLength = entityHalf[i];
	}
	vector3 center = rb->GetCenterLocal();
	entityMinMax.clear();
	SafeDelete(rb);

	m_fSize = maxLength * 2.0;

	m_v3Center = center;
	m_v3Min = m_v3Center - (vector3(maxLength));
	m_v3Max = m_v3Center + (vector3(maxLength));

	m_uOctantCount++;
	ConstructTree(m_uMaxLevel);
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize) 
{
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}

// copy constructor
MyOctant::MyOctant(MyOctant const& other) 
{
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;

	m_fSize = other.m_fSize;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_pParent = other.m_pParent;
	for (int i = 0; i < 8; i++)
		m_pChild[i] = other.m_pChild[i];

	m_pRoot = other.m_pRoot;
	m_lChild = other.m_lChild;
}

// copy assignment constructor
MyOctant& MyOctant::operator=(MyOctant const& other) 
{
	if (this != &other) 
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}

// destructor
MyOctant::~MyOctant(void) 
{
	Release();
}


void MyOctant::Swap(MyOctant& other)
{
	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_fSize, other.m_fSize);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	std::swap(m_pParent, other.m_pParent);
	for (int i = 0; i < 8; i++)
		std::swap(m_pChild[i], other.m_pChild[i]);

	std::swap(m_pRoot, other.m_pRoot);
}

float MyOctant::GetSize(void)
{
	return m_fSize;
}

vector3 MyOctant::GetCenterGlobal(void) 
{
	return m_v3Center;
}

vector3 MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

vector3 MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

bool MyOctant::IsColliding(uint a_uRBIndex)
{
	uint nObjectsCount = m_pEntityMngr->GetEntityCount();
	if (a_uRBIndex >= nObjectsCount)
		return false;
	// create a temp object to store rb of entity index
	MyRigidBody * temp = m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody();

	// Do ARBB collision check
	if (m_v3Max.x < temp->GetMinGlobal().x) // check left
		return false;
	else if
		(m_v3Min.x > temp->GetMaxGlobal().x) // check right
		return false;

	if (m_v3Max.y < temp->GetMinGlobal().y) // check down
		return false;
	else if
		(m_v3Min.y > temp->GetMaxGlobal().y) // check up
		return false;

	if (m_v3Max.z < temp->GetMinGlobal().z) // check back
		return false;
	else if
		(m_v3Min.z > temp->GetMaxGlobal().z) // check front
		return false;

	return true;
}

void MyOctant::Display(uint a_nIndex, vector3 a_v3Color) 
{
	if (m_uID == a_nIndex) 
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
		return;
	}

	for (int i = 0; i < m_uChildren; i++) 
		m_pChild[i]->Display(a_nIndex);
}

void MyOctant::Display(vector3 a_v3Color) 
{
	for (int i = 0; i < m_uChildren; i++)
		m_pChild[i]->Display(a_v3Color);

	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void MyOctant::DisplayLeafs(vector3 a_v3Color) 
{
	uint leafs = m_lChild.size();
	for (int i = 0; i < leafs; i++)
		m_lChild[i]->DisplayLeafs(a_v3Color);

	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void MyOctant::ClearEntityList(void) 
{
	// get the leaf nodes
	for (int i = 0; i < m_uChildren; i++)
		m_pChild[i]->ClearEntityList();

	m_EntityList.clear();
}

void MyOctant::Subdivide(void) 
{
	// don't subdivide if at max
	if (m_uLevel >= m_uMaxLevel)
		return;

	// make sure it's a leaf
	if (m_uChildren != 0)
		return;

	// since octree, 8 children
	m_uChildren = 8;

	float quarterSize = m_fSize / 4.0f;
	float halfSize = quarterSize * 2.0f;
	vector3 center;

	center = m_v3Center;
	center.x -= quarterSize;
	center.y -= quarterSize;
	center.z -= quarterSize;

	m_pChild[0] = new MyOctant(center, halfSize); // bottom left back

	center.x += halfSize;
	m_pChild[1] = new MyOctant(center, halfSize); // bottom right back

	center.z += halfSize;
	m_pChild[2] = new MyOctant(center, halfSize); // bottom right front

	center.x -= halfSize;
	m_pChild[3] = new MyOctant(center, halfSize); // bottom left front

	center.y += halfSize;
	m_pChild[4] = new MyOctant(center, halfSize); // top left front

	center.z -= halfSize;
	m_pChild[5] = new MyOctant(center, halfSize); // top left back

	center.x += halfSize;
	m_pChild[6] = new MyOctant(center, halfSize); // top right back

	center.z += halfSize;
	m_pChild[7] = new MyOctant(center, halfSize); // top right front

	// set the values for each new child
	for (int i = 0; i < 8; i++)
	{
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;

		if (m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount))
			m_pChild[i]->Subdivide();
	}
}

MyOctant * Simplex::MyOctant::GetChild(uint a_nChild)
{
	if (a_nChild > 7) return nullptr;
	return m_pChild[a_nChild];
}


MyOctant * Simplex::MyOctant::GetParent(void)
{
	return m_pParent;
}

bool MyOctant::IsLeaf(void) 
{
	// a leaf has no children
	return (0 == m_uChildren);
}

bool MyOctant::ContainsMoreThan(uint a_nEntities) 
{
	uint collideCount = 0;
	uint totalEntityCount = m_pEntityMngr->GetEntityCount();
	for (uint i = 0; i < totalEntityCount; i++)
	{
		if (IsColliding(i))
			collideCount++;

		if (collideCount > a_nEntities)
			return true;
	}
	return false;
}

void MyOctant::KillBranches(void) 
{
	// go through any existing children
	for (int i = 0; i < m_uChildren; i++) 
	{
		// kill any branches on child branches
		m_pChild[i]->KillBranches();

		// delete the child
		delete m_pChild[i];
		m_pChild[i] = nullptr;
	}

	// set children to 0;
	m_uChildren = 0;
}

void MyOctant::ConstructTree(uint a_nMaxLevel)
{
	// make sure it's the root
	if (m_uLevel != 0)
		return;

	// set the new max level
	m_uMaxLevel = a_nMaxLevel;
	m_uOctantCount = 1;
	m_EntityList.clear();

	// clear the tree
	KillBranches();
	m_lChild.clear();

	// subdivide to the new max level
	if(ContainsMoreThan(m_uIdealEntityCount))
		Subdivide();

	AssignIDtoEntity();
	ConstructList();
}

void MyOctant::AssignIDtoEntity(void) 
{
	// get to the leaf node
	for (int i = 0; i < m_uChildren; i++)
		m_pChild[i]->AssignIDtoEntity();

	if (IsLeaf()) 
	{
		for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
		{
			if (IsColliding(i)) 
			{
				m_EntityList.push_back(i);
				m_pEntityMngr->AddDimension(i, m_uID);
			}
		}
	}
}

uint MyOctant::GetOctantCount(void)
{
	return m_uOctantCount;
}

void MyOctant::Release(void)
{
	// if it's the root, just call kill branches
	if (m_uLevel == 0)
		KillBranches();

	m_fSize = 0.0f;
	m_uChildren = 0;
	m_EntityList.clear();
	m_lChild.clear();
}

void MyOctant::Init(void)
{
	m_uID = m_uOctantCount;
	m_uLevel = 0;
	m_uChildren = 0;

	m_fSize = 0.0f;

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_pParent = nullptr;
	for (int i = 0; i < 8; i++)
		m_pChild[i] = nullptr;

	m_pRoot = nullptr;
}

void MyOctant::ConstructList(void) 
{
	// get to leaf node
	for (int i = 0; i < m_uChildren; i++)
		m_pChild[i]->ConstructList();

	// if there is nothing in the list
	if (m_EntityList.size() > 0)
		m_pRoot->m_lChild.push_back(this);
}

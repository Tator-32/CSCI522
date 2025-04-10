
#include "SceneNode.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "Skeleton.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/DefaultAnimationSM.h"
#include "PrimeEngine/Scene/SkeletonInstance.h"
#include "PrimeEngine/Scene/DebugRenderer.h"

namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(SceneNode, Component);

SceneNode *SceneNode::s_pRootSceneNode = 0;

// Constructor -------------------------------------------------------------
SceneNode::SceneNode(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) :  Component(context, arena, hMyself), m_lights(context, arena, 8), m_inheritPositionOnly(false)
{
}

void SceneNode::addDefaultComponents()
{
	Component::addDefaultComponents();
	
	
	// Data components

	// event handlers
	PE_REGISTER_EVENT_HANDLER(Events::Event_MOVE, SceneNode::do_MOVE);
	PE_REGISTER_EVENT_HANDLER(Events::Event_CALCULATE_TRANSFORMATIONS, SceneNode::do_CALCULATE_TRANSFORMATIONS);
	PE_REGISTER_EVENT_HANDLER(Events::Event_PRE_RENDER_needsRC, SceneNode::do_PRE_RENDER_needsRC);

}

void SceneNode::do_CALCULATE_TRANSFORMATIONS(Events::Event *pEvt)
{
	// PEINFO("SceneNode::do_CALCULATE_TRANSFORMATIONS\n");
    Handle hParentSN = Component::getFirstParentByType<SceneNode>();
	if (hParentSN.isValid())
	{
		Matrix4x4 tmp = hParentSN.getObject<SceneNode>()->m_worldTransform;
		if (m_inheritPositionOnly)
		{
			Vector3 pos = tmp.getPos();
			tmp.loadIdentity();
			tmp.setPos(pos);
		}
		m_worldTransform = tmp * m_base;
	}
	else
	{
		m_worldTransform = m_base;
	}

}

void SceneNode::do_MOVE(Events::Event *pEvt)
{
	Events::Event_MOVE *pRealEvent = (Events::Event_MOVE *)(pEvt);
	Matrix4x4 &m = m_base;
	m.setPos(m.getPos() + pRealEvent->m_dir);
}

void SceneNode::do_PRE_RENDER_needsRC(Events::Event* pEvt)
{

	//MeshInstance *pMeshInstance = getFirstComponent<MeshInstance>();
	//if (pMeshInstance && pMeshInstance->m_hAsset.isValid())
	//{
	//	Array<PrimitiveTypes::Float32> aabbv = pMeshInstance->m_hAsset.getObject<Mesh>()->m_hPositionBufferCPU.getObject<PositionBufferCPU>()->m_boundingBox;
	//	if (aabbv.m_size == 6) // if we have bounding box
	//	{

	//		DebugRenderer::Instance()->createBoundingBox(m_worldTransform, Vector3(aabbv[0], aabbv[2], aabbv[4]), Vector3(aabbv[1], aabbv[3], aabbv[5]), Vector3(0.2f, 0.8f, 0.6f), 0);
	//	
	//	}

	//}
}


PE_IMPLEMENT_CLASS1(JointSceneNode, SceneNode);

void JointSceneNode::do_CALCULATE_TRANSFORMATIONS(Events::Event *pEvt)
{
	Handle hSkelInst = this->getFirstParentByType<SkeletonInstance>();
	SkeletonInstance *pSkelInst = hSkelInst.getObject<SkeletonInstance>();
	
	DefaultAnimationSM *pAnimSM = pSkelInst->getFirstComponent<DefaultAnimationSM>();
	Matrix4x4 jntTransform;

	if (pAnimSM->m_modelSpacePalette.m_size)
	{
		jntTransform = pAnimSM->m_modelSpacePalette[m_myJoint];
	}

	Matrix4x4 skinTransform = pSkelInst->getFirstParentByType<SceneNode>().getObject<SceneNode>()->m_worldTransform;
 	m_worldTransform = (skinTransform * jntTransform) * m_base;
}

}; // namespace Components
}; // namespace PE

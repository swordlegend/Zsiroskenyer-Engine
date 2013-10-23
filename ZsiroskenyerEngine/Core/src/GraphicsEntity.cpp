////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Entity.cpp
//	2013.oct.09, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "GraphicsEntity.h" for more information.
////////////////////////////////////////////////////////////////////////////////

#include "GraphicsEntity.h"
#include "../../Core/src/common.h"

cGraphicsEntity::cGraphicsEntity() 
: 		position(0,0,0),
		rotation(Vec3(0,0,0), 0),
		scale(1,1,1),
		isVisible(true),
		instanceGroup(NULL)
{
}

cInstanceGroup* cGraphicsEntity::GetInstanceGroup() {
	return instanceGroup;
}

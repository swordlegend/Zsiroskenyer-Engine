////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Entity.h
//	2013.oct.09, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A class that represents an entity on the 3D scene.
////////////////////////////////////////////////////////////////////////////////


#include <math/math_all.h>


class cInstanceGroup;



class Entity {
public:
	Vec3 position;
	Quat rotation;
	Vec3 scale;

	cInstanceGroup* instanceGroup;
};
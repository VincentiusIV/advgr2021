#pragma once

class Transform
{
public:
	Transform() : position( 0.0, 0.0, 0.0 ), rotation( 0.0, 0.0, 0.0 ), scale( 1.0, 1.0, 1.0 ){};
	Transform( Point3 position ) : position( position ), rotation( 0.0, 0.0, 0.0 ), scale( 1.0, 1.0, 1.0 ){};
	Transform( Point3 position, vec3 rotation ) : position( position ), rotation( rotation ), scale( 1.0, 1.0, 1.0 ){};

	Point3 position, rotation, scale;
	mat4 localToWorldMatrix() { return (mat4::trs(position, rotation, scale)); }
};
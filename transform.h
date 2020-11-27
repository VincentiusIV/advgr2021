#pragma once

class Transform
{
public:
	Transform() : Transform( vec3( 0.0, 0.0, 0.0 ) ){};
	Transform( point3 position ) : Transform(position, vec3( 0.0, 0.0, 0.0 )){};
	Transform( point3 position, vec3 rotation ) : position( position ), rotation( rotation ), scale( 1.0, 1.0, 1.0 ) { UpdateTRS(); };
	virtual void Translate( vec3 delta )
	{
		position += delta;
		UpdateTRS();
	}
	void UpdateTRS() { localToWorldMatrix = mat4::trs( position, rotation, scale ); }
	point3 position, rotation, scale;
	mat4 localToWorldMatrix;
};
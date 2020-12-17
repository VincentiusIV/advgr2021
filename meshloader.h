#pragma once

class MeshLoader
{
  public:
	static vector<shared_ptr<MeshObject>> Load( const std::string filePath );
};

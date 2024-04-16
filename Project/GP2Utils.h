#pragma once
struct Vertex3D;

static bool ParseOBJ(const std::string& filename, std::vector<Vertex3D>& positions, std::vector<uint16_t>& indices)
{
	std::ifstream file(filename);
	if (!file)
		return false;

	std::string sCommand;
	// start a while iteration ending when the end of file is reached (ios::eof)
	while (!file.eof())
	{
		//read the first word of the string, use the >> operator (istream::operator>>) 
		file >> sCommand;
		//use conditional statements to process the different commands	
		if (sCommand == "#")
		{
			// Ignore Comment
		}
		else if (sCommand == "v")
		{
			//Vertex
			float x, y, z;
			file >> x >> y >> z;
			Vertex3D vertexOutput{};
			glm::vec3 vertexPos{};
			vertexOutput.pos = { x,y,z };
			vertexOutput.color = { 1,1,1 };
			positions.push_back(vertexOutput);
		}
		else if (sCommand == "f")
		{
			float i0, i1, i2;
			file >> i0 >> i1 >> i2;

			indices.push_back((uint16_t)i0 - 1);
			indices.push_back((uint16_t)i1 - 1);
			indices.push_back((uint16_t)i2 - 1);
		}
		//read till end of line and ignore all remaining chars
		file.ignore(1000, '\n');

		if (file.eof())
			break;
	}

	////Precompute normals
	//for (uint64_t index = 0; index < indices.size(); index += 3)
	//{
	//	uint32_t i0 = indices[index];
	//	uint32_t i1 = indices[index + 1];
	//	uint32_t i2 = indices[index + 2];

	//	Vector3 edgeV0V1 = positions[i1] - positions[i0];
	//	Vector3 edgeV0V2 = positions[i2] - positions[i0];
	//	Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

	//	if (isnan(normal.x))
	//	{
	//		int k = 0;
	//	}

	//	normal.Normalize();
	//	if (isnan(normal.x))
	//	{
	//		int k = 0;
	//	}

	//	normals.push_back(normal);
	//}

	return true;
}

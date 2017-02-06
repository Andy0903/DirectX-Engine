#include <iostream>
#include <fstream>

typedef	struct { float x, y, z; } VertexType;

typedef struct
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
} FaceType;

void GetModelFileName(char* aFileName)
{
	bool done = false;
	std::ifstream fileIn;

	//Loop until we have a file name.
	while (!done)
	{
		std::cout << "Enter model filename: ";
		std::cin >> aFileName;

		fileIn.open(aFileName);

		if (fileIn.good())
		{
			done = true;
		}
		else
		{
			fileIn.clear();
			std::cout << std::endl;
			std::cout << "File " << aFileName << " could not be opened." << std::endl << std::endl;
		}
	}
}

bool ReadFileCounts(char* aFileName, int& aVertexCount, int& aTextureCount, int& aNormalCount, int& aFaceCount)
{
	std::ifstream fileIn;
	char input;
	aVertexCount = 0;
	aTextureCount = 0;
	aNormalCount = 0;
	aFaceCount = 0;

	fileIn.open(aFileName);
	if (fileIn.fail()) { return false; }

	fileIn.get(input);
	while (!fileIn.eof())
	{
		// If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
		if (input == 'v')
		{
			fileIn.get(input);
			if (input == ' ') { aVertexCount++; }
			if (input == 't') { aTextureCount++; }
			if (input == 'n') { aNormalCount++; }
		}

		// If the line starts with 'f' then increment the face count.
		if (input == 'f')
		{
			fileIn.get(input);
			if (input == ' ') { aFaceCount++; }
		}

		// Otherwise read in the remainder of the line.
		while (input != '\n')
		{
			fileIn.get(input);
		}

		// Start reading the beginning of the next line.
		fileIn.get(input);
	}

	fileIn.close();
	return true;
}

bool LoadDataStructures(char* aFileName, int aVertexCount, int aTextureCount, int aNormalCount, int aFaceCount)
{
	VertexType *vertices, *texcoords, *normals;
	FaceType *faces;
	std::ifstream fileIn;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	std::ofstream fileOut;

	vertices = new VertexType[aVertexCount];
	texcoords = new VertexType[aTextureCount];
	normals = new VertexType[aNormalCount];
	faces = new FaceType[aFaceCount];

	vertexIndex = texcoordIndex = normalIndex = faceIndex = 0;

	fileIn.open(aFileName);
	if (fileIn.fail()) { return false; }

	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fileIn.get(input);
	while (!fileIn.eof())
	{
		if (input == 'v')
		{
			fileIn.get(input);

			if (input == ' ')
			{
				fileIn >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;
				// Invert the Z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

			if (input == 't')
			{
				fileIn >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;
				// Invert the V texture coordinates to left hand system.
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}

			if (input == 'n')
			{
				fileIn >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;
				// Invert the Z normal to change to left hand system.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++;
			}
		}

		if (input == 'f')
		{
			fileIn.get(input);
			if (input == ' ')
			{
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				fileIn >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		// Read in the remainder of the line.
		while (input != '\n')
		{
			fileIn.get(input);
		}

		// Start reading the beginning of the next line.
		fileIn.get(input);
	}

	fileIn.close();

	fileOut.open("model.txt");

	fileOut << "Vertex Count: " << (aFaceCount * 3) << std::endl;
	fileOut << std::endl;
	fileOut << "Data:" << std::endl;
	fileOut << std::endl;

	// loop through all the faces and output the three vertices for each face.
	for (int i = 0; i<faceIndex; i++)
	{
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		fileOut << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << std::endl;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		fileOut << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << std::endl;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		fileOut << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << std::endl;
	}

	fileOut.close();



	//Cleanup
	if (vertices)
	{
		delete[] vertices;
		vertices = nullptr;
	}
	if (texcoords)
	{
		delete[] texcoords;
		texcoords = nullptr;
	}
	if (normals)
	{
		delete[] normals;
		normals = nullptr;
	}
	if (faces)
	{
		delete[] faces;
		faces = nullptr;
	}

	return true;
}

int main()
{
	bool result;
	char filename[256];
	int vertexCount, textureCount, normalCount, faceCount;
	char garbage;

	GetModelFileName(filename);
	result = ReadFileCounts(filename, vertexCount, textureCount, normalCount, faceCount);
	if (result == false) { return -1; }

	std::cout << std::endl;
	std::cout << "Vertices: " << vertexCount << std::endl;
	std::cout << "UVs: " << textureCount << std::endl;
	std::cout << "Normals: " << normalCount << std::endl;
	std::cout << "Faces: " << faceCount << std::endl;

	result = LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount);
	if (result == false) { return -1; }

	std::cout << "\nFile has been converted." << std::endl;
	std::cout << "Press any key to exit." << std::endl;
	std::cin >> garbage;

	return 0;
}
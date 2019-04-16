#ifndef _MODEL_H_
#define _MODEL_H_

#include <map>
#include "mesh.h"
typedef std::map<std::string, Texture> LoadedTextMapType;
class Model
{
public:
	void draw(const Shader& shader)
	{
		for (std::vector<Mesh>::iterator it = this->meshes.begin(); this->meshes.end() != it; ++it)
		{
			it->draw(shader);
		}
	}
	bool loadModel(const std::string& filePath,GLfloat scale=1.0f)
	{
		Assimp::Importer importer;
		if (filePath.empty())
		{
			std::cerr << "Error:Model::loadModel, empty model file path." << std::endl;
			return false;
		}
		const aiScene* sceneObjPtr = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);
		
		if (!sceneObjPtr|| sceneObjPtr->mFlags == AI_SCENE_FLAGS_INCOMPLETE|| !sceneObjPtr->mRootNode)
		{
			std::cerr << "Error:Model::loadModel, description: " << importer.GetErrorString() << std::endl;
			return false;
		}
		this->modelFileDir = filePath.substr(0, filePath.find_last_of('/'));
		
		if (!this->processNode(sceneObjPtr->mRootNode, sceneObjPtr,scale))
		{
			std::cerr << "Error:Model::loadModel, process node failed."<< std::endl;
			return false;
		}
		return true;
	}
	~Model()
	{
		for (std::vector<Mesh>::const_iterator it = this->meshes.begin(); this->meshes.end() != it; ++it)
		{
			it->clear();
		}
	}
public:
	bool processNode(const aiNode* node, const aiScene* sceneObjPtr,GLfloat scale)
	{
		if (!node || !sceneObjPtr)
		{
			return false;
		}
		for (size_t i = 0; i < node->mNumMeshes; ++i)
		{
			const aiMesh* meshPtr = sceneObjPtr->mMeshes[node->mMeshes[i]]; 
			if (meshPtr)
			{
				Mesh meshObj;
				if (this->processMesh(meshPtr, sceneObjPtr, meshObj,scale))
				{
					this->meshes.push_back(meshObj);
				}
			}
		}
		for (size_t i = 0; i < node->mNumChildren; ++i)
		{
			this->processNode(node->mChildren[i], sceneObjPtr,scale);
		}
		return true;
	}
	bool processMesh(const aiMesh* meshPtr, const aiScene* sceneObjPtr, Mesh& meshObj,GLfloat scale)
	{
		if (!meshPtr || !sceneObjPtr)
		{
			return false;
		}
		std::vector<Vertex> vertData;
		std::vector<Texture> textures;
		std::vector<GLuint> indices;
		for (size_t i = 0; i < meshPtr->mNumVertices; ++i)
		{
			Vertex vertex;
			if (meshPtr->HasPositions())
			{
				vertex.position.x = meshPtr->mVertices[i].x/scale;
				vertex.position.y = meshPtr->mVertices[i].y/scale;
				vertex.position.z = meshPtr->mVertices[i].z/scale;
			}
			if (meshPtr->HasTextureCoords(0))
			{
				vertex.texCoords.x = meshPtr->mTextureCoords[0][i].x;
				vertex.texCoords.y = meshPtr->mTextureCoords[0][i].y;
			}
			else
			{
				vertex.texCoords = glm::vec2(0.0f, 0.0f);
			}
			if (meshPtr->HasNormals())
			{
				vertex.normal.x = meshPtr->mNormals[i].x/scale;
				vertex.normal.y = meshPtr->mNormals[i].y/scale;
				vertex.normal.z = meshPtr->mNormals[i].z/scale;
			}
			vertData.push_back(vertex);
		}
		for (size_t i = 0; i < meshPtr->mNumFaces; ++i)
		{
			aiFace face = meshPtr->mFaces[i];
			if (face.mNumIndices != 3)
			{
				std::cerr << "Error:Model::processMesh, mesh not transformed to triangle mesh." << std::endl;
				return false;
			}
			for (size_t j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}
		if (meshPtr->mMaterialIndex >= 0)
		{
			const aiMaterial* materialPtr = sceneObjPtr->mMaterials[meshPtr->mMaterialIndex];
			std::vector<Texture> diffuseTexture;
			this->processMaterial(materialPtr, sceneObjPtr, aiTextureType_DIFFUSE, diffuseTexture);
			textures.insert(textures.end(), diffuseTexture.begin(), diffuseTexture.end());
			std::vector<Texture> specularTexture;
			this->processMaterial(materialPtr, sceneObjPtr, aiTextureType_SPECULAR, specularTexture);
			textures.insert(textures.end(), specularTexture.begin(), specularTexture.end());
		}
		meshObj.setData(vertData, textures, indices);
		return true;
	}
	bool processMaterial(const aiMaterial* matPtr, const aiScene* sceneObjPtr,const aiTextureType textureType, std::vector<Texture>& textures)
	{
		textures.clear();

		if (!matPtr|| !sceneObjPtr )
		{
			return false;
		}
		if (matPtr->GetTextureCount(textureType) <= 0)
		{
			return true;
		}
		for (size_t i = 0; i < matPtr->GetTextureCount(textureType); ++i)
		{
			Texture text;
			aiString textPath;
			aiReturn retStatus = matPtr->GetTexture(textureType, i, &textPath);
			if (retStatus != aiReturn_SUCCESS 
				|| textPath.length == 0)
			{
				std::cerr << "Warning, load texture type=" << textureType<< "index= " << i << " failed with return value= "<< retStatus << std::endl;
				continue;
			}
			std::string absolutePath = this->modelFileDir + "/" + textPath.C_Str();
			LoadedTextMapType::const_iterator it = this->loadedTextureMap.find(absolutePath);
			if (it == this->loadedTextureMap.end())
			{
				GLuint textId = load2DTexture(absolutePath.c_str());
				text.id = textId;
				text.path = absolutePath;
				text.type = textureType;
				textures.push_back(text);
				loadedTextureMap[absolutePath] = text;
			}
			else
			{
				textures.push_back(it->second);
			}
		}
		return true;
	}
	static  GLuint load2DTexture(const char* filename, GLint internalFormat = GL_RGB, GLenum picFormat = GL_RGB, GLenum picType = GL_UNSIGNED_BYTE, int loadChannels = SOIL_LOAD_RGB, GLboolean alpha = false)
	{
		GLuint textureId = 0;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		GLubyte *imageData = NULL;
		int picWidth, picHeight;
		int channels = 0;
		imageData = SOIL_load_image(filename, &picWidth, &picHeight, &channels, loadChannels);
		if (imageData == NULL)
		{
			std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
			return 0;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, picWidth, picHeight, 0, picFormat, picType, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(imageData);
		glBindTexture(GL_TEXTURE_2D, 0);
		return textureId;
	}
	static GLuint loadSkyBoxTexture(std::vector<const char*> picFilePathVec,GLint internalFormat = GL_RGB, GLenum picFormat = GL_RGB,GLenum picDataType = GL_UNSIGNED_BYTE, int loadChannels = SOIL_LOAD_RGB)
	{
		GLuint textId;
		glGenTextures(1, &textId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textId);
		GLubyte *imageData = NULL;
		int picWidth, picHeight;
		for (std::vector<const char*>::size_type i = 0; i < picFilePathVec.size(); ++i)
		{
			int channels = 0;
			imageData = SOIL_load_image(picFilePathVec[i], &picWidth,
				&picHeight, &channels, loadChannels);
			if (imageData == NULL)
			{
				std::cerr << "Error::loadSkyBoxTexture could not load texture file:"
					<< picFilePathVec[i] << std::endl;
				return 0;
			}
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, picWidth, picHeight, 0, picFormat, picDataType, imageData);
			SOIL_free_image_data(imageData);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		return textId;
	}
public:
	std::vector<Mesh> meshes; 
	std::string modelFileDir; 
	LoadedTextMapType loadedTextureMap;
};

#endif
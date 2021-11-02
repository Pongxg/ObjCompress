#pragma once
#include "Singleton.h"
#include "Util.h"

#include <cinttypes>
#include <cstdlib>

#include "draco/compression/encode.h"
#include "draco/core/cycle_timer.h"
#include "draco/io/file_utils.h"
#include "draco/io/mesh_io.h"

class ContextManager :public Singleton<ContextManager>
{
public:
	ContextManager();
	~ContextManager();

	bool ParserCmdLine(int argc, char** argv);

	bool ParserConfig();

	void TraversePath();

	bool MakePathDir();

	bool EncodeFiles();

	bool EncodeObj(std::string filePath, std::string newPath);

	bool EncodePng(std::string filePath, std::string newPath);

private:
	int EncodeMeshToFile(const draco::Mesh& mesh, const std::string& file,draco::Encoder* encoder);

	StrMap m_mapFile;
	StrMap m_mapFileObj;
	StrMap m_mapFilePng;

	std::string		m_strConfigName;
	JSON_SETTING    m_tJosnSetting;
	
};

#define  gContextInstance ContextManager::GetInstance()
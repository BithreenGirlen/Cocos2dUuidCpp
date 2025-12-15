

#include <iostream>

#include "../cocos2d_uuid.h"
#include "../cocos2d_config.h"

/* Intentionally insufficient for practical use. */

static std::string LoadFileAsString(const char* filePath)
{
	std::string strFile;

	FILE* pFile = nullptr;
	int iRet = -1;
	long lSize = -1;
	size_t nRead = 0;

	pFile = ::fopen(filePath, "rb");
	if (pFile == nullptr) return {};

	iRet = ::fseek(pFile, 0, SEEK_END);
	if (iRet == -1)goto end;

	lSize = ::ftell(pFile);
	if (lSize == -1)goto end;

	::rewind(pFile);
	strFile.resize(lSize);
	nRead = ::fread(&strFile[0], sizeof(char), lSize, pFile);

end:
	if (pFile != nullptr)
	{
		::fclose(pFile);
	}

	return strFile;
}

static std::string TypeToExtension(int iType, const std::unordered_map<int, std::string>& types)
{
	/* Define extension map for native files, or deduce from the corresponding import file. */
	static const std::unordered_map<std::string, std::string> extensionMap =
	{
		{"cc.AudioClip", ".mp3"},
		// ..
	};

	const auto& iter = types.find(iType);
	if (iter != types.cend())
	{
		const auto& iter2 = extensionMap.find(iter->second);
		if (iter2 != extensionMap.cend())
		{
			return iter2->second;
		}
	}

	return {};
}

static void Test(const char* filePath)
{
	const std::string strFile = LoadFileAsString(filePath);
	if (strFile.empty())return;

	ResourceInfo r;
	ReadConfig(strFile.c_str(), r);

	for (size_t i = 0; i < r.uuids.size(); ++i)
	{
		const auto& path = r.paths.find(i);
		if (path == r.paths.cend())continue;

		const auto& pathDatum = path->second;
		const auto& extension = TypeToExtension(pathDatum.ucFileTypes[0], r.types);
		if (extension == ".json")
		{
			/* Some files, like scenario files, are significant only in import versions. */
			const auto& importIter = r.importVersion.find(std::to_string(i));
			if (importIter != r.importVersion.cend())
			{
				/* Build path and do something of the interest. */
				std::cout << "path: " << pathDatum.strPath << ", uuid: " << DecodeUuid(r.uuids[i]) << ", version: " << importIter->second << std::endl;
			}

			continue;
		}

		const auto& nativeIter = r.nativeVersion.find(std::to_string(i));
		if (nativeIter != r.nativeVersion.cend())
		{
			/* Build path and do something of the interest. */
			std::cout << "path: " << pathDatum.strPath << ", uuid: " << DecodeUuid(r.uuids[i]) << ", version: " << nativeIter->second << std::endl;
		}
		else
		{
			/* Might be found in packs. */
			long long nIndex = SearchPackIndices(i, r.packs);
			if (nIndex == -1)continue;

			const auto& importIter = r.importVersion.find(r.packs[nIndex].strPackName);
			if (importIter != r.importVersion.cend())
			{
				std::cout << "path: " << pathDatum.strPath << ", pack name: " << r.packs[nIndex].strPackName << std::endl;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		::setlocale(LC_ALL, ".utf8");
		/* In real case, config file should be traced from index page.
		* (index) => ( application ) => ( setting ) => ( config )
		*/
		Test(argv[1]);
	}
}
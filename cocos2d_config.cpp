

#include "json_minimal.h"
#include "cocos2d_config.h"

static long ToLong(const char* src, size_t length)
{
	char sBuffer[32]{};
	if (length > sizeof(sBuffer) - 1)return 0;
	memcpy(sBuffer, src, length);
	sBuffer[length] = '\0';

	return strtol(sBuffer, nullptr, 10);
}

static void ReadPath(const char* src, std::unordered_map<size_t, PathData>& pathMap)
{
	const char* p = src;
	const char* pStart = nullptr, * pEnd = nullptr;
	bool bRet = json_minimal::FindNextObject(&src, "paths", &pStart, &pEnd);
	if (!bRet)return;

	p = pStart;
	for (;;)
	{
		PathData path;

		const char* pNameStart = nullptr, * pNameEnd = nullptr;
		const char* pValueStart = nullptr, * pValueEnd = nullptr;
		bRet = json_minimal::util::ReadNextKeyInObject(&p, &pNameStart, &pNameEnd, &pValueStart, &pValueEnd);
		if (!bRet)break;

		int iIndex = ToLong(pNameStart, pNameEnd - pNameStart);

		const char* p2 = pValueStart;
		for (size_t i = 0;;++i)
		{
			bRet = json_minimal::util::ReadNextValueInArray(&p2, &pValueStart, &pValueEnd);
			if (!bRet)break;

			if (i == 0)
			{
				path.strPath.assign(pValueStart, pValueEnd);
			}
			else
			{
				int iType = ToLong(pValueStart, pValueEnd - pValueStart);
				path.ucFileTypes.push_back(iType);
			}
		}

		pathMap.insert({ iIndex, std::move(path) });
	}
}

static void ReadPack(const char* src, std::vector<PackData>& packs)
{
	const char* p = src;
	const char* pStart = nullptr, * pEnd = nullptr;
	bool bRet = json_minimal::FindNextObject(&src, "packs", &pStart, &pEnd);
	if (!bRet)return;

	p = pStart;
	for (;;)
	{
		PackData pack;

		const char* pNameStart = nullptr, * pNameEnd = nullptr;
		const char* pValueStart = nullptr, * pValueEnd = nullptr;
		bRet = json_minimal::util::ReadNextKeyInObject(&p, &pNameStart, &pNameEnd, &pValueStart, &pValueEnd);
		if (!bRet)break;

		pack.strPackName.assign(pNameStart, pNameEnd);

		const char* p2 = pValueStart;
		for (;;)
		{
			bRet = json_minimal::util::ReadNextValueInArray(&p2, &pValueStart, &pValueEnd);
			if (!bRet)break;

			int iType = ToLong(pValueStart, pValueEnd - pValueStart);
			pack.nIndices.push_back(iType);
		}

		packs.push_back(std::move(pack));
	}
}

static void ReadVersion(const char* src, std::unordered_map<std::string, std::string>& versionMap, bool bNative)
{
	const char* p = src;
	const char* pStart = nullptr, * pEnd = nullptr;
	bool bRet = json_minimal::FindNextObject(&src, "versions", &pStart, &pEnd);
	if (!bRet)return;

	p = pStart;
	const char* key = bNative ? "native" : "import";
	bRet = json_minimal::FindNextArray(&p, key, &pStart, &pEnd);
	if (!bRet)return;

	p = pStart;
	std::vector<std::string> temps;
	for (;;)
	{
		const char* pValueStart = nullptr, * pValueEnd = nullptr;
		bRet = json_minimal::util::ReadNextValueInArray(&p, &pValueStart, &pValueEnd);
		if (!bRet)break;

		temps.emplace_back(pValueStart, pValueEnd);
	}

	for (size_t i = 0; i < temps.size(); i += 2)
	{
		versionMap.insert({ std::move(temps[i]), std::move(temps[i + 1])});
	}
}

static void ReadUuid(const char* src, std::vector<std::string>& uuids)
{
	const char* p = src;
	const char* pStart = nullptr, * pEnd = nullptr;
	bool bRet = json_minimal::FindNextArray(&p, "uuids", &pStart, &pEnd);
	if (!bRet)return;

	p = pStart;
	for (;;)
	{
		const char* pValueStart = nullptr, * pValueEnd = nullptr;
		bool bRet = json_minimal::util::ReadNextValueInArray(&p, &pValueStart, &pValueEnd);
		if (!bRet)break;

		uuids.emplace_back(pValueStart, pValueEnd);
	}
}

static void ReadTypes(const char* src, std::unordered_map<int, std::string>& types)
{
	const char* p = src;
	const char* pStart = nullptr, * pEnd = nullptr;
	bool bRet = json_minimal::FindNextObject(&src, "types", &pStart, &pEnd);
	if (!bRet)return;

	p = pStart;
	for (int i = 0;;++i)
	{
		const char* pValueStart = nullptr, * pValueEnd = nullptr;
		bRet = json_minimal::util::ReadNextValueInArray(&p, &pValueStart, &pValueEnd);
		if (!bRet)break;

		types.insert({ i, std::string{pValueStart, pValueEnd} });
	}
}


void ReadConfig(const char* src, ResourceInfo& r)
{
	ReadUuid(src, r.uuids);
	ReadPath(src, r.paths);
	ReadVersion(src, r.importVersion, false);
	ReadVersion(src, r.nativeVersion, true);
	ReadPack(src, r.packs);
	ReadTypes(src, r.types);
}

void ReadSetting(const char* src, std::vector<BundleVer>& bundles)
{
	const char* p = src;
	const char* pStart = nullptr;
	const char* pEnd = nullptr;
	bool bRet = json_minimal::FindNextObject(&p, "bundleVers", &pStart, &pEnd);
	if (!bRet)return;

	p = pStart;
	for (;;)
	{
		const char* pNameStart = nullptr, *pNameEnd = nullptr;
		const char* pValueStart = nullptr, *pValueEnd = nullptr;
		bRet = json_minimal::util::ReadNextKeyInObject(&p, &pNameStart, &pNameEnd, &pValueStart, &pValueEnd);
		if (!bRet)break;

		bundles.emplace_back(BundleVer{ std::string(pNameStart, pNameEnd), std::string(pValueStart, pValueEnd) });
	}
}

long long SearchPackIndices(size_t nIndex, const std::vector<PackData>& packs)
{
	for (size_t i = 0; i < packs.size(); ++i)
	{
		const auto& pack = packs[i];
		const auto& iter = std::find(pack.nIndices.cbegin(), pack.nIndices.cend(), nIndex);
		if (iter != pack.nIndices.cend())
		{
			return i;
		}
	}

	return -1;
}
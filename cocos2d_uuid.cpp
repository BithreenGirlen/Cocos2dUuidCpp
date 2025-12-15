
#include "cocos2d_uuid.h"

static constexpr const char g_base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
static constexpr const char g_hexDigits[] = "0123456789abcdef";

/* UUID解号 */
std::string DecodeUuid(const std::string& src)
{
	size_t nPos = src.find('@');
	if (nPos == std::string::npos)nPos = 0;
	if (src.size() - nPos < 22)return {};

	const char* pSrc = &src[nPos];
	char sDecoded[31]{};
	char* pWritten = sDecoded;
	for (size_t i = 2; i < 22; i += 2)
	{
		const char* p1 = strchr(g_base64, pSrc[i]);
		const char* p2 = strchr(g_base64, pSrc[i + 1]);
		if (p1 == nullptr || p2 == nullptr)return {};

		size_t l = p1 - g_base64;
		size_t r = p2 - g_base64;

		*pWritten++ = g_hexDigits[l >> 2];
		*pWritten++ = g_hexDigits[((l & 3) << 2) | r >> 4];
		*pWritten++ = g_hexDigits[r & 0xF];
	}

	std::string strResult;
	strResult.append(pSrc, 2);
	strResult.append(&sDecoded[0], 6);
	strResult += '-';
	strResult.append(&sDecoded[6], 4);
	strResult += '-';
	strResult.append(&sDecoded[10], 4);
	strResult += '-';
	strResult.append(&sDecoded[14], 4);
	strResult += '-';
	strResult.append(&sDecoded[18]);

	return strResult;
}

/*UUID編号*/
std::string EncodeUuid(const std::string& src)
{
	size_t nPos = src.find('@');
	if (nPos == std::string::npos)nPos = 0;
	if (src.size() - nPos < 36)return {};

	char sEncoded[33]{};
	constexpr const size_t nDecodedSize = sizeof(sEncoded) - 1;
	char* pWritten = sEncoded;
	for (size_t nRead = nPos; (pWritten != &sEncoded[nDecodedSize]) && nRead < 36; ++nRead)
	{
		if (src[nRead] == '-')continue;
		*pWritten++ = src[nRead];
	}

	std::string strResult;
	strResult.resize(22);
	memcpy(&strResult[0], &src[nPos], 2);
	pWritten = &strResult[2];
	for (size_t i = 2; i < nDecodedSize; i += 3)
	{
		const char* p1 = strchr(g_hexDigits, sEncoded[i]);
		const char* p2 = strchr(g_hexDigits, sEncoded[i + 1]);
		const char* p3 = strchr(g_hexDigits, sEncoded[i + 2]);
		if (p1 == nullptr || p2 == nullptr || p3 == nullptr)return {};

		size_t l = p1 - g_hexDigits;
		size_t m = p2 - g_hexDigits;
		size_t r = p3 - g_hexDigits;

		*pWritten++ = g_base64[(l << 2) | (m >> 2)];
		*pWritten++ = g_base64[((m & 3) << 4) | r];
	}

	return strResult;
}

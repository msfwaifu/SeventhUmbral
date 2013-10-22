#include "ResourceSection.h"
#include "ShaderSection.h"
#include <vector>
#include <assert.h>

CResourceSection::CResourceSection()
{

}

CResourceSection::~CResourceSection()
{

}

void CResourceSection::Read(Framework::CStream& inputStream)
{
	CBaseSection::Read(inputStream);

	SEDBRES_HEADER header = {};
	inputStream.Read(&header, sizeof(SEDBRES_HEADER));

	std::vector<SEDBRES_ENTRY> entries;
	entries.resize(header.resourceCount);
	inputStream.Read(entries.data(), header.resourceCount * sizeof(SEDBRES_ENTRY));

	uint64 basePosition = inputStream.Tell();

	assert(header.resourceCount >= 2);

	const auto& resourceTypeEntry = entries[header.resourceCount - 2];
	const auto& resourceIdEntry = entries[header.resourceCount - 1];

	std::vector<uint32> resourceTypes;
	resourceTypes.resize(header.resourceCount);
	inputStream.Seek(basePosition + resourceTypeEntry.offset, Framework::STREAM_SEEK_SET);
	inputStream.Read(resourceTypes.data(), sizeof(uint32) * header.resourceCount);

	inputStream.Seek(basePosition + header.stringsOffset, Framework::STREAM_SEEK_SET);
	std::vector<std::string> strings;
	for(unsigned int i = 0; i < header.stringCount; i++)
	{
		strings.push_back(inputStream.ReadString());
	}

	for(unsigned int i = 0; i < header.resourceCount; i++)
	{
		const auto& entry = entries[i];
		uint32 resourceType = resourceTypes[i];
		inputStream.Seek(basePosition + entry.offset, Framework::STREAM_SEEK_SET);
		switch(resourceType)
		{
		case '\0trb':
			{
				auto section = new CResourceSection();
				section->Read(inputStream);
				delete section;
			}
			break;
		case 'sdrb':
			{
				auto section = new CShaderSection();
				section->Read(inputStream);
				delete section;
			}
			break;
		}
	}
}
//A class to parse PE files
#pragma once

#define WIN32_LEAN_AND_MEAN
//      WIN32_FAT_AND_STUPID

#include "Handle.h"
#include "Support.h"

#include <string>
#include <string_view>
#include <vector>

#include <windows.h>

struct PEThunkData
{
	IMAGE_THUNK_DATA			uThunkData;
	bool						bIsOrdinal;

	DWORD						Address;

	//bIsOrdinal
	int							Ordinal;

	//!bIsOrdinal
	std::string					Name;
	WORD						wWord;
};

struct PEImport
{
	IMAGE_IMPORT_DESCRIPTOR		uDesc;
	std::string					Name;

	std::vector<PEThunkData>	vecThunkData;
};

class PortableExecutable
{
private:
	std::string					Filename;

	// basic pe structure;
	IMAGE_DOS_HEADER			uDOSHeader;
	IMAGE_NT_HEADERS			uPEHeader;

	// sections
	std::vector<IMAGE_SECTION_HEADER>	vecPESections;

	// imports
	std::vector<PEImport>		vecImports;

	FileHandle Handle;

public:
	PortableExecutable(std::string_view filename) : Filename(filename) {
		if(!Filename.empty()) {
			Handle = FileHandle(_fsopen(Filename.c_str(), "rb", _SH_DENYNO));
		}

		if(!this->Handle || !this->ReadFile()) {
			throw_lasterror_or(ERROR_BAD_EXE_FORMAT, Filename);
		}
	};

	auto GetFilename() const noexcept {
		return Filename.c_str();
	}

	// pe
	auto const& GetDOSHeader() const noexcept {
		return uDOSHeader;
	}

	auto const& GetPEHeader() const noexcept {
		return uPEHeader;
	}

	// sections
	auto const& GetSections() const noexcept {
		return vecPESections;
	}

	auto const& GetImports() const noexcept {
		return vecImports;
	}

	// helpers
	auto const& GetImageBase() const noexcept {
		return this->GetPEHeader().OptionalHeader.ImageBase;
	}

	DWORD VirtualToRaw(DWORD dwAddress) const noexcept;

	bool ReadBytes(DWORD dwRawAddress, size_t Size, void* Dest) const noexcept;

	bool ReadCString(DWORD dwRawAddress, std::string& result) const;

	IMAGE_SECTION_HEADER const* FindSection(
		std::string_view name) const noexcept;

private:
	bool ReadFile();
};

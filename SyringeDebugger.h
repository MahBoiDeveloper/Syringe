#pragma once
#define WIN32_LEAN_AND_MEAN
//      WIN32_FAT_AND_STUPID

#include <windows.h>
#include "PortableExecutable.h"
#include <iostream>
#include <map>
#include <hash_map>

#define	MAX_NAME_LENGTH	0x100
#define EXE_NAME_LENGTH	0x100

#define INIT 0x00

const unsigned char INT3 = 0xCC;	//trap to debugger interupt opcode.
									//was some static const w/e shit, caused errors =S   -pd
const unsigned char NOP = 0x90;

class SyringeDebugger
{
public:
	SyringeDebugger();
	~SyringeDebugger();

	//Debugger
	bool DebugProcess(const char* exeFile,char* params);
	bool Run(char* params);
	DWORD HandleException(const DEBUG_EVENT& dbgEvent);

	//Breakpoints
	bool SetBP(void* address);
	void RemoveBP(void* address, bool restoreOpcode);

	//Memory
	LPVOID AllocMem(void* address,size_t size);
	bool PatchMem(void* address,void* buffer,DWORD size);
	bool ReadMem(void* address,void* buffer,DWORD size);

	//Syringe
	bool RetrieveInfo(const char*);
	void FindDLLs();

private:
	//Helper Functions
	DWORD RelativeOffset(DWORD from,DWORD to);

	//ThreadInfo
	struct threadInfo
	{
		HANDLE hThread;
		LPVOID lastBP;
	};
	typedef std::map<DWORD, threadInfo> ThreadMapType;
	ThreadMapType* threadInfoMap;

	//ProcessInfo
	PROCESS_INFORMATION pInfo;

	//Flags
	bool bAttached;
	bool bEntryBP;
	void* pEntryBP;

	//Breakpoints
	struct Hook
	{
		char lib[MAX_NAME_LENGTH];
		char proc[MAX_NAME_LENGTH];
		void* proc_address;

		int num_overridden;
		//BYTE*		p_caller_code;
	};
	struct BPInfo
	{
		BYTE original_opcode;
		std::vector<Hook> hooks;
		BYTE* p_caller_code; //used to delete later?
	};
	typedef stdext::hash_map<void*, BPInfo> BPMapType;
	BPMapType bpMap;

	std::vector<Hook*> v_AllHooks;
	std::vector<Hook*>::iterator loop_LoadLibrary;

	//Syringe
	char exe[EXE_NAME_LENGTH];
	void* pcEntryPoint;
	void* pImLoadLibrary;
	void* pImGetProcAddress;
	BYTE* pAlloc;
	DWORD dwTimeStamp;

	bool bControlLoaded;
	bool bDLLsLoaded;
	bool bHooksCreated;

	void* pLastBP;

	bool bAVLogged;

	DWORD time_start;
	DWORD time;
	int repeat;

	//data addresses
	BYTE* pdData;

	void* pdProcAddress;
	void* pdMessage;
	void* pdReturnEIP;
	void* pdRegisters;
	void* pdBuffer;

	void* pdLibName;
	void* pdProcName;

	//Code addresses
	BYTE* pcLoadLibrary;
	BYTE* pcLoadLibraryEnd;
};
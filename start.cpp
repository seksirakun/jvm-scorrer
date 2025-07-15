// JVMDumper.cpp - by veysel,rakun,batuzane
// dumps JNI/JVM/JVMTI exported function addresses from jvm.dll and hotspot.dll
// Outputs results to: C:\<programname>-Score\... with .cc extension
// Honorable Mentions (Helpers);
// - Axirus (sonoyuncu cheater)

#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

#pragma comment(lib, "psapi.lib")

std::vector<std::string> dumpedSymbols;
int totalSymbols = 0;
int foundSymbols = 0;

bool DumpFunctions(HMODULE module, const std::string& moduleName, std::ofstream& out) {
    if (!module) return false;

    BYTE* base = reinterpret_cast<BYTE*>(module);
    IMAGE_DOS_HEADER* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    IMAGE_NT_HEADERS* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);

    IMAGE_DATA_DIRECTORY dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    if (!dir.VirtualAddress) return false;

    IMAGE_EXPORT_DIRECTORY* exports = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(base + dir.VirtualAddress);
    DWORD* names = reinterpret_cast<DWORD*>(base + exports->AddressOfNames);
    DWORD* funcs = reinterpret_cast<DWORD*>(base + exports->AddressOfFunctions);
    WORD* ordinals = reinterpret_cast<WORD*>(base + exports->AddressOfNameOrdinals);

    for (size_t i = 0; i < exports->NumberOfNames; i++) {
        const char* name = reinterpret_cast<const char*>(base + names[i]);
        void* address = (void*)(base + funcs[ordinals[i]]);
        std::string sName(name);

        if (sName.find("JNI_") == 0 || sName.find("JVM_") == 0 || sName.find("jvmti") != std::string::npos) {
            out << "[+] " << name << " -> 0x" << std::hex << reinterpret_cast<uintptr_t>(address) << std::endl;
            foundSymbols++;
        } else {
            out << "[-] " << name << " (irrelevant symbol)" << std::endl;
        }
        totalSymbols++;
    }
    return true;
}

std::string GetExecutableName() {
    char path[MAX_PATH] = { 0 };
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    std::string full(path);
    size_t lastSlash = full.find_last_of("\\/");
    size_t dot = full.find_last_of(".");
    return full.substr(lastSlash + 1, dot - lastSlash - 1);
}

int main() {
    std::string exeName = GetExecutableName();
    std::string outDir = "C:\\" + exeName + "-Score";
    std::filesystem::create_directories(outDir);

    std::string outFile = outDir + "\\dump.cc";
    std::ofstream out(outFile);

    if (!out.is_open()) {
        std::cerr << "[-] Failed to open output file." << std::endl;
        return 1;
    }

    out << "// JVMDumper Report for " << exeName << std::endl;
    out << "=======================================\n";

    HMODULE hJvm = GetModuleHandleA("jvm.dll");
    HMODULE hHotspot = GetModuleHandleA("hotspot.dll");

    if (!hJvm) hJvm = LoadLibraryA("jvm.dll");
    if (!hHotspot) hHotspot = LoadLibraryA("hotspot.dll");

    bool anySuccess = false;
    if (hJvm) {
        out << "[MODULE] jvm.dll\n";
        if (DumpFunctions(hJvm, "jvm.dll", out)) anySuccess = true;
    }
    if (hHotspot) {
        out << "\n[MODULE] hotspot.dll\n";
        if (DumpFunctions(hHotspot, "hotspot.dll", out)) anySuccess = true;
    }

    if (!anySuccess) {
        out << "[-] Neither jvm.dll nor hotspot.dll loaded.\n";
        out.close();
        return 1;
    }

    float score = (float)foundSymbols / (float)totalSymbols * 10.0f;
    out << "\n[!] TOTAL SYMBOLS: " << totalSymbols << std::endl;
    out << "[!] FOUND JNI/JVM/JVMTI: " << foundSymbols << std::endl;
    out << "[!] JVM Compatibility Score: " << score << " / 10.0\n";

    out.close();
    std::cout << "[+] Dump completed: " << outFile << std::endl;
    return 0;
}
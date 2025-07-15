JVMDumper – JVM Function Scanner and Score Reporter

**JVMDumper** is a low-level diagnostic tool designed to examine and evaluate the exported functionality of the JVM runtime on Windows platforms. It analyzes the existence and accessibility of core JNI, JVM, and JVMTI functions by performing complete symbol enumeration and function address resolution on both `jvm.dll` and `hotspot.dll`.

This tool is designed to evaluate the visibility and binding ability of core Java native interfaces from a native process context without requiring a JVM agent or any JVM-based tools.

---

## Features

### Full JVM Symbol Enumeration

The tool dynamically loads the following libraries (if any):

* `jvm.dll`
* `hotspot.dll`

It enumerates exported symbols using Windows APIs such as `GetModuleHandle`, `GetProcAddress`, and `EnumProcessModules`.

---

### JNI/JVM/JVMTI Function Enumeration

All known native interface functions (e.g., `JNI_DefineClass`, `JVM_StartThread`, `JVM_GetSystemProperties`, etc.) are queried via `GetProcAddress`. Results are categorized as follows:

* **Resolved** (address retrieved successfully)
* **Not Resolved** (function not exported or missing)

Each result includes:

* Function name
* Address (hexadecimal) or status message

---
### JVM Compatibility Score

At the end of the scan, the tool calculates a compatibility score:

```
Detected: 34 / 30 functions successfully resolved
Score: 8.8 / 10.0
```

This score gives a rough indication of how open or restricted the JVM instance is regarding native function access.

---

## Output Format

Results are saved as a plain text report in the following location:

```
C:\<program_name>-Score\Dump.cc
```

The `.cc` file extension is used to indicate that the file contains C/C++-style symbol data, even if it does not contain source code.

Sample output snippet:

```
JNI_DefineClass .......... 0x6F1A23B0
JVM_StartThread .......... 0x6F203110
JVM_MonitorWait .......... <unresolved>
...
```

---

## Requirements

* Windows operating system
* A running Java installation (accessible via `jvm.dll` or `hotspot.dll`)
* Administrator privileges may be required in some environments

---

## Limitations

* Only exported symbols are scanned; non-exported internal JVM functions cannot be parsed.
* You cannot interact with JNI or the JVM if you are not using it.
* This tool does not connect to or interact with the JVM; it is passive and read-only.
* Results may vary between different Java versions and vendors (e.g., Oracle HotSpot, OpenJ9, GraalVM).

---

## Purpose

JVMDumper is designed for developers and analysts who want to learn about the native architecture of the Java Virtual Machine on Windows. It is particularly useful for reverse engineering, compatibility verification, and instrumentation feasibility assessments.

Used by veysel,axirus,batuzane,rakun,serhat,beyaz in Projects.

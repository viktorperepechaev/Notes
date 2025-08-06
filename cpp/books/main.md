Tree stages:
- Configuration
- Generation
- Building

## Configuration

During this stage CMake reads information from **source tree** directory and prepares a **build tree** directory for generation stage. **source tree** is usually the directory in which CMakeLists.txt is located.

Files:
- `CMakeLists.txt` - project configuration file
- `CMakeCache.txt` - file for storing somewhat stable variables (like paths to compilers). It saves time during the next configuration

## Generation

During this stage CMake generates a **buildsystem** (a set of files and directories for some build tool (for example, Make) that will actually create the needed binaries).
CMake still can apply some changes to the build configuration based on generator expressions (Just some syntax to specify something that is known only during generation).

## Building

During this stage CMake calls the appropriate build tool to produce **targets** (Simply things that we, as a developer, want to create).

A diagram:

```txt
+------------------------+                                           +-------+
|      Configure         |                                           | Build |
+------------------------+                                           +-------+
|  Read CMakeCache.txt   |                                           |Compile|
|          |             |          +---------------------+          |   |   |
|          v             |          |       Generate      |          |   v   |
|Parse and Execute CMake | -> -> -> +---------------------+ -> -> -> |  Link |
|      Language          |          |Generate Makefiles or|          |   |   |
|          |             |          |    project files    |          |   v   |
|          v             |          +---------------------+          |  Test |
|Write initial build tree|                                           |   |   |
|   and CMakeCache.txt   |                                           |   v   |
+------------------------+                                           |Package|
                                                                     +-------+
```

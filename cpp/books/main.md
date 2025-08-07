Three stages:
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

# Command line

CMake is a family of tools and consists of 5 executables:
- cmake - the main executable that configures, generates and builds projects
- ctest - this program is used to run and report test results
- cpack - the packaging program used to generate installers and source packages
- cmake-gui - the graphical wrapper around cmake
- ccmake - the console-based GUI wrapper around cmake

## cmake

It allows us to:
- Generate a project buildsystem
- Build a project
- Install a project
- Run a script
- Run a command-line tool
- Get help

### Generating a project buildsystem

The preferable syntax: `cmake [<options>] -S <path-to-source> -B <path-to-build>`
Ex: `cmake -S ./project -B ./build` - This command will a buildsystem in the `./build` directory (and create it if it's missing) from the source in the `./project` direcory.

You can see which generators (components of CMake that translate CMakeLists.txt into concrete command for the buildsystem, like Make) are available by looking at the end of the ouput of command: ```cmake --help```

### Options for caching

You can initialize a cache variable via `-D` flag. Ex: ```cmake -S . -B build -D CMAKE_BUILD_TYPE=Release```

### Options for debugging and tracing

Don't think they're that important. Might add later

### Builing a project

The syntax of the build mode: ```cmake --build <dir> [<options>] [-- <build-tool-options>]```
Usually: ```cmake --build <dir>``` does the job well. <dir> - is the argument that we passed to `-B` during the generation stage.

You can use multiple concurrent processes to speed-up building:
```cmake --build <dir> --parallel [<number-of-jobs>]```

### Options for targets

Every project is made up of one or more targets. You can specify which target to make with `--target`, `-t` flag. Ex: `cmake --build <dir> -t <target1> -t <target2>`
One target that isn't build usually is `clean` (It removes all artifacts from build directory). Ex: `cmake --build <dir> -t clean`.
You can also clean and then do a normal build: `cmake --build <dir> --clean-first`

### Options for debugging

We can ask CMake for more detailed logs with `--verbose`, `-v` flag. Ex: `cmake --build <dir> --verbose`.

### Installing a project

After you have built the artifacts you might want to separate them from all the files that CMake creats during its work process. You can do with the flag `--install`.
Ex: `cmake --install <dir> [options]` - this will execute the installation rules(which are usually just moving the artifacts to your special testing directory).
The installation rules are configured using the install() command in CMakeLists.txt.

### Running a script

CMake has its own programming language and we can simply use it without building any targets. The syntax: `cmake [{-D <var>=<value>}...] -P <cmake-script-file> [-- <unparsed-options>...]`
Ex: ```cmake -D MY_VAR="Hello" -P my_script.cmake -- aboba fast 123```

### Getting help

Syntax: ```cmake --help[-<topic>]```. Ex: ```cmake --help-command add_executable```

## ctest

It's basically a wrapper over cmake which standardizes running tests and getting feedbeck from solutions built with cmake.
To run tests for a built project you can call `ctest` in the build tree.

## cpack

It simply a packaging tool that creates distributable packages (.exe, .dmg, .deb, .rpm, .zip, .tar.gz) from the build project.

## cmake-gui

Self-explanatory

## ccmake

Self-explanatory

# Navigating the project files

Let's look at the files/directories that CMake uses to manage its projects.

## The source tree

This direcory contains all of the C++ source files and CMake project files. Its other name if project root.

Things to remember:
- You must provide a CMakeLists.txt file in the top direcotory of the source tree.
- It should be managed with a VCS (Version Control System), for example, git.
- The path to this directory is given with a `-S` flag.
- Don't hardcode absolute paths to the source tree in your CMake code - the user might store the project in a different place.

## The build tree

CMake uses this direcory to store everything that gets generated during the building proccess. Other names are: build root and binary tree.

Things to remember:
- Your binary files, executables, libraries, object files, archives will be created here.
- This directory is specific to the system, so don't add it to VCS.
- It's recommended to produce artifacts in a directory that's not inside the build tree - to keep things clean.
- The path to this directory is given with a `-B` flag.
- It's recommended to have an installation stage that puts the final artifacts in some separate directory so that you can remove all the temporary files.

## Listfiles

It's simply files that contain CMake language. One of them can be included into another using the include(), find_package(), add_subdirectory() commands.

Things to remember:
- These files usually (but it's not obligatory) have a .cmake extension.
- The most importnat listfile is CMakeLists.txt. It's the first file to be executed and must be in the root of the source tree.
- Variables about listfiles: 
    - ```CMAKE_CURRENT_LIST_DIR```
    - ```CMAKE_CURRENT_LIST_FILE```
    - ```CMAKE_PARENT_LIST_FILE```
    - ```CMAKE_CURRENT_LIST_LINE```

### CMakeLists.txt

Such files must contain at least2 lines:
- ```cmake_minimum_required(VERSION <x.xx>)```: sets an expected version of CMake
- ```project(<name> <OPTIONS>)```: names the project and specifies the options of its configuration

You can have more than one CMakeLists.txt.
Ex:
```
CMakeLists.txt
api/CMakeLists.txt
api/api.h
api/api.cpp
```
An example of CMakeLists.txt that brings everything together:
```
cmake_minimum_required(VERSION 3.20)
project(app)
message("Top Level CMakeLists.txt")
add_subdirectory(api)
```

### CMakeCache.txt

This file contains cache variables from listfiles when the configuration stage is run for the first time. It is located in the root of build tree.

Things to remember:
- You can change this file manually by using cmake flags.
- You can reset the project to default state by deleting this file.

### The Config-files for packages

Your project may depend on an external package. Packages that support CMake should provide a configuration file so CMake can understand how to use them.

Things to remember:
- "Config-files" contain information regarding how to use the library binaries, headers and helper tools.
- Use ```find_package()``` command to include packages.
- CMake files describing packages are named ```<PackageName>-config.cmake``` and ```<PackageName>Config.cmake```
- You can specify version of the package and CMake will search for ```<Config>Version.cmake```
- If a vendor doesn't provide a need config-file - use "Find-module". It's a set of rules on how to find some dependency on your machine. You write the "Find-module" yourself.
- CMake provides a package registry. It's like a local database that helps CMake with find previously installed libraries.

### cmake\_install.cmake, CTestTestfile.cmake, CPackConfig.cmake

Don't change these files manually. They are just for the internal work of cmake, ctest, cpack.

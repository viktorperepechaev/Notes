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

### CMakePresets.json and CMakeUserPresets.json

TODO...

### Ignoring files in Git

```
build_debug/
build_release/
build/

**/CMakeCache.txt
**/CMakeUserPresets.json
**/CTestTestfile.cmake
**/CPackConfig.cmake
**/cmake_install.cmake
**/install_manifest.txt
**/compile_commands.json
```

### Scripts

CMake offers a platform-agnostic programming language. As a result, you can write cross-platform scripts.
The script file can be as complex as you want, however it's recommended that you csll the ```cmake_minimum_required``` command at the beginning.
Ex:
```
cmake_minimum_required(VERSION 3.20.0)
message("Hello world")
file(WRITE Hello.txt "I am writing to a file")
```

### Utility modules

Modules are just files that contain cmake code and that add functionality to your CMake project.
Ex:
```
cmake_minimum_required(VERSION 3.20.0)
project(ModuleExample)
include(TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
    message("BIG_ENDIAN")
else()
    message("LITTLE_ENDIAN")
endif()
```

# The basics of CMake Language syntax

Reminder: to run a script do ```cmake -P script.cmake```

Everything in a CMake listfile is either a command invocation or a comment.

## Comments

There are two kinds of comments:
- single-line
- bracket (multiline), can be nested
Ex:
```txt
# comment1
message("Hello!") # comment2

#[=[
comment3
  #[[
   comment4 
  #]]
#]=]
```
Multiline commets start with an opening square bracket "[" + any number of equal signs "=" + another square bracket "[".
To close a bracket comment just reverse the square brackets.
You can prepend openning brackets with # to quickly uncomment something (We used this in the example above).
Ex:
```
##[=[ this is a single-line comment now
comment3 <- no longer commented
  #[[
   comment4  <- still commented
  #]]
#]=] this is a single-line comment now
```

## Command invocations

To execute a command, you must provide its name, followed by parentheses, in which you enclose a whitespace-separated list of command arguments.
Ex:
```txt
message("hello" world)
\-----/ \-----------/
   |          |
  name    arguments
```
Use snake_case in command names, even though it's not obligatory.

Command invocations in CMake are not expressions. You can't provide abother command as an argument to a called command, as *everything* between the parentheses is interpreted as an argument for that command.
Ex: ```set(MY_DIR get_filename_component(SOME_PATH DIRECTORY))``` will literally set the variable MY_DIR to the string "get_filename_component(SOME_PATH DIRECTORY)".

CMake commands don't require semicolons at the end of an invocation, because there're only 2 "types" of lines:
- ```command(argument1, argument2, ...) # comment```
- ```[[multiline comment]]```
Also putting a command after a bracket comment isn't allowed.

## Command arguments

The only data type recognized by CMake is a string.

CMake has 3 types of arguments:
- Bracket
- Quoted
- Unquoted
Each type offers a different level of evaluation and has a few small quirks to it.

### Bracket arguments

Bracket arguments aren't evaluated and can pass multiple strings as a single argument.
The syntax is very similar to comments, however you can't have nested bracket arguments.
Ex:
```
message([[Lol
  Aboba
  zombie
  ]] # Note: a newline will be printed because the closing tag is on the another line

message([==[
  this all
  is still one argument
  {"array" = [["mouse"], ["dog"]]}
  ]==])
```

### Quoted arguments

They allow for more processing (unlike bracket arguments which interpret everything "literally"), for example, expand sequences and variable references.
Ex:
```
message("1. escape sequrnce: \" in a quoted argument")
message("2. multi
line")
message("3. and a variable reference: ${CMAKE_VERSION}")
```

### Unquoted arguments

They evaluate escape sequences, variable references and allow to create a list of arguments instead of one string.
Note: ";" is treated as a delimitre, even though simple spaces are preferable
Ex:
```
   argum\ ent\;1     arg;ume nts
   \-----------/     \---------/
         |                |
  a signle argument three arguments
```
Some rules:
- Unquoted arguments cannot contain unescaped quotes `"`, hashes `#`, and backslashes `/`
- Parentheses: `()` are allowed only if they form correct, matching pairs
Exs:
```
message(a\ single\ argument)
message(two arguments)
message(three;separated;arguments)
message(${CMAKE_VERSION}) # a variable reference
message(()()()) # a matching parentheses
```

## Working with variables

Things to remember:
- Variable names are case-sensitive
- Variables are stored internally as strings
- To set a variable use `set()`
- To unset a variable use `unset()`
Ex:
```
set(MyString1 "Text1")
set([[My string2]] "Text2")
set("My string 3" "Text3")

message(${MyString1})
message(${My\ string2})
message(${My\ string\ 3})
```

## Variable references

To create reference to a defined variable use: `${MyVariable}`.

The process of evaluation:
1. CMake traverses the scope stack
2. Replaces `${MyVariable}` with a value or an empty string if a variable is not found
This process is called _variable evaluation_, _expansion_, _interpolation_.

The process is performed in an inside-out fashion, meaning:
1. If the following reference is encountered `${MyOuter${MyInner}}` - CMake will try to evaluate the inner first.
2. If the MyInner variable is successfully expanded, CMake will repeat the first step but this time for the inner.

Interesting example:
Let's suppose that:
- MyOuter holds _Hello_
- MyInner holds _${My_ 
If we call ```message("${MyOuter}Inner} World")``` the output will be: ```Helo World```. So the algorithm is **very** direct.

Another interesting example is: what happens if we call ```set(${MyInner} "Hi")``` instead of the normal ```set(MyInner "Hi")```?
Well, ```${MyInner}``` will expand and get replaces with _Hello_. As a result we will have a ```set(Hello "Hi")``` command which sets the value of the Hello variable.

We can do variable referencing in three ways:
- The ```${}``` syntax is used to reference _normal_ or _cache_ variables
- The ```$ENV{}``` is used to reference _environment_ variables
- The ```$CACHE{}``` is used to reference _cache_ variables

## Using the environment variables

CMake simply copies the environment variables and puts the copies into global scope.
You can ```set()``` and ```unset()``` them but the changes are seen only by the running cmake proccess.

To create or modify an environment variable use: ```set(ENV{<variable} <value>)``` (notice the _$_ is missing). Ex: ```set(ENV{CXX} "clang++")```
To clear an environment variable use: ```unset(ENV{<variable>})```

Another interesting thing to keep in mind is that CMake reads environment variables only once - during the configuration stage, after which CMake "bakes" them into the build tree
and changing them later won't have any effect on the building stage.

## Using the cache variables

Note: cache variables aren't available in _scripts_ - they only exist in _projects_.

Refer to cache variable with the syntax: ```$CACHE{<name>}```
Set a cache variable with the syntax: ```set(<variable> <value> CACHE <type> <docstring> [FORCE])```. ```CACHE``` and ```FORCE``` are keywords.

The ```CACHE``` says that we want to change something that was provided during the configuration stage, as a result we need to provide <type> and <docstring> so that
GUI can display these vasriables correctly (Why cache variables must be displayable? Well, so that ccmake or cmake-gui can show you them for you to easily remember :) ).
Possible types:
- ```BOOL```
- ```FILEPATH``` - path to a file
- ```PATH``` - path to a directory
- ```STRING``` - a line of text
- ```INTERNAL``` - a line of text, but GUI skips internal values (They won't be visible to the user). It implicitly adds ```FORCE``` keyword.

The <docstring> is simply a description of the variable, you must provide it even for the internal variables.

Just like enveronment variables, changing a cache variable affects only the current cmake execution, but if you set a variable that doesn't exist in cache or add ```FORCE``` keyword in that case
the value will persist.
Ex: ```set(FOO "BAR" CACHE string "some random value" FORCE)```
Note: if you set some cache variable and a normal variable with the same name exists then the normal variable will be removed.

## How to correctly use the variable scope in CMake

CMake has two scopes:
- Function scope - when we use ```function()```
- Directory scope - when we use ```add_subdirectory()```

When a nested scope is created, CMake simply fills it with copies of all the variables from the current scope. Subsequent command will affect the copies.
At the end of the nested scope execution the copies are deleted.

Ex:
1. The parent scope sets the `VAR` variable to ONE.
2. The nested scope prints `VAR`.
3. The nested scope sets `VAR` to TWO.
4. The nested scope prints `VAR`.
5. The nested scope ends and `VAR` is printed.
The output: ONE, TWO, ONE.

If you use ```unset()``` in the nested scope then only the copy is deleted.
Whenever we try to access the normal variabe (for exmaple, with ```${}``` syntax) CMake will:
1. Search for the variable in the current scope.
2. If CMake finds it - normal situation.
3. If CMake doesn't find it - CMake starts searching in cache.
4. At this point, only the _usual_ outcome is possibel.

You can change a variable in the parent scope by adding PARENT_SCOPE flag.
Exs:
- ```set(MyVariable "NewValue" PARENT_SCOPE)```
- ```unset(MyVariable PARENT_SCOPE)```

You can access only one level up.

## Using lists

CMake stores lists by concating all of the elements with `;` as a separator. Ex: ```a;list;of;5;elements```

You can create a list using ```set()``` command: ```set(myList a list of five elements)```.
Interestingly, the following commands will produce the same list (because of how lists are stored):
- set(myList "a;list;of;five;elements")
- set(myList a list "of;five;elements")

CMake unpacks lists into separate arguments.
Ex: ```message("the list is:" ${myList})```
The ```message()``` command will receive six arguments:
1. the list is:
2. a
3. list
4. of
5. five
6. elements

And so the output will be: ```the list is:alistoffiveelements```

You can do many things with a list, for example: ```list(SORT <list> [...])```

## Understanding control structures in CMake

CMake has three forms of conditional structures:
- Conditional blocks
- Loops
- Command definitions

They are executed in scripts and during the building process.

### Conditional blocks

The only conditional block in CMake is the ```if()``` command. All if-s must be closed with ```endif()```. You can add as many ```elseif()``` as you like.
```
if(<condition>)
    <commands>
elseif(<condition>)
    <commands>
else()
    <commands>
endif()
```

### The syntax for conditional commands

The same syntax is valid for ```if()```, ```elseif()``` and ```while()``` commands.

Logic operators:
- NOT <condition>
- <condition> AND <condition>
- <condition> OR <condition>

You can comdine conditions:
- (<condition>) AND (<condition> OR <condition>)

### The evaluation of a string and a variable

Another thing: strings are considered Boolean true only if they are exactly equal to one of these:
- ON
- Y
- YES
- TRUE
- A non-zero number

The process of evaluating unquoted variables is a bit tricky. When CMake encounters the following: ```if(VAR)``` it performs a one-time check (non-reqursive):
1. if VAR variable is undefined - ```if(VAR)``` turns into false.
2. if VAR variable is defined - CMake replaces VAR with its value
    1. If the value is exactly equal to _OFF_, _NO_, _FALSE_, _N_, _IGNORE_, _NOTFOUND_ - false
    2. If the value is a string ending with _-NOTFOUND_ - false
    3. If the value is an empty sctring - false
    4. If the value is _0_ - false
    5. true in other cases.

Ex1:
```
set(VAR1 FALSE)
set(VAR2 "VAR1")
if(${VAR2})
  message("True")
else()
  message("False")
endif()
```
output: False
Reason: firstly, ```${VAR2}``` is expanded to ```VAR1```, secondly VAR1 is expanded to ```FALSE```, thirdly CMake interprets ```FALSE``` as a false condition because its from the list above.

Ex2:
```
set(VAR3 "FALSE")
set(VAR2 VAR3)
set(VAR1 [[${VAR2}]])
if (VAR1)
  message("True")
else()
  message("False")
endif()
```
output: True
Reason: firstly, ```VAR1``` is expanded to ```${VAR2}```, secondly ```${VAR2}``` is interpreted as a true condition because the literal string ```${VAR2}``` doesn't fit in the list above.
Rememeber evaluation of unquoted variables is non-recursive!

We can explicitly check if a variable is defined or not:
- ```if(DEFINED <name>)```
- ```if(DEFINED CACHE{<name>})```
- ```if(DEFINED ENV{<name>})```

### Comparing values

For that we have following operators: _EQUAL_, _LESS_, _LESS_EQUAL_, _GREATER_, _GREATER_EQUAL_.
Ex: ```if (1 LESS 2)```
Note: CMake documentation specifies that if one of the operands is not a number, the value will be _false_. However, practical examples show that comparing with strings that start with numbers works somewhat ok: ```if (20 EQUAL "20 GB")``` -> _true_.

You can compare software versions by adding _VERSION__ prefix to any of the operators: ```if (1.3.4 VERSION_LESS_EQUAL 1.4)```
You can compare strings lexicographically by adding _STR_ prefix: ```if (A STREQUAL "${B}")```

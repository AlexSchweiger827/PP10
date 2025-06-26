# PP10

## Goal

In this exercise you will:

* Explore custom `struct` types and `typedef` in C.
* Link against existing system libraries (e.g., `-lm`).
* Create and evolve a custom C library from header-only to a precompiled static archive and install it system-wide.
* Install and use a third-party JSON library (`jansson`) via your package manager.
* Download, build, and install a GitHub-hosted library with a Makefile into standard include/lib paths.

**Important:** Start a stopwatch when you begin and work uninterruptedly for **90 minutes**. When time is up, stop immediately and record where you paused.

---

## Workflow

1. **Fork** this repository on GitHub.
2. **Clone** your fork locally.
3. Create a `solutions/` directory in the project root:

   ```bash
   mkdir solutions
   ```
4. For each task, add or modify source files under `solutions/`.
5. **Commit** and **push** your solutions.
6. **Submit** your GitHub repo link for review.

---

## Prerequisites

* GNU C compiler (`gcc`) and linker (`ld`).
* Make utility (`make`).
* `apt` or your distro’s package manager.

---

## Tasks

### Task 0: Exploring `typedef` and `struct`

**Objective:** Define and use a custom struct type with `typedef`.

1. Create `solutions/point.h` with:

   ```c
   typedef struct {
       double x;
       double y;
   } Point;
   ```
2. Create `solutions/point_main.c` that includes `point.h`, declares a `Point p = {3.0, 4.0}`, and prints its distance from origin using `sqrt(p.x*p.x + p.y*p.y)` (linking `-lm`).
   **
   
**Link for point.h**

[point.h ](https://github.com/AlexSchweiger827/PP10/blob/master/PP10/solutions/point.h)

**Link for main.c**
[point_main.c ](https://github.com/AlexSchweiger827/PP10/blob/master/PP10/solutions/point_main.c) 


#### Reflection Questions

1. **What does `typedef struct { ... } Point;` achieve compared to `struct Point { ... };`?**

   typedef struct { ... } Point declares a structure without a tag name.
   "typedef" creates an alias for this structure. In this case it is "Point".
   By inlcuding the structure in a program, you can declare variables of this structure type
   (e.g. "Point p={3.0 , 4.0};"). 
   
   
2. **How does the compiler lay out a `Point` in memory?**

The content of the structure is ordered sequentially. 
First comes double x then double y.
The data type double takes 8 byte of the memory.
That means the variable x needs the adress 0 to 7 in the memory.
The variable y takes the adress 8 to 15.

---

### Task 1: Linking the Math Library (`-lm`)

**Objective:** Compile and link a program against the math library.

1. In `solutions/`, compile `point_main.c` with:

   ```bash
   gcc -o solutions/point_main solutions/point_main.c -lm
   ```
2. Run `./solutions/point_main` and verify it prints `5.0`.
   
   **point_main.c compiled and point_main test run**
   
![point_main.c compiled](https://github.com/AlexSchweiger827/PP10/blob/master/PP10%20screenshots/Task%200%20compiled.PNG?raw=true) 

#### Reflection Questions

1. **Why is the `-lm` flag necessary to resolve `sqrt`?**
   
"-lm" instructs the linker to link against the math library. 
"sqrt" is a function stored in the math library (libm).

  
2. **What happens if you omit `-lm` when calling math functions?**

By not using the -lm flag in the compiling the linker would show an error like:

```
/usr/bin/ld: /tmp/ccXal4Vi.o: in function `main':
point_main.c:(.text+0x51): undefined reference to `sqrt'
collect2: error: ld returned 1 exit status

```
The linker can not find the right library (libm).

---

### Task 2: Header-Only Library

**Objective:** Create a simple header-only utility library.

1. Create `solutions/libutil.h` with an inline function:

   ```c
   #ifndef LIBUTIL_H
   #define LIBUTIL_H
   static inline int clamp(int v, int lo, int hi) {
       if (v < lo) return lo;
       if (v > hi) return hi;
       return v;
   }
   #endif
   ```
2. Create `solutions/util_main.c` that includes `libutil.h`, calls `clamp(15, 0, 10)` and prints `10`.
3. Compile and run:

   ```bash
   gcc -o solutions/util_main solutions/util_main.c
   ./solutions/util_main
   ```
**Link for libutil.h**

[libutil.h](https://github.com/AlexSchweiger827/PP10/blob/master/PP10/solutions/libutil.h) 

**Link for main.c**

[util_main.c ](https://github.com/AlexSchweiger827/PP10/blob/master/PP10/solutions/util_main.c) 
   
**util_main.c compiled and util_main test run**

![util_main.c compiled](https://github.com/AlexSchweiger827/PP10/blob/master/PP10%20screenshots/Task%202.PNG) 

#### Reflection Questions

1. **What are the advantages and drawbacks of a header-only library?**
   
**advantages:**

headerfiles only need to be inlcuded in the main programm and does not need a seperate compiling.
The linker does not need specific flags like "-lm" for compiling the program.
The compiler has full access to the source code of the library. Therefore it can better optimize the code through inlining of functions.
Header-only libraries are easier to use for different platforms and compilers. Because they do not rely on precompiled binaries.

**disavdvantage:**
The compilation time is increased, because with every inclusion of the header only library, the compiler has to re-compile the code in that header.
Parsing and holding the code of the libray can lead to an increased memory usage during compilation.
The compiler may generate multiple call sites of the same function or class, which leads to an increased binary size.
If the implementation of the library changes, all code that is dependent of the library needs to be recompiled.

2. **How does `static inline` affect linkage and code size?**


"static" gives the function an internel linkage. The functions name is only visible  within the current translation unit.
It prevents an overlap of the names if another file defines a function with the same name.

"inline" hints the compiler to consider replacing the function call with the content of the function at the point where the function is called. The compiler is free to ignore this. 

static inline will give the function external linkage. 
As result every file that includes the function will get its own seperate copy of the function.
Because of the individually copies of the function for every file, there will be no name conflicts, which allows a function definition in multiple files.
By small and frequently called functions can lead to smaller code size. 
If the function is large and the compiler chooses not to inline it, each translation unit will contain its own copy of the functions machine code.
This can lead to unenecessarily large and resource intensive code.

---

### Task 3: Precompiled Static Library

**Objective:** Convert the header-only utility into a compiled static library and link it manually.

1. Split `clamp` into `solutions/util.c` & `solutions/util.h` (remove `inline` and `static`).
2. Compile:

   ```bash
   gcc -c solutions/util.c -o solutions/util.o
   ```
3. Create the executable linking manually:

   ```bash
   gcc -o solutions/util_main_pc solutions/util.o solutions/util_main.c
   ```
4. Run `./solutions/util_main_pc` to verify output.

**Link for util.c**

[util.c ](https://github.com/AlexSchweiger827/PP10/blob/master/PP10/solutions/util.c) 

**Link for util.h**

[util.h ](https://github.com/AlexSchweiger827/PP10/blob/master/PP10/solutions/util.h) 

**Link for util.o (util.o is not readable in Github)** 

[util.o ](https://github.com/AlexSchweiger827/PP10/blob/master/PP10/solutions/util.o) 

**Link for util_main.c**

[util_main.c ](https://github.com/AlexSchweiger827/PP10/blob/master/PP10/solutions/util_main.c) 

**util_main.c compiled and util_main_pc test run**

![util_main.c compiled](https://github.com/AlexSchweiger827/PP10/blob/master/PP10%20screenshots/Task%203.PNG) 

#### Reflection Questions

1. **Why must you include `solutions/util.o` when linking instead of just the header?**
The header only tells the compiler how to call the function.
It does not provide the content of the function.
The linker needs the machine code of the function, which is in the object file util.o for a executable.

   
2. **What symbol resolution occurs at compile vs. link time?**
At compile the compiler generates from a source file a object files with the information or declaration of the symbol.

At link time the linker combines one or more object files into a executable program.
The linker finds the definitions for the undetermined external symbols. 

---

### Task 4: Packaging into `.a` and System Installation

**Objective:** Archive the static library and install it to system paths.

1. Create `libutil.a`:

   ```bash
   ar rcs libutil.a solutions/util.o
   ```
2. Move headers and archive:

   ```bash
   sudo cp solutions/util.h /usr/local/include/libutil.h
   sudo cp libutil.a /usr/local/lib/libutil.a
   sudo ldconfig
   ```
3. Compile a test program using system-installed lib:

   ```bash
   gcc -o solutions/util_sys solutions/util_main.c -lutil
   ```

   (assumes `#include <libutil.h>`)


**Link for libutil.a (libutil.a is not readable in Github)**

[libutil.a ](https://github.com/AlexSchweiger827/PP10/blob/master/PP10/libutil.a) 
   
**Link for libutil.h**

[libutil.h ](https://github.com/AlexSchweiger827/PP10/blob/master/PP10/solutions/libutil.h) 



#### Reflection Questions

1. **How does `ar` create an archive, and how does the linker find `-lutil`?**
"ar" is a command that is used for creating static libraries.
The syntax is: ar [Options] archive_name member_files
The "options" flag tells the command "ar" what actions should be executed.

for example:  ar rcs libutil.a solutions/util.o. 
The Option "rcs" says to replace or add util.o into the archive libutil.a ("r").
"c" tells "ar" to create an archive libutil.a if it does not exist.
The last option "s" tells ar to create a list with al symbols (functions, global variables) in the archive.

In the archive is a global header (!<arch>) to identify the files as an archive.
For each file to the archive, ar creates a file header, which contains metadata the name of the file, its size in bytes etc.


2. **What is the purpose of `ldconfig`?**
"ldconfig" creates links and cache to the most recent shared libraries.
It checks the header and the libraries and decides which versions should have their links updated.
In short the purpose of "ldconfig" is to creat, update and to manage the information for shared libraries.
---

### Task 5: Installing and Using `jansson`

**Objective:** Install a third-party JSON library and link against it.

1. Install via `apt`:

   ```bash
   sudo apt update && sudo apt install libjansson-dev
   ```
2. Create `solutions/json_main.c`:

   ```c
   #include <jansson.h>
   #include <stdio.h>
   int main(void) {
       json_t *root = json_pack("{s:i, s:s}", "id", 1, "name", "Alice");
       char *dump = json_dumps(root, 0);
       printf("%s\n", dump);
       free(dump);
       json_decref(root);
       return 0;
   }
   ```
3. Compile and run:

   ```bash
   gcc -o solutions/json_main solutions/json_main.c -ljansson
   ./solutions/json_main
   ```

**Link for json_main.c**

[json_main.c ](https://github.com/AlexSchweiger827/PP10/blob/master/PP10/solutions/json_main.c) 
   
**json_main.c compiled and json_main test run**

![json_main.c compiled](https://github.com/AlexSchweiger827/PP10/blob/master/PP10%20screenshots/Task%204.PNG) 

#### Reflection Questions

1. **What files does `libjansson-dev` install, and where?**
2. **How does the linker know where to find `-ljansson`?**

---

### Task 6: Building and Installing a GitHub Library

**Objective:** Download, build, and install a library from GitHub using its Makefile.

1. Choose a small C library on GitHub (e.g., `sesh/strbuf`).
2. Clone and build:

   ```bash
   git clone https://github.com/sesh/strbuf.git
   cd strbuf
   make
   ```
3. Install to system paths:

   ```bash
   sudo make install PREFIX=/usr/local
   sudo ldconfig
   ```
4. Write `solutions/strbuf_main.c` that includes `strbuf.h`, uses its API, and prints a test string.
5. Compile and link:

   ```bash
   gcc -o solutions/strbuf_main solutions/strbuf_main.c -lstrbuf
   ./solutions/strbuf_main
   ```

#### Reflection Questions

1. **What does `make install` do, and how does `PREFIX` affect installation paths?**
2. **How can you inspect a library’s exported symbols to verify installation?**

---
**Remember:** Stop after **90 minutes** and record where you stopped.

I finsihed task 0-4 and only finished the program for task 5. It took me 98 minutes.

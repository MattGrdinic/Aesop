# Aesop

Project Description: A front-end GUI to Facebook's HPHP compiler. The HPHP compiler generates highly-optimized C++ code from your PHP source code. It then wraps the result into self-contained executable files that can be run as a full web server. This project is an extension to this in that we've added a UI to the system to make compiling and managing the results much easier.

Requirements
Aesop and HPHP only runs on 64-bit Linux, Ubuntu 11.04 or Fedora 15 is preferred.

Current Version/Next Release
The current version is: .10b.

Downloads
We have two main downloads. The first (Aesop + HPHP) contains:

    The full Aesop UI Source
    The full, precompiled and ready to run HPHP binaries

This package should run on Ubuntu 10.10, 11.04, Fedora 15, and Mint 12 without changes, save for requiring the Qt 4 libraries and HPHP dependencies described in the blog post and docs.

The second (smaller) package is just the source release for Aesop, without the precompiled HPHP package. In theory, if we already have the Qt 4 libs and HPHP built on our machine we should be able to just place these files one level up from HPHP's main directory (that is, we create a new folder that holds both Aesop and HPHP). 

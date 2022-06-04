# nvc++: Compiler failed to translate accelerator region

Clone the repo:

```
git clone git@github.com:ugGit/nvhpc225-o3-issue-recreation.git
```

## Requirements
The issue has been encountered in an environment using the following modules, which must be available during the compilation:

* gcc/11.2.0
* nvhpc/22.5 (using CUDA 11.7)
* nvhpc/22.3 (using CUDA 11.6)

## Information about the setup
The code has been tested on an Nvidia A6000 and a GeForce RTX 2080.

This project contains the minimal required modules and code to recreate the issue encountered.

## Issue description
When trying to compile a c

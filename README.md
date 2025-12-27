# Lidar Viewer

### Description:

This is a C++ application that can visualize local LAS/LAZ file point-cloud data into 3D space as cubes. Then the user can view the mapped points with either an orbital or free camera. The color ramp, scale, and other various settings can be adjusted to preference.

---

### Developer Notes:

```bash

# CLONE REPOSITORY (WITH SUBMODULE)
git clone --recursive https://github.com/sethcg/Lidar-Viewer.git

# CONFIGURE
cmake --preset default

# BUILD RELEASE (INCREASED PERFORMANCE)
cmake --build --preset release

# BUILD DEBUG
cmake --build --preset debug

```

<details closed>
<summary><b>Line Count</b></summary>
<br/>

```bash

# GET LINE COUNT (REQUIRES CLOC TO BE INSTALLED)
cloc --include-lang=C++,"C/C++ Header",CMake --exclude-dir=build,vcpkg --out=line-count.txt .

```

| Language         | Files  | Blank | Comment | Code |
|:-----------------|:------:|:-----:|:-------:|:-----:|
| **C++**          | 11     | 276   | 69      | 961   |
| **C/C++ Header** | 14     | 222   | 58      | 791   |
| **CMake**        | 9      | 53    | 38      | 195   |
|                                                     |
| **Total**        | 34     | 551   | 165     | 1947  |

</details>

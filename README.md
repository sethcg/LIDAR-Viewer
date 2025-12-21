# Lidar Viewer

### Description:

This is a C++ application that can visualize local LAS/LAZ file point-cloud data into 3D space as cubes. Then the user can view the mapped points with either an orbital or free camera. The color ramp, scale, and other various settings can be adjusted to preference.

---

### Developer Notes:

```bash

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
| **C++**          | 11     | 278   | 70      | 970   |
| **C/C++ Header** | 14     | 223   | 58      | 792   |
| **CMake**        | 9      | 53    | 38      | 195   |
|                                                     |
| **Total**        | 34     | 554   | 92      | 1957  |

</details>


{
            "label": "Build Release",
            "type": "shell",
            "command": "cmake --build --preset release",
            "group": { "kind": "build", "isDefault": true },
            "dependsOn": "Configure"
        },
        {
            "label": "Build Debug",
            "type": "shell",
            "command": "cmake --build --preset debug",
            "group": { "kind": "build" },
            "dependsOn": "Configure"
        },
        {
            "label": "Configure",
            "type": "shell",
            "command": "cmake --preset default"
        }

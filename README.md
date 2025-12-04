# Point Cloud Viewer

### Description:

---

### Developer Notes:

```bash

# CONFIGURE
cmake --preset default

# BUILD
cmake --build --preset default

```

<details closed>
<summary><b>Line Count</b></summary>
<br/>

```bash

# GET LINE COUNT (REQUIRES CLOC TO BE INSTALLED)
cloc --include-lang=C++,"C/C++ Header",CMake --exclude-dir=build,vcpkg --out=line-count.txt .

```

| Language         | Files | Blank | Comment | Code |
|:-----------------|:-----:|:-----:|:-------:|:----:|
| **C++**          | 8     | 191   | 65      | 599  |
| **C/C++ Header** | 7     | 88    | 8       | 221  |
| **CMake**        | 3     | 18    | 19      | 78   |
|                                                   |
| **Total**        | 18    | 297   | 92      | 898  |

</details>
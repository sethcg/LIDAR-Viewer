# Lidar Viewer

<h2>Overview:</h2>

<dl>
  <dd>
    <h4>Description:</h4>
    <dl>
      <dd>
        This is a C++ application that can visualize local LAS/LAZ file point-cloud data into 3D space as cubes, colored by a normalized intensity value. The user can view the cubes using either a free/orbital camera. Color ramp, scale, and other settings can be adjusted to preference.
      </dd>
    </dl>
  </dd>
</dl>

<dl>
  <dd>
    <h4>Build Process:</h4>
    <dl>
      <dd>
        To build the program the application is using a combination of CMake and Vcpkg. There is the option to build for Release or Debug, and some optimizations added that significantly increase the performance of reading/filtering point-cloud data.
      </dd>
    </dl>
    <dl>
      <dd>
        Also, there is a ./plugin folder containing two obsolete PDAL plugins. These are no longer used because of performance issues.
      </dd>
    </dl>
  </dd>
</dl>

### Preview:


<dl>
  <dd>
  <details open>
    <summary>Orbital Camera</summary>
    <img width="800px" height="507px" src="https://github.com/sethcg/LIDAR-Viewer/blob/e547f3449c91423b641a48f5382c2babe3acff2d/_demo/laz-viewer-orbital_camera.gif" alt="image">
    <br>
  </details>
  
  <details closed>
    <summary>Free Camera</summary>
    <img width="800px" height="509px" src="https://github.com/sethcg/LIDAR-Viewer/blob/e547f3449c91423b641a48f5382c2babe3acff2d/_demo/laz-viewer-free_camera.gif" alt="image">
    <br>
  </details>
  </dd>
</dl>

### Tooling:

  - [x] Point Data Abstraction Library <a href="https://github.com/PDAL/PDAL">(PDAL)</a>
  - [x] SDL3
  - [x] OpenGL
  - [x] ImGui
  - [x] Vcpkg
  - [x] CMake

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

| Language         | Files  | Blank | Comment | Code   |
|:-----------------|:------:|:-----:|:-------:|:------:|
| **C++**          | 13     | 326   | 82      | 1113   |
| **C/C++ Header** | 17     | 250   | 63      | 857    |
| **CMake**        | 9      | 53    | 38      | 195    |
|                                                      |
| **Total**        | 39     | 629   | 183     | 2165   |

</details>

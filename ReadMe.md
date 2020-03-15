# Curve
* Author: Maksym Pylypenko
* Email: pylypenm@myumanitoba.ca

![Image](https://github.com/MaksymPylypenko/Curves/blob/master/curve.png)

## How to run?
1. Open a VS project **`opengl.sln`**
2. Press run (a new window should appear)

## Controls
* `Left click` - create a new point and extend the curve smoothly (if enough points)
* `Right click` - remove the last point and erase the curve smoothly 
* `R` - redraw the full curve
* `W`		- subdivide
* `SPACE`	- change the curve type

### Curve types
* `CatmullRom`
  - c1 continuity fully interpolating
  - local control for `i` and `i-1` curve segments
* `Bezier`
  - c0 continuity
  - requires 4 consequetive control points
* `B spline`
  - c2 continuity

## Notes
The program was successfully tested using:
* OS: Windows 10
* IDE: Visual Studio 2019 16.4
* Graphics card: Intel® HD Graphics 3000

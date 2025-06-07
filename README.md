# Godot 4 Ocean Addon

> [!NOTE]
> Requires Godot 4.2+

An early work in progress ocean addon for Godot 4 base on Jerry Tessendorf's
FFT method for generating the waves, using compute shaders to generate the
displacement map, and a quad tree based continuous distant dependant level of
detail (CDLOD) system to dynamically manage the polygon density without
popping or snapping between LOD levels.

The GLSL shaders that generates the displacement map were ported into Godot 4
from [this project](https://github.com/achalpandeyy/OceanFFT). The underlying
math is identical, but some things like binding points and how uniforms were
accessed had to be changed to work in Godots compute shader API.

![Ocean](https://user-images.githubusercontent.com/118585625/220286315-b2b4ccb5-1700-467c-aa2f-2b5983d9909a.png)

## Features
- FFT ocean wave simulation, wave energy derived from Unified Spectrum
- Integrated LOD for surface (CDLOD)
- Basic buoyancy system
- Basic underwater effect
- Basic whitecap rendering
- Surface shading:
  - Transparency
  - Reflections (provided by ReflectionProbe)
  - Refractions
  - Sub surface scattering

### Example Scene Controls
|  Button  |  Action  |
| --- | --- |
|  `/~  |  Toggle menu/free cam modes|
|  Mouse Motion  |  Free cam look|
|  Right Click  |  Free cam zoom|
|  W  |  Free cam forwards (locked to horizontal plane)|
|  S  |  Free cam backwards (locked to horizontal plane)|
|  A  |  Free cam strafe left (locked to horizontal plane)|
|  D  |  Free cam strafe right (locked to horizontal plane)|
|  Space  |  Free cam up (locked to vertical axis)|
|  Ctrl  |  Free cam down (locked to vertical axis)|
|  Shift  |  Free cam sprint/move faster|

## Todo List
- Improve visual rendering
  - Splash particles
  - Improve refractions
  - Improve underwater effect
- Lower detail but faster visual shader for lower LOD quads
- Optimize Quadtree3D at large depths
- Beach/cliff/rocks interactions
- Wave collision interactions (boat wakes, splashes, etc.)
- Multiplayer synchronization? [This reference](https://developer.download.nvidia.com/assets/gameworks/downloads/regular/events/cgdc15/CGDC2015_ocean_simulation_en.pdf) may be helpful for this.

![OceanCDLOD](https://user-images.githubusercontent.com/118585625/213395892-4c6e6b2e-82b5-4708-9689-6cbb93dad340.png)

![OceanUnderwater](https://user-images.githubusercontent.com/118585625/220287411-52c7df03-6197-430b-9990-5e16b1825f2d.png)

![OceanCurve](https://user-images.githubusercontent.com/118585625/213397029-6d327b02-a701-474d-b56a-da7b13a054ab.png)

## References
### Wave Generation Theory
- [Jerry Tessendorf - Simulating Ocean Water](https://people.computing.clemson.edu/~jtessen/reports/papers_files/coursenotes2004.pdf)
- [Fynn-Jorin Flügge - Realtime GPGPU FFT Ocean Water Simulation](https://tore.tuhh.de/bitstream/11420/1439/1/GPGPU_FFT_Ocean_Simulation.pdf)
- [Thomas Gamper - Ocean Surface Generation and Rendering](https://www.cg.tuwien.ac.at/research/publications/2018/GAMPER-2018-OSG/GAMPER-2018-OSG-thesis.pdf)
- [T. Elfouhaily, B. Chapron, K. Katsaros, D. Vandemark - A unified directional spectrum for long and short wind-driven waves](https://archimer.ifremer.fr/doc/00091/20226/17877.pdf)

### Water Implementation Examples
These are not exclusively limited to ocean wave implementations, and may
include other types of water simulations if they include visual rendering
techniques that are of value to reference.

- The implementation behind the wave height generation: [achalpandeyy/OceanFFT](https://github.com/achalpandeyy/OceanFFT)
- [Platinguin/Godot-Water-Shader-Prototype](https://github.com/Platinguin/Godot-Water-Shader-Prototype/)
- [godot-extended-libraries/hydro](https://github.com/godot-extended-libraries/hydro)
- [godot-extended-libraries/godot-realistic-water](https://github.com/godot-extended-libraries/godot-realistic-water)
- [Crest](https://github.com/wave-harmonic/crest)
- [Virtual Terrain Project](http://vterrain.org/)
- [jdupuy/whitecaps](https://github.com/jdupuy/whitecaps)
- [AlphaMistral/Mistral-Water](https://github.com/AlphaMistral/Mistral-Water)

### AAA-Game Implementation Examples
These generally don't give much in the way of code examples, but do give
a higher level overview of how the whole thing comes together.

- [Assassin’s Creed III: The tech behind (or beneath) the action](https://www.fxguide.com/fxfeatured/assassins-creed-iii-the-tech-behind-or-beneath-the-action/)
- [The technical art of Sea of Thieves](https://dl.acm.org/doi/10.1145/3214745.3214820)
- [Ocean simulation and rendering in War Thunder](https://developer.download.nvidia.com/assets/gameworks/downloads/regular/events/cgdc15/CGDC2015_ocean_simulation_en.pdf)
- [Wakes, Explosions And Lighting: Interactive Water Simulation in 'Atlas'](https://gpuopen.com/gdc-presentations/2019/gdc-2019-agtd6-interactive-water-simulation-in-atlas.pdf)

### Level Of Detail Implementation
- [Filip Struger - Continuous Distance-Dependent Level of Detail for Rendering Heightmaps (CDLOD)](https://github.com/fstrugar/CDLOD/blob/master/cdlod_paper_latest.pdf)
- [Claes Johanson - Real-time water rendering Introducing the projected grid concept](https://fileadmin.cs.lth.se/graphics/theses/projects/projgrid/projgrid-lq.pdf)
- [Willem H. de Boer - Fast Terrain Rendering Using Geometrical MipMapping](https://www.flipcode.com/archives/article_geomipmaps.pdf)
- [Frank Losasso, Hugues Hoppe - Geometry Clipmaps: Terrain Rendering Using Nested Regular Grids](https://hhoppe.com/geomclipmap.pdf)

### Foam/Whitecap Rendering
- [ARM Software - Using the Jacobian for modelling turbulent effects at wave crests](https://arm-software.github.io/opengl-es-sdk-for-android/ocean_f_f_t.html#oceanJacobian)
- [Mary Yingst, Jennifer R. Alford, Ian Parberry - Very Fast Real-Time Ocean Wave Foam Rendering Using Halftoning](https://ianparberry.com/techreports/LARC-2011-05.pdf)
- [Lining Chen, Yicheng Jin, Yong Yin - Ocean Wave Rendering with Whitecap in the Visual System of a Maritime Simulator](https://www.semanticscholar.org/paper/Ocean-Wave-Rendering-with-Whitecap-in-the-Visual-of-Chen-Jin/557fadde5e917ba8015a825963e359bc7903b464)
- [Jonathan Dupuy, Eric Bruneton - Real-time Animation and Rendering of Ocean Whitecaps](https://hal.inria.fr/hal-00967078/file/Whitecaps-presentation.pdf)

### Wave Collision Interaction
- [Jerry Tessendorf - eWave: Using an Exponential Solver on the iWave Problem](https://people.computing.clemson.edu/~jtessen/reports/papers_files/ewavealgorithm.pdf)
- [Jerry Tessendorf - Simulation of Interactive Surface Waves](https://people.computing.clemson.edu/~jtessen/reports/papers_files/SimInterSurfWaves.pdf)
- [Stefan Jeschke, Tomáš Skřivan, Matthias Müller-fischer, Nuttapong Chentanez, Miles Macklin, Chris Wojtan - Water Surface Wavelets](https://dl.acm.org/doi/pdf/10.1145/3197517.3201336)
- [Jos Stam - A Simple Fluid Solver based on the FFT](https://www.dgp.toronto.edu/public_user/stam/reality/Research/pdf/jgt01.pdf)

### Miscellaneous
- [Khan Academy - Computing a Jacobian matrix](https://www.khanacademy.org/math/multivariable-calculus/multivariable-derivatives/jacobian/v/computing-a-jacobian-matrix)
- [Jump Trajectory - Ocean waves simulation with Fast Fourier transform](https://www.youtube.com/watch?v=kGEqaX4Y4bQ)
- [Tiago Sousa - GPU Gems 2: Chapter 19. Generic Refraction Simulation](https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-19-generic-refraction-simulation)
- [Kenny Mitchell - GPU Gems 3: Chapter 13. Volumetric Light Scattering as a Post-Process](https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-13-volumetric-light-scattering-post-process)
- [Matt Pharr, Wenzel Jakob, Greg Humphreys - Physically Based Rendering: From Theory To Implementation: 8.4 Microfacet Models](https://www.pbr-book.org/3ed-2018/Reflection_Models/Microfacet_Models)
- [Eric Bruneton, Fabrice Neyret, Nicolas Holzschuch - Real-time Realistic Ocean Lighting using Seamless Transitions from Geometry to BRDF](https://hal.inria.fr/inria-00443630v3/document)

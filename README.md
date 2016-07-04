# GParticles: a generic, GPU-focused Particle Engine

Most implementations of particle systems process data on the CPU and transfer that data to the graphics hardware for rendering, a bottleneck that limits the maximum number of particles at around 10.000 instances. GParticles stores and processes all particle data directly on the GPU, eliminating the previous limitation, which allows the development of systems composed by millions of particles.

GParticles also provides a simple but powerful architechture that promotes reusability and modularity of functionality, with many utilities that let the user extend particle system behaviour and configurations at different abstraction levels.

Here are a few examples of what can be achieved with GParticles:
* Virus puddles

  ![](http://i.imgur.com/5uPBhE4.gif)

* Particle Gun

  ![](http://i.imgur.com/dKQiQl2.gif)

* Boids (**Coming Soon**)

Be sure to visit the GParticles wiki (<https://github.com/tiagoddinis/GParticles/wiki>), which contains a tutorial series, the xml project definition file documentation, and more information about the library architechture and its components.


## LICENSE
Copyright (c) 2016 Tiago Dinis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
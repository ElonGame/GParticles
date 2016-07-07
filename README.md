# GParticles: a generic, GPU-focused Particle Engine

A particle system is a collection of individual entities that have both a set of data and behavior rules. These entities, or particles, interact with each other and/or with the environment, forming a system that models a large-scale visual effect. Particle systems are a commonly employed method to reproduce "_fuzzy_" and natural phenomena, or chaotic systems composed by many individual entities (smoke, fire, water, herds, abstract effects, ...).  The main idea is that the viewer will only perceive the effect being modeled and not the particles themselves, since they all work as a cohesive unit.

Most implementations of particle systems process data on the CPU and transfer that data to the graphics hardware for rendering, a bottleneck that limits the maximum number of particles at around 10.000 instances. GParticles stores and processes all particle data directly on the GPU, eliminating the previous limitation, which allows the development of systems composed by millions of particles.

GParticles also provides a simple but powerful architecture that promotes reusability and modularity of functionality, with many utilities that let the user extend particle system behavior and configurations at different abstraction levels.

Be sure to visit the GParticles wiki (<https://github.com/tiagoddinis/GParticles/wiki>) to lear more. There you can find a tutorial series, the xml project definition file documentation and more information about the library architecture and its components.

Here are a few examples of what can be achieved with GParticles:
* Virus puddles

  ![](http://i.imgur.com/5uPBhE4.gif)

* Particle Gun

  ![](http://i.imgur.com/dKQiQl2.gif)

This project was an assignment for the Computer Engineering Lab class of my [Computer Engineering Master's Degree](https://www.di.uminho.pt/miei.html). I chose [Computer Graphics](http://www3.di.uminho.pt/uce-cg/>) as the project profile and developed GParticles with the guidance of [informatic department](https://www.di.uminho.pt/) assistant professor Atónio Ramires (<http://www.lighthouse3d.com/>, <https://github.com/Nau3D/nau>).


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
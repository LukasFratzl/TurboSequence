# Turbo Sequence Overview

[![License](https://img.shields.io/badge/License-MIT-blue)](#license)
[![GitHub release](https://img.shields.io/github/release/LukasFratzl/Turbo-Sequence?include_prereleases=&sort=semver&color=blue)](https://github.com/LukasFratzl/Turbo-Sequence/releases/)
[![stars - Turbo-Sequence](https://img.shields.io/github/stars/LukasFratzl/Turbo-Sequence?style=social)](https://github.com/LukasFratzl/Turbo-Sequence)
[![forks - Turbo-Sequence](https://img.shields.io/github/forks/LukasFratzl/Turbo-Sequence?style=social)](https://github.com/LukasFratzl/Turbo-Sequence)

Turbo Sequence is built to support a modern way to render Skeletal Meshes.
It's using GPU Instancing with Niagara to manage draw calls efficiently, which is mostly the bottleneck of traditional rendering systems.
Turbo Sequence is using bones to animate the meshes, which allow IK and layer mask blending per Bone.

![A Crowd](https://github.com/LukasFratzl/Turbo-Sequence/blob/main/TS_WikiResources/TurbosequenceOverview.gif)

## What is Turbo Sequence

Turbo Sequence is a Plugin for Unreal Engine 5 which is Open Source with an MIT License.
The advantage of using Turbo Sequence over VATs is that Turbo Sequence uses bone joint bending instead of pre-computed animations, which allows runtime bone joint bending like IK or Sockets.
Turbo Sequence is trying to use Draw-Calls per archetype efficiently and not per instance, which has an advantage compared to traditional rendering methods.

_*Traditional Rendering:*_
```
CPU       GPU
         |
         v
Instance 1 (Mesh data 1) -> Draw call
         |                      |
         v                      v
Instance 2 (Mesh data 2) -> Draw call
         |                      |
         v                      v
Instance N (Mesh data N) -> Draw call
```

_*GPU Instancing*_
```
CPU       GPU
         |
         v
Base mesh data -> Draw call
                  | (all instances combined)
                  v
                  Rendered instances
```

TS is optimized for crowds around 10k - 50k, if you need more units, use VATs, Turbo Sequence is built to combine Animation Quality with Modern Rendering which means it is just as fast as the Quality of Bone joint bending allows it.

## Nanite

In UE 5.5 Nanite Skeletal Meshes are here, before using Turbo Sequence, UEs Skeletal Mesh system for crowds can be a good option when using Nanite.


## Features

* Playing Animations
* Stop Animation
* Get IK Transform
* Set IK Transform
* Tweak Animation ( Speed, Weight, Start/End Transition )
* Most Possible Skeleton Rig Support without Bugs
* Blend Spaces
* Switch Material
* Switch Mesh
* Editor UI
* Windows Support
* Linux Support
* Dynamic Chunked Async Animation Loading
* Get Socket Transform
* Get Animation Curves
* Root Motion
* Animation Layer Masks Per Bone
* Optimization: Smart Vertex iterations on the Material
* Optimization: Update Groups
* Smooth, Flat Normals
* Level Of Details
* Multi-Mesh-Character Animations
* Blueprint Support
* Hybrid Animation Mode between the UE System and the TS System
* Add Instances
* Remove instances
* Per Instance Custom Data

## Limits
 * Blueprint-only projects are not supported.
 * A Blueprint API Exists, but I highly recommend using the C++ API because it's so much faster.
 * Only Windows and Linux are supported, Not IOS and Android or Mac.
 + Turbo Sequence is not built to handle metahumans, please use UE Nanite Skeletal Meshes instead.

## Quick Start Guide

1. Download Turbo Sequence
2. Install Turbo Sequence in your UE project in the Plugin Folder
3. Open the Feature Demo Map and press Play
4. Navigate to the Wiki to get started with the API

#### Download Turbo Sequence:

Navigate to the [Releases](https://github.com/LukasFratzl/Turbo-Sequence/releases) and download the source of your Unreal Engine Version

#### Install Turbo Sequence:

- Extract the TS Release on your Computer
- Copy the extracted Root folder to the Plugin folder of your **Games** UE project ( Not in the Engine ).
- Compile the Project and open it in Unreal Engine

#### Demo

Inside Unreal Engine, navigate to `..\Plugins\TurboSequence\Content\Demo` and play through the demos; there is no additional setup required.

## Docs

The official documentation and API can be found here:
<div align="center">

[![view - Documentation](https://img.shields.io/badge/view-Documentation-blue?style=for-the-badge)](https://github.com/LukasFratzl/Turbo-Sequence/blob/main/DOCS.md)

</div>

## Support and Responsibility

Turbo Sequence is a Hobby Project, nothing commercial behind the Repo, the Contributors are not responsible for solving bugs for users of Turbo Sequence, features may or may not come; if you really need a specific feature, please fork the Repo and build your own system on top of it; and optional create a pull request if you really think it should be part of the original Turbo Sequence Repo.

If you encounter a bug, create an issue and when I have the time, I try to respond to it.

## Contribution

Contributions are welcome; if you want to contribute, Fork the Repo, create an Exp Branch, and create a Pull Request from the Exp Branch.
Pull requests need to be reviewed to maintain the quality standards of this Repo in terms of runtime performance and the UE Coding Standards.

[![forks - Turbo-Sequence](https://img.shields.io/github/forks/LukasFratzl/Turbo-Sequence?style=social)](https://github.com/LukasFratzl/Turbo-Sequence)

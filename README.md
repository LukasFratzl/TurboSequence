# Turbo Sequence Overview

[![License](https://img.shields.io/badge/License-MIT-blue)](#license)
[![GitHub release](https://img.shields.io/github/release/LukasFratzl/Turbo-Sequence?include_prereleases=&sort=semver&color=blue)](https://github.com/LukasFratzl/Turbo-Sequence/releases/)
[![stars - Turbo-Sequence](https://img.shields.io/github/stars/LukasFratzl/Turbo-Sequence?style=social)](https://github.com/LukasFratzl/Turbo-Sequence)
[![forks - Turbo-Sequence](https://img.shields.io/github/forks/LukasFratzl/Turbo-Sequence?style=social)](https://github.com/LukasFratzl/Turbo-Sequence)

Turbo Sequence is a modern way to render Skeletal Meshes.

It's using GPU Instancing with Niagara to bypass draw calls which are mostly the bottleneck of traditional rendering systems.

Turbo Sequence is using bones to animate the meshes, which allow IK and Sockets and Layer Mask Blending per Bone.

![A Crowd](Docs/TurbosequenceOverview.gif)

## What is Turbo Sequence

Turbo Sequence is a Plugin for Unreal Engine 5 which is Open Source with the MIT License.

The advantage point from using VATs is the fact that Turbo Sequence is using bone joint bending instead of pre computed animations, which allows runtime bone joint bending.

Turbo Sequence is trying to bypass draw calls to one draw call per archetype which has an advantage point compared to traditional Skeletal Meshes.

TS is optimized for crowds around 10k - 50k, if you need more units, use VATs, Turbo Sequence is build for combining Animation Quality with Modern Rendering which mean it is just as fast as the Quality of Bone joint Bending allows it.

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
* Optimization: 16 or 32 bit Textures
* Optimization: Smart Vertex iterations on the Material
* Optimization: Update Groups
* Smooth, Flat Normals
* Level Of Details
* Multi-Mesh-Character Animations
* Blueprint Support
* Hybrid Animation Mode between UE System and TS System
* Add Instances
* Remove instances
* Per Instance Custom Data

## Limits
 * Blueprint only projects are not supported.
 * A Blueprint API Exists but I highly recommend using the C++ API because it's so much faster.
 * Only Windows and Linux is supported, Not IOS and Android and Mac.

## Quick Start Guide

Summary
1. Download a Release
2. Install Turbo Sequence in your UE project in the Plugin Folder
3. Open the Feature Demo Map and press Play
4. Navigate the the Wiki to get started with the API

#### Download a Release:

Navigate to the [Releases](https://github.com/LukasFratzl/Turbo-Sequence/releases) and download the source of your Unreal Engine Version

#### Install Turbo Sequence:

- Extract the TS Release on your Computer
- Copy the extracted Root folder in your Plugin folder of your UE Project
- Compile the your Project and open it in Unreal Engine

#### Demo and and Docs

Inside Unreal Engine, navigate to `..\Plugins\TurboSequence\Content\Demo` and Play through the demos, there is no additional setup required.

API and Docs can be found here:
<div align="center">

[![view - Documentation](https://img.shields.io/badge/view-Documentation-blue?style=for-the-badge)](https://github.com/LukasFratzl/Turbo-Sequence/wiki)

</div>

## Support and Responsibility

Turbo Sequence is a Hobby Project, nothing commercial behind the Repo, the Contributors are not responsible for solving bugs to users of Turbo Sequence, features may or may not come, if you really need a specific feature please fork the Repo and build your own system on top of it and optional create a pull request if you really think it should be part of the original Turbo Sequence Repo.

If you encounter a Bug, create an issue and when I have the time I try to response to it.

## Contribution

Contributions are welcome, if you want contribute; Fork the Repo, Create a Exp Branch, Create a Pull Request from the Exp Branch.
Pull requests need to be reviewed to keep the quality standards of this Repo in therms of runtime performance and the UE Coding Standards.

[![forks - Turbo-Sequence](https://img.shields.io/github/forks/LukasFratzl/Turbo-Sequence?style=social)](https://github.com/LukasFratzl/Turbo-Sequence)
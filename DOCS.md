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

## Limits

 - TS not using Nanite
 - Metahumans are not supported, please use UE Nanite Skeletal Meshes instead
 - Windows and Linux exclusive
 - Blueprint-Only projects are not supported

## How to use this docs:

The Docs releases are based on commits, if you use a later TS version please browse the repo at this give time of your release version and the repo should have the most recent data.

---

# Get stated with Content Creation

Skeletal Meshes are not compatible with Turbo Sequence; every skeletal mesh has to get converted into a Turbo Sequence Mesh Asset.

Here is a little checklist from the import of the Mesh to the production-ready Turbo Sequence Mesh:

### 1) Create a Material

- Materials for Turbo Sequence need to have the Turbo Sequence Position Offset Node plugged into the World Position Offset Pin
- The Material should have a Turbo Sequence Normal Calculation; there is Tangent Space and World Space, as well as an optional Flat Normal
- Turbo Sequence uses Niagara Static Meshes to render the Instances, which means the Material needs to have the Niagara Mesh Particle render feature
- Assign the Material to your Archetype Skeletal Mesh Character

### 2) Create the Static Meshes

- Open the Turbo Sequence Control Panel and follow the instructions until you generate the Mesh

### 3) Write or Use an Animation Controller

- Navigate to Plugins\TurboSequence\Resources and find BP_TurboSequence_MeshTester_Lf, drag and drop it into your map and assign the mesh
- Most Projects need to write specific Animation Controllers, in this case, there is a C++ API -> [Here](https://github.com/LukasFratzl/TurboSequence/blob/main/DOCS.md#get-started-with-the-scripting-api)

## Materials

The Material needs to support Niagara Mesh Particles, it needs a World Position Offset Pin.

Here a default Turbo Sequence Material in the most minimal way:

![Most Minimal Material](https://github.com/LukasFratzl/Turbo-Sequence/blob/main/TS_WikiResources/MostMinimalTSMaterial.png)

Turbo Sequence comes with prebuilt material functions; the use case is somehow clear by title.

![Material Function](https://github.com/LukasFratzl/Turbo-Sequence/blob/main/TS_WikiResources/MaterialFunctions.png)


## Control Panel

### How to show the Control Panel

Turbo Sequence has a custom Control Panel interface in Unreal Engine.
The Control Panel can be accessed from the drop-down menu.

![ShowCP](https://github.com/LukasFratzl/Turbo-Sequence/blob/main/TS_WikiResources/ShowControlPanel.png)

It has three sections:
- Reference Section
- Generate Setion
- Tweaks Section

It goes like this: reference the Mesh Asset, Generate the Mesh and optional tweak settings

### Reference Section

The Reference Section is a key part of Turbo Sequence.
It requires a Mesh Asset, without it, nothing works in the Control Panel. 
You can create a Mesh Asset by right click in the content browser of UE.

![Control Panel Reference Section](https://github.com/LukasFratzl/Turbo-Sequence/blob/main/TS_WikiResources/ControlPanelReference.png)

### Generate Section

Turbo Sequence works this way:
It does render a Static Mesh Model with Niagara, and the Material is deforming the Vertices of this Static Mesh to make it animate.
In order to animate a Mesh, you need bones and skin weight information, and TS gets this info from the Archetype Skeletal Mesh Model.

![Control Panel Generate Section](https://github.com/LukasFratzl/Turbo-Sequence/blob/main/TS_WikiResources/ControlPanelGenerate.png)

So the Generate Section is taking several steps:
- It lets you define a Skeletal Mesh Archetype for the generation (The Model you like to animate)
- It Generates Static Mesh, Level of Details based on your selection, 8–13 LODs is a good range
- It writes Bone information and Skin Weights into the Mesh Asset

After generating your model successfully, it should be ready for use.

### Tweak Section

The tweaks section is optional; sometimes you need to tweak the texture size to fit your personal mesh targets.
It highly depends on your target device; sometimes it makes sense to lower or raise it, but it always should be as close as possible to your target.

![Control Panel Tweaks Section](https://github.com/LukasFratzl/Turbo-Sequence/blob/main/TS_WikiResources/ControlPanelTweaks.png)

## Mesh Assets

Mesh Assets are Data Assets that store info about the Mesh, like all TS Mesh properties and Level of detail, as well as hidden data like Skin Weights, and bone indications for working with the CPU and GPU.

![Mesh Asset](https://github.com/LukasFratzl/Turbo-Sequence/blob/main/TS_WikiResources/MeshAssetExample.png)

When Tweaking the Mesh Asset, you can optimize a lot of CPU and GPU Times by simply iterating over, adjusting the settings and play testing.
Some performance optimization best practices can be found [Here](https://github.com/LukasFratzl/TurboSequence/blob/main/DOCS.md#optimization).


---

# Get Started with the Scripting API

Most Projects with Turbo Sequence require a custom Animation Controller which is invoking animations, IK and Root Motion or more.
In order to get started, here is the most minimal basic setup which every Animation Controller needs:

.h:
```cpp
#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_MinimalData_Lf.h"
#include "GameFramework/Actor.h"
#include "ADemoMeshTester.generated.h"

UCLASS()
class TURBOSEQUENCE_LF_API ADemoMeshTester : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADemoMeshTester();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="Mesh Tester")
	// The spawn data to give spawn info in the Actor Details panel
	FTurboSequence_MeshSpawnData_Lf SpawnData;

	UPROPERTY(EditAnywhere, Category="Mesh Tester")
	// The Animation to Play
	TObjectPtr<UAnimSequence> MeshAnimation;

	UPROPERTY(EditAnywhere, Category="Mesh Tester")
	// The Animation settings to play
	FTurboSequence_AnimPlaySettings_Lf MeshAnimationSettings = FTurboSequence_AnimPlaySettings_Lf();
};
```

.cpp:
```cpp
#include "ADemoMeshTester.h"

#include "TurboSequence_Manager_Lf.h"


// Sets default values
ADemoMeshTester::ADemoMeshTester()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADemoMeshTester::BeginPlay()
{
	Super::BeginPlay();

	// Create the Instance
	const FTurboSequence_MinimalMeshData_Lf& Instance = ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(SpawnData, GetActorTransform(), GetWorld());
	// When the Mesh Instance is valid, do stuff with it..
	if (Instance.IsMeshDataValid())
	{
		// Add it to an Update Group to solve it later in groups
		ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, Instance);

		// PLay an animation with the settings
		const FTurboSequence_AnimMinimalCollection_Lf& Animation = ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(Instance, MeshAnimation, MeshAnimationSettings);
	}
}

// Called every frame
void ADemoMeshTester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Create an Update Context
	FTurboSequence_UpdateContext_Lf UpdateContext = FTurboSequence_UpdateContext_Lf();
	// Here we use the same index we specified when creating the instance, more info about update groups in the docs
	UpdateContext.GroupIndex = 0;

	// Solve this group
	ATurboSequence_Manager_Lf::SolveMeshes_GameThread(DeltaTime, GetWorld(), UpdateContext);
}
```
This code above will generate one Mesh Instance and play an animation to solve it.

## Code Structure

TS best practice of writing the code structure is like following:
```

// Code Flow

+--------------------+
| Game Thread Logic..|
+--------------------+
      |
      v
+--------------------+
| for loop (all     |
|     instances)    |
+--------------------+
      |
      v
+--------------------+
| Solve Update Group |
+--------------------+
      |
      v
+--------------------+
| Game Thread Logic..|
+--------------------+
      |
      v
```

So basically, you update all instance at once, in a big loop which can be multithreaded and after this loop ends you need to solve the animations per update group.

ECS is totally fine, you can update all instances in an ECS Loop, however make sure to call
```cpp
ATurboSequence_Manager_Lf::SolveMeshes_GameThread(DeltaTime, GetWorld(), UpdateContext);
```
one time per update group, one time a frame

### Update Groups

This is optional and only needed if you encounter too high CPU Times.

In order to get better CPU Times, the instances can get updated in Update Groups, which mean:
```
+--------------------+
| Group 1            |
+--------------------+
      |
      v
+--------------------+
| Frame 1 Ends       |
+--------------------+
      |
      v
+--------------------+
| Group 2            |
+--------------------+
      |
      v
+--------------------+
| Frame 2 Ends       |
+--------------------+
      |
      v
+--------------------+
| Group N            |
+--------------------+
      |
      v
+--------------------+
| Frame N Ends       |
+--------------------+
      |
      v
```
It will make Group 1 Lag N Amounts of Frames behind the current Frame Group, which will look like slow update rates of the Animations, however if the Units are 50 Meters away from the Camera, the "Lag" is not noticeable.

With this way, it's possible to increase the number of instances in the map up to 200k or more, depending on the target device.

**Update Groups are not managed by the system, you have to design the logic yourself**

In order to add an instance to a group, you call:
```cpp
ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(MeshUpdateContext.GroupIndex, Instance);
```

To remove one instance to a group call:
```cpp
ATurboSequence_Manager_Lf::RemoveInstanceFromUpdateGroup_Concurrent(MeshUpdateContext.GroupIndex, Instance);
```

NOTE: It's self-managed for more control, that means you have to add and remove it yourself when adding and removing an instance.

- When Having multiple Update Groups, the DeltaTime is too small which mean you have to Accumulate it over the frames which are not part of this Group

## API References

The whole Turbo Sequence API is inside 2 Files:

- `TurboSequence_Manager_Lf.h` which contains all Functions and can be found [Here](https://github.com/LukasFratzl/TurboSequence/blob/main/Source/TurboSequence_Lf/Public/TurboSequence_Manager_Lf.h) in the Repo
- `TurboSequence_MinimalData_Lf.h` which contains the top level structures of the API, which can be found [Here](https://github.com/LukasFratzl/TurboSequence/blob/main/Source/TurboSequence_Lf/Public/TurboSequence_MinimalData_Lf.h) in the Repo

---

# Optimization

Graphics Card Memory is mostly limited across multiple Target Devices, so it makes sense using the CPU Memory, there are some engines out there which does work mostly on the GPU but for Unreal Engine this is not the case, a lot of it like Animations etc. are still using massively the CPU.

It also makes no sense to cache most of the Data, because in the end on a complicated system the amount of memory can be a bottleneck, sometimes generating the data at runtime is much faster.

Here some performance optimization guidelines:

## CPU Time

### Mesh Asset
- Increase `Time Between Animation Library Frames` to the most acceptable value
- Enable `Use Distance Updating` and play around with the `Distance Updating Ratio`
- Increase `Auto Lod Ratio`
- Disable Animations for High Distance LODs, do you really need to Animate LOD 14 which is 800 Meters away from the camera?

### Animation Controller
- Solve the Animations just one time per frame when having just one Update Group
- Use Multiple Update Groups, Updated Groups, when using correctly are the most powerful optimization technique in TS
- Multi-Thread the controller; 95% of all functions are Thread safe
- Use C++ Instead of BP

### Content Creation
- Use max 30–75 bones on the Mesh

## GPU Time

### Mesh Asset
- Increase `Time Between Animation Library Frames` to the most acceptable value
- Decrease `Highest Detail Draw Distance`
- Increase `Auto Lod Ratio`
- Disable Animations for High Distance LODs, do you really need to Animate LOD 14 which is 800 Meters away from the camera?

### Control Panel
- Make sure the Mesh has enough LODs, there is no performance downside using too many LODs, it may look wrong having just a quad on the last LOD
- In the Tweak Section, make sure the Textures having good dimensions

### Content Creation
- Use max 30–75 bones on the Mesh
- Make sure the Material you use is optimized, because the Vertex Skinning of the Vertices is a heavy operation
- Use less geometry for the mesh, you can exclude LODs from the system in the Mesh Asset, most of the Time LOD 2 or 3 is good enough for the 1st LOD

### Draw Calls
Having 2 Material on the Mesh meaning, there are 2 Mesh Sections under the hood: 
- One Archetype with 1 Mesh Section is generating 1 Draw call, with 4 Mesh Sections, 4 Draw Calls
- Individual Instances don't create draw calls
- Keep the Instances count high but keep the Archetype count low

## Memory
- Instances create CPU and GPU Memory
- More Animations Create More Memory on the CPU and GPU
- Increase `Time Between Animation Library Frames` to the most acceptable value to increase FPS

---

# Hybrid Mode

Hybrid Mode lets the developer create Unreal Engine Skeletal Meshes within Turbo Sequence Meshes.
The most common way:
- Unreal Engine Skeletal Mesh or UE Animation Systems in a range from 0 to 40 Meter
- Turbo Sequence Meshes with the faster Animation System in a range after this UE Systems 40 to 1000 Meters
- Of course 40 Meters is just a value which works good, you can always tweak the ranges

**The Hybrid Mode has a default fading behavior, which means it can smooth fade in and fade out between UE and TS**

### Default Hybrid Mode

1. Create a Data Asset derived from `TurboSequence_FootprintAsset_Lf`, or inherit `TurboSequence_FootprintAsset_Lf` from your own class, which allows modding.
2. Open it, and fill out the settings.
4. Assign the created Data Asset into one of your mesh spawn data, `FTurboSequence_MeshSpawnData_Lf` somewhere in your own animation controller where it makes sense for you.

You can tweak the functionality, create a derived class from `TurboSequence_FootprintAsset_Lf` and override the functions in C++, make sure the Data Asset is derived from your created override class.

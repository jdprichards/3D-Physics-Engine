# 3D Physics & Collision Detection Engine

This project serves as a foundational prototype of a 3D rigid-body physics engine built from first principles in C++ using DirectX 11, featuring custom collision detection, physics simulation, and GPU-accelerated rendering.

- Final year dissertation focused on real-time physics simulation and engine design 

---

## 🧠 Overview

This project implements a fully custom physics engine designed to simulate realistic rigid body dynamics in a 3D environment.

The engine was developed to explore the mathematical and computational foundations of physics simulation, including collision detection, numerical integration, and GPU-based rendering.

---

## ✨ Key Features

- ⚙️ **Rigid Body Physics Simulation**  
  Simulates motion using Newtonian mechanics and numerical integration techniques (Runge-Kutta 4 integration)

- 💥 **Collision Detection System**  
  Custom collision detection and response algorithms for 3D objects  

- 🎮 **Real-Time Rendering**  
  GPU-accelerated rendering using DirectX 11 and HLSL shaders  

- 🧮 **Physical Property Computation**  
  Calculates volume, mass, and inertia tensors for 3D bodies  

- 🚀 **Built from First Principles**  
  No external physics engines used — all systems implemented from scratch  

---

## 🛠️ Tech Stack

- **Language:** C++  
- **Graphics API:** DirectX 11  
- **Shaders:** HLSL  
- **Concepts:** Linear Algebra, Rigid Body Dynamics, Numerical Methods, Bounding volumes

---

## 🏗️ System Architecture

The engine is composed of several core systems:

### 1. Physics System
- Updates rigid body motion using numerical integration  
- Applies forces, velocities, and constraints
- Simulates multiple interacting rigid bodies in real time 

### 2. Collision Detection
- Detects intersections between 3D objects using bounding volumes
- Computes contact points and collision responses  

### 3. Rendering Pipeline
- Uses DirectX 11 for real-time rendering  
- Implements custom HLSL shaders for GPU acceleration  

### 4. Object Representation
- Stores physical properties (mass, inertia tensors, geometry)  
- Supports dynamic simulation of multiple bodies  

---

## 🔄 Ongoing Development

This project represents the initial implementation of a custom physics engine developed from first principles.

I am currently planning a follow-up project to redesign and extend this engine with a stronger focus on scalability, performance, and modern architecture. This will include:

- Improved system architecture and modular design  
- More advanced collision detection techniques (such as broad-phase and narrow-phase collision detection and spatial partitioning)
- Performance optimisations and potential multithreading
- Expanded use of GPU acceleration for simulation and rendering
- Support for additional constraints and simulation features  

The next iteration will be developed in a separate repository and will build upon the foundations established in this project.

---

## 🚀 Getting Started

### Prerequisites
- Windows OS  
- Visual Studio
- DirectX 11 SDK  

### Build & Run

```bash
git clone https://github.com/jdprichards/3D-Physics-Engine

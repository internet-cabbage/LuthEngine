# LuthEngine

**A 3D N body simulator designed to simulate galaxy collisions (eventually)**

<p align="center">
  <img src="docs/GithubDemo.gif" width="600" alt="LuthEngine simulating two N-body clusters interacting."/>
</p>


[//]: # "Badge code, idk why people do this but it looks cool at least"

![Language](https://img.shields.io/badge/language-C-blue)
![Rendering](https://img.shields.io/badge/rendering-OpenGL%20%7C%20VisPy-orange)
![Parallelism](https://img.shields.io/badge/parallelism-OpenMP-green)
![License](https://img.shields.io/badge/license-GPL--3.0-lightgrey)

## Overview

LuthEngine is a gravitational N-body simulator written entirely in C. It approximates the force exerted upon each particle in the simulation, and integrates this to simulate how the particle's positions change over a series of time steps. It uses a Barnes-Hut oct-tree to improve the performance scaling factor from O(N^2) of a naive implementation to O(Nlog(N)). The simulation writes the particle's positions to an output binary file, so that it can be played back either through a lightweight python script using VisPy, or a dedicated OpenGL renderer with various visual effects.

The long-term goal of this program is to simulate large scale galaxy collisions

## Highlights

## Components of the repo

## Physics and algorithms

### Force evaluation

### Gravitational softening

### Integration method

### Initial conditions

### Parallelism

### Memory usage

## Rendering

## Project layout

## Build and run

### Requirements

### Running the software

### Data output format

## Roadmap

## References

## License

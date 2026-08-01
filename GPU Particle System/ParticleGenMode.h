#pragma once

enum class ParticleGenMode : int {
    ContinuousEmitter = 0,
    BurstEmitter = 1,
    ShapeSphere = 2,
    ShapeDisc = 3,
    ShapeCube = 4,
};

inline bool IsShapeMode(int mode) { return mode >= (int)ParticleGenMode::ShapeSphere; }
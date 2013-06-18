﻿// Ŭnicode please 
#pragma once

#include "scene.h"

class DebugDrawManager2D;
class DebugDrawManager3D;

class DebugDrawScene : public Scene {
public:
	DebugDrawScene(irr::IrrlichtDevice *dev);
	virtual ~DebugDrawScene();

	virtual void setUp();
	virtual void shutDown();

	virtual void update(int ms);
	virtual void draw();
};

﻿// Ŭnicode please 
#pragma once

#include "scene.h"

class DebugDrawManager2D;
class DebugDrawManager3D;

class DebugDrawScene : public Scene {
public:
	DebugDrawScene(irr::IrrlichtDevice *dev);
	virtual ~DebugDrawScene();

	virtual void update(int ms);
	virtual void setUp();
	virtual void shutDown();

private:
	std::unique_ptr<DebugDrawManager2D> DebugDrawMgr2d;
	std::unique_ptr<DebugDrawManager3D> DebugDrawMgr3d;
};

﻿// Ŭnicode please 
#pragma once

typedef enum {
	//kSequenceTitle,
	kSequenceMain,
	kSequenceDebugDraw,
	kSequenceGame,
	kSequenceGameLoading,
	// demo라고 해서 다른 enum으로 분리하면 관리가 귀찮을듯
	kSequenceDemoCylinderHMD,
	kSequenceDemoSphereHMD,
	kSequenceDemoColor,
	kSequenceDemoAni,
} SequenceType;

typedef enum {
	kDisplayDefault,
	kDisplayFullscreen,
	kDisplayWindow,
} DisplayType;

class Sequence;

class SequenceFactory {
public:
	Sequence *createRaw(SequenceType type) const;
	std::unique_ptr<Sequence> create(SequenceType type) const;
	SequenceType select(DisplayType *displayType) const;
};
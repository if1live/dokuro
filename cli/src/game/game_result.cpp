﻿// Ŭnicode please 
#include "stdafx.h"
#include "game_result.h"
#include "rps_event.h"
#include "base/lib.h"
#include "res.h"
#include "irr/sprite_scene_node.h"
#include "irr/text_3d_scene_node.h"
#include "irr/debug_drawer.h"
#include "game/finger_direction_event.h"
#include "rock_paper_scissor.h"
#include "finger_direction_phase.h"
#include "attack_phase.h"
#include "defense_phase.h"
#include "score_board.h"
#include "util/SimpleAudioEngine.h"

using namespace std;
using namespace irr;

irr::video::ITexture *getChamChamChamTexture(const FingerDirectionEvent &evt)
{
	switch(evt.value) {
	case FingerDirectionEvent::kUp:
		return Lib::driver->getTexture(res::texture::ARROW_HOLLOW_UP_PNG);
		break;
	case FingerDirectionEvent::kDown:
		return Lib::driver->getTexture(res::texture::ARROW_HOLLOW_DOWN_PNG);
		break;
	case FingerDirectionEvent::kLeft:
		return Lib::driver->getTexture(res::texture::ARROW_HOLLOW_LEFT_PNG);
		break;
	case FingerDirectionEvent::kRight:
		return Lib::driver->getTexture(res::texture::ARROW_HOLLOW_RIGHT_PNG);
		break;
	default:
		SR_ASSERT(!"do not reach");
		break;
	}
	return nullptr;
}

AbstractGameResult::AbstractGameResult(irr::scene::ICameraSceneNode *cam, ScoreBoard *board, int remain)
	: SubSequence(cam, board),
	root_(nullptr),
	aiNode_(nullptr),
	playerNode_(nullptr),
	textNode_(nullptr),
	remain_(remain)
{
	//게임 끝난다음에 결과 보여주는 용도로 사용할 node
	root_ = Lib::smgr->addEmptySceneNode(cam);
	root_->grab();
	root_->setRotation(core::vector3df(0, 180, 0));
	//root_->setVisible(false);

	auto root = getRoot();

	const core::dimension2d<float> rpsSize(3, 3);
	const core::dimension2d<float> faceSize(2, 2);
	const float radius = 30.0f;
	{
		//left=ai
		auto leftNode = Lib::smgr->addEmptySceneNode(root);
		leftNode->setRotation(core::vector3df(10, -20, 0));
		aiNode_ = new irr::scene::SpriteSceneNode(leftNode, Lib::smgr, 0, nullptr);
		aiNode_->setPosition(core::vector3df(0, 0, radius));
		aiNode_->setSize(rpsSize);
		aiNode_->drop();

		auto faceTex = Lib::driver->getTexture(res::texture::FACE_REINA_PNG);
		auto face = new irr::scene::SpriteSceneNode(aiNode_, Lib::smgr, 0, faceTex);
		face->setPosition(core::vector3df(0, 5, -0.5));
		face->setSize(faceSize);
		face->drop();
	}

	{
		//right=player
		auto rightNode = Lib::smgr->addEmptySceneNode(root);
		rightNode->setRotation(core::vector3df(10, 20, 0));
		playerNode_ = new irr::scene::SpriteSceneNode(rightNode, Lib::smgr, 0, nullptr);
		playerNode_->setPosition(core::vector3df(0, 0, radius));
		playerNode_->setSize(rpsSize);
		playerNode_->drop();
		
		auto faceTex = Lib::driver->getTexture(res::texture::FACE_PLAYER_PNG);
		auto face = new irr::scene::SpriteSceneNode(playerNode_, Lib::smgr, 0, faceTex);
		face->setPosition(core::vector3df(0, 5, -0.5));
		face->setSize(faceSize);
		face->drop();
	}

	textNode_ = new irr::scene::Text3dSceneNode(root, Lib::smgr, 0, getDebugFont(), L"?"); 
	textNode_->setPosition(core::vector3df(0, 0, 30.0f));
	auto size = core::dimension2d<f32>(5.0f, 5.0f);
	textNode_->setSize(size);
	textNode_->drop();
}
AbstractGameResult::~AbstractGameResult()
{
	root_->remove();
	root_->drop();
	root_ = nullptr;
}
void AbstractGameResult::setAiTexture(irr::video::ITexture *tex)
{
	aiNode_->setMaterialTexture(0, tex);
}
void AbstractGameResult::setPlayerTexture(irr::video::ITexture *tex)
{
	playerNode_->setMaterialTexture(0, tex);
}
void AbstractGameResult::setText(const wchar_t *txt)
{
	textNode_->setText(txt);
}

std::unique_ptr<SubSequence> AbstractGameResult::update(int ms)
{
	remain_ -= ms;
	if(remain_ < 0) {
		return std::unique_ptr<SubSequence>(createNext());
	} else {
		std::unique_ptr<SubSequence> empty;
		return empty;
	}
}

////////////////////////////

RockPaperScissorResult::RockPaperScissorResult(irr::scene::ICameraSceneNode *cam,
											   ScoreBoard *board, 
											   const RPSEvent &playerChoice,
											   const RPSEvent &aiChoice)
	: AbstractGameResult(cam, board, 1000),
	aiChoice_(new RPSEvent(aiChoice)),
	playerChoice_(new RPSEvent(playerChoice)),
	effect_(AL_NONE)
{
	std::wstring msg;
	if(aiChoice == playerChoice) {
		msg = L"Draw";
	} else if(aiChoice > playerChoice) {
		msg = L"Lose";
	} else if(aiChoice < playerChoice) {
		msg = L"Win";
	} else {
		SR_ASSERT(!"do not reach");
	}
	setText(msg.c_str());
	setPlayerTexture(getRPSTexture(playerChoice.value)); 
	setAiTexture(getRPSTexture(aiChoice.value));

	effect_ = Lib::audio->playEffect(res::voice::PAPER_WAV);
}

RockPaperScissorResult::~RockPaperScissorResult()
{
}

irr::video::ITexture *RockPaperScissorResult::getRPSTexture(int rps)
{
	switch(rps) {
	case RPSEvent::kRock:
		return Lib::driver->getTexture(res::texture::ICON_ROCK_PNG);
		break;
	case RPSEvent::kPaper:
		return Lib::driver->getTexture(res::texture::ICON_PAPER_PNG);
		break;
	case RPSEvent::kScissor:
		return Lib::driver->getTexture(res::texture::ICON_SCISSOR_PNG);
		break;
	default:
		SR_ASSERT(!"do not reach");
		break;
	}
	return nullptr;
}

SubSequence *RockPaperScissorResult::createNext()
{
	const RPSEvent &ai = *aiChoice_;
	const RPSEvent &player = *playerChoice_;

	if(ai == player) {
		return new RockPaperScissor(getCamera(), getScoreBoard());
	} else if(ai > player) {
		return new DefensePhase(getCamera(), getScoreBoard());
	} else if(ai < player) {
		return new AttackPhase(getCamera(), getScoreBoard());
	} else {
		SR_ASSERT(!"do not reach");
		return nullptr;
	}
}

/////////////////////////////////////////////////////

FingerResult::FingerResult(irr::scene::ICameraSceneNode *cam,
						   ScoreBoard *board, 
		const FingerDirectionEvent &playerChoice,
		const FingerDirectionEvent &aiChoice)
	: AbstractGameResult(cam, board, 1000),
	aiChoice_(new FingerDirectionEvent(aiChoice)),
	playerChoice_(new FingerDirectionEvent(playerChoice)),
	effect_(AL_NONE)
{
	setPlayerTexture(getChamChamChamTexture(playerChoice)); 
	setAiTexture(getChamChamChamTexture(aiChoice));

	effect_ = Lib::audio->playEffect(res::voice::CHAM_WAV);
}
FingerResult::~FingerResult()
{
}

SubSequence *FingerResult::createNext()
{
	if(getScoreBoard()->isGameOver()) {
		//TODO 게임 오버에서 추가 메세지 띄우기?
		getScoreBoard()->reset();
		printf("new game\n");
	}
	return new RockPaperScissor(getCamera(), getScoreBoard());
}

AttackResult::AttackResult(irr::scene::ICameraSceneNode *cam,
						   ScoreBoard *board, 
		const FingerDirectionEvent &playerChoice,
		const FingerDirectionEvent &aiChoice)
		: FingerResult(cam, board, playerChoice, aiChoice)
{
	std::wstring msg;
	if(aiChoice == playerChoice) {
		msg = L"Success";
	} else {
		msg = L"Fail";
	}
	setText(msg.c_str());
	
	if(aiChoice == playerChoice) {
		//attack success
		board->playerGetPoint();
	}
}



DefenseResult::DefenseResult(irr::scene::ICameraSceneNode *cam,
							 ScoreBoard *board, 
		const FingerDirectionEvent &playerChoice,
		const FingerDirectionEvent &aiChoice)
	: FingerResult(cam, board, playerChoice, aiChoice)
{
	std::wstring msg;
	if(aiChoice == playerChoice) {
		msg = L"Fail";
	} else {
		msg = L"Success";
	}
	setText(msg.c_str());

	if(aiChoice == playerChoice) {
		//defense fail
		board->aiGetPoint();	
	}
}

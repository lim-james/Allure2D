#ifndef BUBBLE_MANAGER_H
#define BUBBLE_MANAGER_H

#include "Script.h"
#include "SpectrumBubbleMaterial.h"
#include "Prefab.h"
#include "AudioControlller.h"
#include "SongData.h"

#include <Audio/AudioFile.h>
#include <Handler/Handler.h>
#include <Events/Event.h>
#include <vector>

struct BubbleManager : Script {

	using base_type = BubbleManager;

	unsigned startFrequency;
	unsigned endFrequency;
	float audioDuration;

	float minRadius;
	float maxRadius;

	float rotationSpeed;
	float multiplier;

	Material::SpectrumBubble* material;
	Prefab* audioPrefab;

	BubbleManager();

	void FadeOut(Handler<void, void> const& completion = nullptr);
	void Play(SongData const& song);

private:

	float t, bt;
	float duration;
	float bInv; // inverse of beat

	SongData current;

	AudioSource* source;
	AudioController* controller;
	AudioFile<int16_t>* file;

	void Start() override;
	void Update() override;
	void OnDestroy() override;

};

#endif

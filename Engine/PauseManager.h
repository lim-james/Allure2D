#ifndef PAUSE_MANAGER_H
#define PAUSE_MANAGER_H

#include "Script.h"

struct PauseManager : Script {

	void Unpause();
	void Quit();

private:

	void Awake() override;

	void KeyHanlder(Events::Event* event);

};

#endif

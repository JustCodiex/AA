#include "AAStackFrame.h"

void AAStackFrame::Load() {

	this->venv = this->varEnv->CloneSelf();
	this->opPtr = 0;
	this->callCount++;

}

void AAStackFrame::Exit() {

	// If variable environment is valid, delete it
	if (this->venv) {
		delete this->venv;
	}

	// Reset variable environment
	this->venv = 0;

}

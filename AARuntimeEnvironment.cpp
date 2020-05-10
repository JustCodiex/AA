#include "AARuntimeEnvironment.h"

AARuntimeEnvironment::AARuntimeEnvironment() {
	this->opPointer = 0;
	this->procPointer = 0;
	this->venv = 0;
}

void AARuntimeEnvironment::PopEnvironment(bool self) {

	// If variable environment is valid, delete it
	if (this->venv) {
		delete this->venv;
	}

	// Reset variable environment
	this->venv = 0;

	// Also delete self if possible
	if (self) {

		// Delete self
		delete this;

	}

}

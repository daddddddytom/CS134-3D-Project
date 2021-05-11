#include "InputHandler.h"

bool InputHandler::getInputState(Input input) {
	return inputStates[input];
}

void InputHandler::setInputState(Input input, bool state) {
	inputStates[input] = state;
}
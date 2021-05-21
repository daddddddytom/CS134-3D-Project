#include "InputHandler.h"

/*
 *	CS 134 Final Project
 *  Team: Hugo Wong, Hanqi Dai (Tom), Tomer Erlich
 *
 *  InputHandler.cpp
 */
bool InputHandler::getInputState(Input input) {
	return inputStates[input];
}

void InputHandler::setInputState(Input input, bool state) {
	inputStates[input] = state;
}
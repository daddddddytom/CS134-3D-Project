#pragma once

/*
 A class for handling multiple user inputs.
*/
/*
 *	CS 134 Final Project
 *  Team: Hugo Wong, Hanqi Dai (Tom), Tomer Erlich
 *
 *  InputHandler.h
 */
class InputHandler {
private:
	bool inputStates[9] = { false, false, false, false, false, false, false, false, false};

public:
	enum Input { UP, DOWN, LEFT, RIGHT, SPACE, A, W, S, D, Q, E };

	InputHandler() {}

	bool getInputState(Input input);

	void setInputState(Input input, bool state);


};
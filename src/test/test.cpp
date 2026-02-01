#include <assert.h>


#include "../engine/game_objects/node.h"

int main() {
	Node* node = new Node();
	delete node;

	for (int i = 0; i < 100000; i++) {
		Node* node = new Node();
		delete node;
	}

	node = new Node();
	delete node;

	return 1;
}
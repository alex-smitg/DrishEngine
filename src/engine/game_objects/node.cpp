#include "node.h"


void Node::appendChild(Node* node) {
	if (node->parent != nullptr) {
		logError(node->name, " already has parent ", node->parent->name);
		return;
	}

	children.push_back(node);
	node->parent = this;
}

void Node::getAllChildNodes(Node* parent, std::vector<Node*>* vec) {
	for (int i = 0; i < parent->children.size(); i++) {
			vec->push_back(parent->children[i]);
			getAllChildNodes(parent->children[i], vec);
	}
}

Node* Node::findChild(std::string name) {
	for (Node* n : children) {
		Node* node = findChild(name);
		if (node->name == name) {
			if (node != n) {
				return node;
			}
		}
	}
	return this;
}

void Node::destroy() {
	logInfo("[NODE] Destroy ", name);

	if (parent != nullptr) {
		parent->children.erase(std::find(parent->children.begin(), parent->children.end(), this));
	}
	for (int i = children.size() - 1; i >= 0; i--) {
		children[i]->destroy();
	}

	delete this;
}

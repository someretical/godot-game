#include "player.h"

using namespace godot;

void Player::_bind_methods() {
}

Player::Player() {
}

Player::Player(Root *root, Vector2 pos) : m_root(root), m_pos(pos) {
}

Player::~Player() {
}

void Player::_process(double delta) {
}

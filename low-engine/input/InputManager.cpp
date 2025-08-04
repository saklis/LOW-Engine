#include "InputManager.h"

namespace LowEngine::Input {
	void InputManager::AddEmptyAction(const std::string& actionName) {
		Action newAction;
		newAction.Name = actionName;

		_actions[actionName] = std::move(newAction);

		_log->debug("Empty Action '{}' added");
	}

	void InputManager::RemoveAction(const std::string& actionName) {
		auto action = _actions.find(actionName);
		if (action != _actions.end()) {
			_actions.erase(action);
			_log->info("Action '{}' removed", actionName);
		}
		else { _log->warn("Attempted to remove non-existing action '{}'", actionName); }
	}

	void InputManager::RemoveAllActions() {
		_actions.clear();
		_log->info("All actions removed");
	}

	sf::Vector2i InputManager::GetMousePosition() { return _currentMousePosition; }

	Action* InputManager::GetAction(const std::string& actionName) {
		auto action = _actions.find(actionName);
		if (action != _actions.end()) { return &(action->second); }

		return nullptr;
	}

	std::unordered_map<std::string, Action>* InputManager::GetActions() { return &_actions; }

	void InputManager::ClearActionState() {
		_inputChanged = false;

		// reset state of actions that started/ended in previous update
		for (auto& action : _actions) {
			action.second.Started = false;
			action.second.Ended = false;
		}
	}

	bool InputManager::HasAction(const std::string& actionName) {
		auto action = _actions.find(actionName);
		if (action != _actions.end()) { return true; }

		return false;
	}

	void InputManager::Read(const std::optional<sf::Event>& event) {
		// current state
		if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
			_currentMousePosition = mouseMoved->position;
		}
		if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
			_currentMouseButtons.emplace_back(mouseButtonPressed->button);
			_inputChanged = true;
		}
		if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
			auto it = std::find(_currentMouseButtons.begin(), _currentMouseButtons.end(), mouseButtonReleased->button);
			if (it != _currentMouseButtons.end()) { _currentMouseButtons.erase(it); }
			_inputChanged = true;
		}
		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
			// read modifiers
			if (keyPressed->code == sf::Keyboard::Key::LShift) {
				_currentModifiers.LShift = true;
				_inputChanged = true;
			}
			else if (keyPressed->code == sf::Keyboard::Key::LControl) {
				_currentModifiers.LCtrl = true;
				_inputChanged = true;
			}
			else if (keyPressed->code == sf::Keyboard::Key::LAlt) {
				_currentModifiers.LAlt = true;
				_inputChanged = true;
			}
			else if (keyPressed->code == sf::Keyboard::Key::RShift) {
				_currentModifiers.RShift = true;
				_inputChanged = true;
			}
			else if (keyPressed->code == sf::Keyboard::Key::RControl) {
				_currentModifiers.RCtrl = true;
				_inputChanged = true;
			}
			else if (keyPressed->code == sf::Keyboard::Key::RAlt) {
				_currentModifiers.RAlt = true;
				_inputChanged = true;
			}
			else {
				_currentKeys.emplace_back(keyPressed->code);
				_inputChanged = true;
			}
		}
		if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
			// read modifiers
			if (keyReleased->code == sf::Keyboard::Key::LShift) {
				_currentModifiers.LShift = false;
				_inputChanged = true;
			}
			else if (keyReleased->code == sf::Keyboard::Key::LControl) {
				_currentModifiers.LCtrl = false;
				_inputChanged = true;
			}
			else if (keyReleased->code == sf::Keyboard::Key::LAlt) {
				_currentModifiers.LAlt = false;
				_inputChanged = true;
			}
			else if (keyReleased->code == sf::Keyboard::Key::RShift) {
				_currentModifiers.RShift = false;
				_inputChanged = true;
			}
			else if (keyReleased->code == sf::Keyboard::Key::RControl) {
				_currentModifiers.RCtrl = false;
				_inputChanged = true;
			}
			else if (keyReleased->code == sf::Keyboard::Key::RAlt) {
				_currentModifiers.RAlt = false;
				_inputChanged = true;
			}
			else {
				auto it = std::find(_currentKeys.begin(), _currentKeys.end(), keyReleased->code);
				if (it != _currentKeys.end()) { _currentKeys.erase(it); }
				_inputChanged = true;
			}
		}
	}

	void InputManager::Update() {
		if (_inputChanged) {
			for (auto& action : _actions) {
				if (action.second.Type == ActionType::Keyboard) {
					if (action.second.Active) {
						if (!action.second.IsKeyPressed(_currentKeys) || action.second != _currentModifiers) {
							action.second.Active = false;
							action.second.Ended = true;
						}
					}
					else {
						if (action.second.IsKeyPressed(_currentKeys) && action.second == _currentModifiers) {
							action.second.Active = true;
							action.second.Started = true;
						}
					}
				}
				if (action.second.Type == ActionType::Mouse) {
					if (action.second.Active) {
						if (!action.second.IsMouseButtonPressed(_currentMouseButtons) || action.second !=
							_currentModifiers) {
							action.second.Active = false;
							action.second.Ended = true;
						}
					}
					else {
						if (action.second.IsMouseButtonPressed(_currentMouseButtons) && action.second ==
							_currentModifiers) {
							action.second.Active = true;
							action.second.Started = true;
						}
					}
				}
			}
		}
	}

	nlohmann::ordered_json InputManager::SerializeActionsToJSON() {
		nlohmann::ordered_json actionsJson;
		for (const auto& action : _actions) {
			nlohmann::ordered_json actionJson;
			actionJson["name"] = action.first;
			actionJson["type"] = action.second.Type;
			if (action.second.Type == ActionType::Keyboard) { actionJson["key"] = action.second.Key; }
			else if (action.second.Type == ActionType::Mouse) { actionJson["mouseButton"] = action.second.MouseButton; }
			actionJson["LShift"] = action.second.LShift;
			actionJson["LCtrl"] = action.second.LCtrl;
			actionJson["LAlt"] = action.second.LAlt;
			actionJson["RShift"] = action.second.RShift;
			actionJson["RCtrl"] = action.second.RCtrl;
			actionJson["RAlt"] = action.second.RAlt;
			actionsJson.push_back(actionJson);
		}
		return actionsJson;
	}

	bool InputManager::LoadActionsFromJSON(const nlohmann::basic_json<nlohmann::ordered_map>& actionsJson) {
		for (const auto& actionJson : actionsJson) {
			if (!actionJson.contains("name") || !actionJson.contains("type")) {
				_log->error("Invalid action JSON format");
				return false;
			}
			auto actionName = actionJson["name"].get<std::string>();
			auto actionType = actionJson["type"].get<ActionType>();
			Action newAction;
			newAction.Name = actionName;
			newAction.Type = actionType;
			switch (actionType) {
			case ActionType::Keyboard:
				if (!actionJson.contains("key")) {
					_log->error("Missing key for keyboard action '{}'", actionName);
					return false;
				}
				newAction.Key = actionJson["key"].get<sf::Keyboard::Key>();
				break;
			case ActionType::Mouse:
				if (!actionJson.contains("mouseButton")) {
					_log->error("Missing mouse button for mouse action '{}'", actionName);
					return false;
				}
				newAction.MouseButton = actionJson["mouseButton"].get<sf::Mouse::Button>();
				break;
			default:
				_log->error("Unknown action type for action '{}'", actionName);
				return false;
			}
			newAction.LShift = actionJson.value("LShift", false);
			newAction.LCtrl = actionJson.value("LCtrl", false);
			newAction.LAlt = actionJson.value("LAlt", false);
			newAction.RShift = actionJson.value("RShift", false);
			newAction.RCtrl = actionJson.value("RCtrl", false);
			newAction.RAlt = actionJson.value("RAlt", false);
			_actions[actionName] = std::move(newAction);

			switch (actionType) {
			case ActionType::Keyboard:
				_log->debug("Action '{}' added with keyboard button {} with modifiers {}",
				            actionName, static_cast<int>(newAction.Key),
				            FormatKeysModifiers(newAction.LShift, newAction.LCtrl, newAction.LAlt, newAction.RShift,
				                                newAction.RCtrl, newAction.RAlt));
				break;
			case ActionType::Mouse:
				_log->debug("Action '{}' added with mouse button {} with modifiers {}",
				            actionName, static_cast<int>(newAction.MouseButton),
				            FormatKeysModifiers(newAction.LShift, newAction.LCtrl, newAction.LAlt, newAction.RShift,
				                                newAction.RCtrl, newAction.RAlt));
				break;
			default:
				_log->error("Unknown action type for action '{}'", actionName);
				return false;
			}
		}
		return true;
	}

	std::string InputManager::GetKeyName(sf::Keyboard::Key key) {
		switch (key) {
		case sf::Keyboard::Key::Unknown:
			return "Unknown";
			break;
		case sf::Keyboard::Key::A:
			return "A";
			break;
		case sf::Keyboard::Key::B:
			return "B";
			break;
		case sf::Keyboard::Key::C:
			return "C";
			break;
		case sf::Keyboard::Key::D:
			return "D";
			break;
		case sf::Keyboard::Key::E:
			return "E";
			break;
		case sf::Keyboard::Key::F:
			return "F";
			break;
		case sf::Keyboard::Key::G:
			return "G";
			break;
		case sf::Keyboard::Key::H:
			return "H";
			break;
		case sf::Keyboard::Key::I:
			return "I";
			break;
		case sf::Keyboard::Key::J:
			return "J";
			break;
		case sf::Keyboard::Key::K:
			return "K";
			break;
		case sf::Keyboard::Key::L:
			return "L";
			break;
		case sf::Keyboard::Key::M:
			return "M";
			break;
		case sf::Keyboard::Key::N:
			return "N";
			break;
		case sf::Keyboard::Key::O:
			return "O";
			break;
		case sf::Keyboard::Key::P:
			return "P";
			break;
		case sf::Keyboard::Key::Q:
			return "Q";
			break;
		case sf::Keyboard::Key::R:
			return "R";
			break;
		case sf::Keyboard::Key::S:
			return "S";
			break;
		case sf::Keyboard::Key::T:
			return "T";
			break;
		case sf::Keyboard::Key::U:
			return "U";
			break;
		case sf::Keyboard::Key::V:
			return "V";
			break;
		case sf::Keyboard::Key::W:
			return "W";
			break;
		case sf::Keyboard::Key::X:
			return "X";
			break;
		case sf::Keyboard::Key::Y:
			return "Y";
			break;
		case sf::Keyboard::Key::Z:
			return "Z";
			break;
		case sf::Keyboard::Key::Num0:
			return "0";
			break;
		case sf::Keyboard::Key::Num1:
			return "1";
			break;
		case sf::Keyboard::Key::Num2:
			return "2";
			break;
		case sf::Keyboard::Key::Num3:
			return "3";
			break;
		case sf::Keyboard::Key::Num4:
			return "4";
			break;
		case sf::Keyboard::Key::Num5:
			return "5";
			break;
		case sf::Keyboard::Key::Num6:
			return "6";
			break;
		case sf::Keyboard::Key::Num7:
			return "7";
			break;
		case sf::Keyboard::Key::Num8:
			return "8";
			break;
		case sf::Keyboard::Key::Num9:
			return "9";
			break;
		case sf::Keyboard::Key::Escape:
			return "Escape";
			break;
		case sf::Keyboard::Key::LControl:
			return "Left Control";
			break;
		case sf::Keyboard::Key::LShift:
			return "Left Shift";
			break;
		case sf::Keyboard::Key::LAlt:
			return "Left Alt";
			break;
		case sf::Keyboard::Key::LSystem:
			return "Left System";
			break;
		case sf::Keyboard::Key::RControl:
			return "Right Control";
			break;
		case sf::Keyboard::Key::RShift:
			return "Right Shift";
			break;
		case sf::Keyboard::Key::RAlt:
			return "Right Alt";
			break;
		case sf::Keyboard::Key::RSystem:
			return "Right System";
			break;
		case sf::Keyboard::Key::Menu:
			return "Menu";
			break;
		case sf::Keyboard::Key::LBracket:
			return "Left Bracket";
			break;
		case sf::Keyboard::Key::RBracket:
			return "Right Bracket";
			break;
		case sf::Keyboard::Key::Semicolon:
			return ";";
			break;
		case sf::Keyboard::Key::Comma:
			return ",";
			break;
		case sf::Keyboard::Key::Period:
			return ".";
			break;
		case sf::Keyboard::Key::Apostrophe:
			return "'";
			break;
		case sf::Keyboard::Key::Slash:
			return "/";
			break;
		case sf::Keyboard::Key::Backslash:
			return "\\";
			break;
		case sf::Keyboard::Key::Grave:
			return "`";
			break;
		case sf::Keyboard::Key::Equal:
			return "=";
			break;
		case sf::Keyboard::Key::Hyphen:
			return "-";
			break;
		case sf::Keyboard::Key::Space:
			return "Space";
			break;
		case sf::Keyboard::Key::Enter:
			return "Enter";
			break;
		case sf::Keyboard::Key::Backspace:
			return "Backspace";
			break;
		case sf::Keyboard::Key::Tab:
			return "Tab";
			break;
		case sf::Keyboard::Key::PageUp:
			return "Page Up";
			break;
		case sf::Keyboard::Key::PageDown:
			return "Page Down";
			break;
		case sf::Keyboard::Key::End:
			return "End";
			break;
		case sf::Keyboard::Key::Home:
			return "Home";
			break;
		case sf::Keyboard::Key::Insert:
			return "Insert";
			break;
		case sf::Keyboard::Key::Delete:
			return "Delete";
			break;
		case sf::Keyboard::Key::Add:
			return "+";
			break;
		case sf::Keyboard::Key::Subtract:
			return "-";
			break;
		case sf::Keyboard::Key::Multiply:
			return "*";
			break;
		case sf::Keyboard::Key::Divide:
			return "/";
			break;
		case sf::Keyboard::Key::Left:
			return "Left Arrow";
			break;
		case sf::Keyboard::Key::Right:
			return "Right Arrow";
			break;
		case sf::Keyboard::Key::Up:
			return "Up Arrow";
			break;
		case sf::Keyboard::Key::Down:
			return "Down Arrow";
			break;
		case sf::Keyboard::Key::Numpad0:
			return "Numpad 0";
			break;
		case sf::Keyboard::Key::Numpad1:
			return "Numpad 1";
			break;
		case sf::Keyboard::Key::Numpad2:
			return "Numpad 2";
			break;
		case sf::Keyboard::Key::Numpad3:
			return "Numpad 3";
			break;
		case sf::Keyboard::Key::Numpad4:
			return "Numpad 4";
			break;
		case sf::Keyboard::Key::Numpad5:
			return "Numpad 5";
			break;
		case sf::Keyboard::Key::Numpad6:
			return "Numpad 6";
			break;
		case sf::Keyboard::Key::Numpad7:
			return "Numpad 7";
			break;
		case sf::Keyboard::Key::Numpad8:
			return "Numpad 8";
			break;
		case sf::Keyboard::Key::Numpad9:
			return "Numpad 9";
			break;
		case sf::Keyboard::Key::F1:
			return "F1";
			break;
		case sf::Keyboard::Key::F2:
			return "F2";
			break;
		case sf::Keyboard::Key::F3:
			return "F3";
			break;
		case sf::Keyboard::Key::F4:
			return "F4";
			break;
		case sf::Keyboard::Key::F5:
			return "F5";
			break;
		case sf::Keyboard::Key::F6:
			return "F6";
			break;
		case sf::Keyboard::Key::F7:
			return "F7";
			break;
		case sf::Keyboard::Key::F8:
			return "F8";
			break;
		case sf::Keyboard::Key::F9:
			return "F9";
			break;
		case sf::Keyboard::Key::F10:
			return "F10";
			break;
		case sf::Keyboard::Key::F11:
			return "F11";
			break;
		case sf::Keyboard::Key::F12:
			return "F12";
			break;
		case sf::Keyboard::Key::F13:
			return "F13";
			break;
		case sf::Keyboard::Key::F14:
			return "F14";
			break;
		case sf::Keyboard::Key::F15:
			return "F15";
			break;
		case sf::Keyboard::Key::Pause:
			return "Pause";
			break;
		default: return "Key " + std::to_string(static_cast<int>(key));
		}
	}

	std::string InputManager::GetMouseButtonName(sf::Mouse::Button button) {
		switch (button) {
		case sf::Mouse::Button::Left: return "Left";
		case sf::Mouse::Button::Right: return "Right";
		case sf::Mouse::Button::Middle: return "Middle";
		case sf::Mouse::Button::Extra1: return "Button 4";
		case sf::Mouse::Button::Extra2: return "Button 5";
		default: return "Button " + std::to_string(static_cast<int>(button));
		}
	}

	void InputManager::ApplyKeyboardModifiers(Action& action, const std::vector<sf::Keyboard::Key>& modifierKeys) {
		for (const auto& modifier : modifierKeys) {
			switch (modifier) {
			case sf::Keyboard::Key::LShift: action.LShift = true;
				break;
			case sf::Keyboard::Key::LControl: action.LCtrl = true;
				break;
			case sf::Keyboard::Key::LAlt: action.LAlt = true;
				break;
			case sf::Keyboard::Key::RShift: action.RShift = true;
				break;
			case sf::Keyboard::Key::RControl: action.RCtrl = true;
				break;
			case sf::Keyboard::Key::RAlt: action.RAlt = true;
				break;
			default: break;
			}
		}
	}
}

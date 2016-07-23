/*
  ==============================================================================

    CommandMap.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/

#include "CommandMap.h"
#include "LRCommands.h"

CommandMap::CommandMap() noexcept {}

void CommandMap::addCommandforMessage(unsigned int command, const MIDI_Message& message) {
    // adds a message to the message:command map, and its associated command to the
    // command:message map
  if (command < LRCommandList::LRStringList.size()) {
    message_map_[message] = LRCommandList::LRStringList[command];
    command_string_map_.insert({LRCommandList::LRStringList[command], message});
  }
  else
    message_map_[message] = LRCommandList::NextPrevProfile[command - LRCommandList::LRStringList.size()];
}

void CommandMap::addCommandforMessage(const std::string& command, const MIDI_Message& message) {
  message_map_[message] = command;
  command_string_map_.insert({command, message});
}

const std::string& CommandMap::getCommandforMessage(const MIDI_Message& message) const {
  return message_map_.at(message);
}

void CommandMap::removeMessage(const MIDI_Message& message) {
    // removes message from the message:command map, and its associated command from
    // the command:message map
  command_string_map_.erase(message_map_[message]);
  message_map_.erase(message);
}

void CommandMap::clearMap() noexcept {
  command_string_map_.clear();
  message_map_.clear();
}

bool CommandMap::messageExistsInMap(const MIDI_Message& message) const {
  return message_map_.count(message) > 0 ? true : false;
}

std::vector<const MIDI_Message*> CommandMap::getMessagesForCommand(const std::string& command) const {
  std::vector<const MIDI_Message*> mm;
  const auto range = command_string_map_.equal_range(command);
  for (auto it = range.first; it != range.second; ++it)
    mm.push_back(&it->second);
  return mm;
}

bool CommandMap::commandHasAssociatedMessage(const std::string& command) const {
  return command_string_map_.find(command) != command_string_map_.end();
}

void CommandMap::toXMLDocument(juce::File& file) const {
  if (message_map_.size()) {//don't bother if map is empty
    // save the contents of the command map to an xml file
    juce::XmlElement root{"settings"};
    for (const auto& map_entry : message_map_) {
      auto* setting = new juce::XmlElement{"setting"};
      setting->setAttribute("channel", map_entry.first.channel);
      if (map_entry.first.isCC)
        setting->setAttribute("controller", map_entry.first.controller);
      else
        setting->setAttribute("note", map_entry.first.pitch);
      setting->setAttribute("command_string", map_entry.second);
      root.addChildElement(setting);
    }
    if (!root.writeToFile(file, ""))
        // Give feedback if file-save doesn't work
      juce::AlertWindow::showMessageBox(juce::AlertWindow::WarningIcon, "File Save Error",
        "Unable to save file as specified. Please try again, and consider saving to a different location.");
  }
}
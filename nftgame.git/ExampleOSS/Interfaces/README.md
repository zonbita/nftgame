# Additional interfaces for Online Subsystems

These interfaces provide a standard way of accessing additional functionality for online subsystems in Unreal Engine.

You should add this repository as a Git submodule underneath the `Private` folder in your module. Follow the instructions in `OnlineLobbyInterface.h` for more details.

## OnlineAvatarInterface

This interface provides a standard way of accessing avatars of players in online subsystems.

## OnlineLobbyInterface

This interface provides a standard way of accessing lobby functionality of online subsystems.

Unreal Engine currently ships with two interfaces which map be mapped onto lobbies: sessions and parties. Unfortunately, sessions do not provide any kind of attribute system, and parties do not provide search functionality, so neither of them map cleanly onto lobbies in a way that the Matchmaking plugin can use. This interface covers that gap.

## OnlineVoiceAdminInterface

This interface provides a standard way of controlling voice channels on dedicated servers. It allows you to create credentials for voice channels as well as mute and kick participants.
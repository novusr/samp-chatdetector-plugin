#if defined _samp_chat_detector_included
	#endinput
#endif
#define _samp_chat_detector_included

// Called when a player's typing state changes
// playerid: The ID of the player
// isTyping: true if player started typing, false if player stopped
forward OnPlayerTypingStateChange(playerid, bool:isTyping);

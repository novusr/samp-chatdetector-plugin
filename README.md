# SAMP-ChatDetector

Detects when players open or close the chatbox (T key).

## Using

Place the plugin files in your server directory:
- **Windows**: `SAMP-ChatDetector.dll` → `plugins/`
- **Linux**: `SAMP-ChatDetector.so` → `plugins/`
- **Client**: `SAMP-ChatDetector.asi` → SA-MP directory

Add to `server.cfg`:
```
plugins SAMP-ChatDetector
```

Include the header file in your script:
```pawn
#include <SAMP-ChatDetector>
```

## Simple example

```pawn
public OnPlayerTypingStateChange(playerid, bool:isTyping)
{
    new buffer[64], name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof(name));

    if (isTyping)
    {
        format(buffer, sizeof(buffer), "%s is typing...", name);
    }
    else
    {
        format(buffer, sizeof(buffer), "%s stopped typing.", name);
    }
    SendClientMessageToAll(-1, buffer);
}
```

## Callback

| Callback | Description |
|---|---|
| `OnPlayerTypingStateChange(playerid, bool:isTyping)` | Called when a player opens or closes the chatbox. `isTyping` is `true` when opened, `false` when closed. |

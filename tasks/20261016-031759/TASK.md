# Architecture of Echo-Mita

- STATUS: OPEN
- PRIORITY: 100
- TAGS: architecture, abstruction

Architecture of Echo-Mita should be modular, simplicity, controlable.
I think the structure is useful:
```
+-src/
| +-core/
| | +-audio/
| | +-audio_processing/
| | +-wake-word-detection/
| | +-commands/
| | +-stt/
| | +-tts/
| +-cli/
| +-gui
| +-main.c
```

A structure for resources (audio samples, commands)  
```
.config/
| +-audio/
| +-model/
| +-commands
| | +-search
| | | +-scripts/
| | | | +-google.sh
| | | +-search.yml
```
![Graph of architecture](./arch.drawio)

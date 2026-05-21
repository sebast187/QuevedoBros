# Quevedo Bros!

![C](https://img.shields.io/badge/Language-C-blue.svg)
![Raylib](https://img.shields.io/badge/Framework-Raylib-red.svg)
![macOS](https://img.shields.io/badge/macOS-Native-black.svg)
![Windows](https://img.shields.io/badge/Windows-Supported-blue.svg)

**Quevedo Bros** is a love letter to classic 8-bit platformers. What started as an experiment evolved into a fully custom 2D game engine written completely from scratch in C. 

There are no game-engine drag-and-drop tools here—every cloud, particle explosion, gravity calculation, and camera movement is powered by raw code and math!

---

## 🎮 How to Play (Download)

**Works natively on Windows and Mac!** You do not need to install any coding software to play the game.

To Download the latest preview, open the `build/` folder in this repository and download:
* 🍎 **Mac:** Download `QuevedoBros.zip`, extract it, and double-click the App to play!
* 🪟 **Windows:** Download `Windows_Release.zip`, extract the folder, and run `QuevedoBros.exe`. *(Note: Keep the resources folder next to the .exe so the music plays!)*

---

## 🌟 Features

### 🕹️ Gameplay
* **Momentum Physics:** Mario-style acceleration, sliding friction, air-drag, and variable jump heights (tap for a short hop, hold for a high jump).
* **11 Unique Levels:** 8 challenging main campaign levels featuring 4 distinct biomes (Grassland, Crystal Caves, Frostbite Towers, and Lava Castle).
* **Secret Warp Zones:** Find the hidden green pipes and press `DOWN` to warp to 3 completely secret levels!
* **2-Player Co-Op:** Classic turn-based multiplayer. If Player 1 (Red) dies, the game dynamically hands the controller to Player 2 (Green), tracking independent scores, lives, and level progress.
* **Save & Highscore System:** Locally saves your game progress and tracks the Top 5 Highscores.

### ⚙️ Under The Hood (The Tech)
* **Custom C Engine:** Built on top of the [Raylib](https://www.raylib.com/) framework.
* **Procedural Rendering:** Zero image assets are used for the graphics! Every character, enemy, brick, and parallax background is procedurally drawn using Raylib shape rendering and Sine wave math.
* **Advanced Collision:** Custom AABB collision with "Corner Forgiveness" (prevents toe-stubbing on jumps) and dynamically shrinking hitboxes for buttery-smooth platforming.
* **Dynamic Audio Engine:** Seamlessly cross-fades background music depending on the level's theme.

---

## ⌨️ Controls

| Action | Key / Button |
| :--- | :--- |
| **Move** | `Left / Right Arrows` or `A / D` |
| **Jump** | `Up Arrow` or `Spacebar` *(Hold to jump higher!)* |
| **Enter Pipe** | `Down Arrow` or `S` |
| **Pause** | `P` or `ESC` |
| **Menu Navigation**| `Arrows` and `Enter` |

---

## 🛠️ For Developers: Building from Source

If you want to compile the game yourself, ensure you have a C compiler (`clang` or `gcc`) and `raylib` installed.

### Mac Build
To compile the raw executable:
```bash
make

# CG1 – Projektabgabe Team Green – SoSe 2025

## Inhaltsverzeichnis

- [Inhaltsverzeichnis](#inhaltsverzeichnis)
- [Features](#features)
- [Abhängigkeiten](#abhängigkeiten)
- [Steuerung](#steuerung)
- [Roadmap](#roadmap)
- [Screenshots](#screenshots)
- [Autoren](#autoren)

## Features

* **OpenGL 3.3 Core**, Fenster- und Vollbildmodus
* **Skybox** inkl. separatem Shaderprogramm und Cubemap, auf verschiedene Tageszeiten umschaltbar
* **Lichtsystem**: Sonne (Directional), Lampe (Point), Taschenlampe (Spot); per UI umschaltbar
* **Nebel**
* **Texturen** pro Material: Albedo, Normalen & Roughness
* **Objekte**: Teapot, Säule, Würfel (Glas), Gras‑Plane, Hütte, "Slenderman"‑Figur, Bäume, Laterne
* **Kamera**: 360° Kamerasteuerung + freie Bewegung
* **Reflektion der Skybox**: Auf Glas oder universell zuschaltbar

## Steuerung

| Funktion               | Taste(n)         |
| ---------------------- | ---------------- |
| Sonnenlicht umschalten | `1`              |
| Punktlicht umschalten  | `2`              |
| Spotlight umschalten   | `f`              |
| Nebel ein/aus          | `n`              |
| Vollbild ein/aus       | `v`              |
| Bewegen                | `<W/A/S/D>`      |
| Umsehen                | `Mouse`          |
| Universelle Spiegelung | `7`              |
| Skybox: Dämmerung      | `8`              |
| Skybox: Tag            | `9`              |
| Skybox: Nacht          | `0`              |

## Roadmap

* Schatten für Lichter
* Interaktion (Zettel sammelen)
* Mehr Vegetation und Instancing

## Abhängigkeiten

* **GLFW**
* **GLEW**
* **stb\_image** (`ImageLoader.h`)
* **Compiler**: GCC (c23)

## Screenshots

![Scene Overview](docs/screenshot_01.png)
![Glass Pass](docs/screenshot_glass.png)

## Autoren

Beverly Hermann,
Lennart Vermehr
Nils Obitz,
Vincent Terzenbach

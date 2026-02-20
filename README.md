# Qt Calculator (C++)

![Windows CI](https://github.com/<USER>/<REPO>/actions/workflows/windows.yml/badge.svg)


## Overview
A calculator application built with Qt, designed using the same
architecture and development principles as the Number Guess Game.

## Design Principles
- Pure-function domain logic
- Immutable state transitions
- Event-driven UI updates
- Clear separation of UI, application, and domain layers

## Architecture
![Architecture](docs/images/architecture.png)

## Domain
![Domain](docs/images/domain.png)

## Build
```bash
cmake -S . -B build
cmake --build build
```

## Development Note
This project was developed with the assistance of AI tools (ChatGPT) for architectural guidance, debugging, and best practices.

All integration, configuration, and final implementation were performed and validated by the author.
# TSA Screen - Tactical Situation Awareness Display

A Qt-based tactical display system for maritime/naval operations showing sensor coverage, target tracking, and vector analysis.

## Overview

This application simulates a Tactical Situation Awareness (TSA) screen commonly used in naval operations. It displays:

- **Sensor Beam**: Green line showing active sensor coverage area
- **Hatched Region**: Gray cross-hatched area below beam (inactive sensor zone)
- **Own Ship**: Yellow marker with cyan velocity vector (North at 10 knots)
- **Sensor Position**: Red marker on the sensor line
- **Trial Track**: Yellow vector showing potential course change (+20° from current)
- **Adopted Track**: Red vector from sensor position (SW direction)
- **Bearing Rate**: Cyan vector showing rate of bearing change

## Features

- Real-time simulation with 2-second update intervals
- Dynamic target tracking with bearing/range calculations
- Visual vector analysis for tactical decision making
- Cross-hatched sensor coverage visualization
- Smooth Qt-based rendering with anti-aliasing

## Build Requirements

- **Qt 5.x** (Widgets, GUI, Core modules)
- **C++11** compatible compiler (GCC/Clang)
- **Linux** (tested on Ubuntu)

## Build Instructions

```bash
# Install Qt5 development packages (Ubuntu/Debian)
sudo apt install qt5-default qtbase5-dev

# Build the project
make clean
make

# Run the application
./TSAScreen
```

## Project Structure

```
TSA_Screen/
├── src/
│   ├── main.cpp              # Application entry point
│   ├── diagramwidget.h       # TSAWidget class declaration
│   └── diagramwidget.cpp     # Main display logic & simulation
├── TSA_Screen.pro           # Qt project file
├── Makefile                 # Build configuration
└── README.md               # This file
```

## Key Components

### TSAWidget Class
- **Simulation Engine**: Updates target position, bearing, and range every 2 seconds
- **Drawing System**: Renders all visual elements with proper layering
- **Vector Analysis**: Calculates and displays tactical vectors

### Simulation Parameters
- **Own Ship**: Course 0° (North), Speed 10 knots, Depth 40m
- **Target**: Initial position (3,3) nm, Course 90°, Speed 8 knots
- **Update Rate**: 2-second intervals

### Visual Elements
1. **Black Background**: Standard tactical display background
2. **Green Sensor Beam**: Active sensor coverage line
3. **Gray Hatched Area**: Inactive sensor region below beam
4. **Yellow Own Ship**: Current vessel position with velocity vector
5. **Red Sensor Marker**: Sensor position on beam line
6. **Tactical Vectors**: Various colored arrows for analysis

## Usage

The application runs automatically with continuous simulation updates. The display shows:

- Real-time bearing and range calculations in console output
- Visual representation of tactical situation
- Vector analysis for course planning

## Technical Details

- **Rendering**: Qt5 QPainter with anti-aliasing
- **Timing**: QTimer-based simulation updates
- **Math**: Trigonometric calculations for bearing/range
- **Memory**: Automatic Qt memory management

## Development Notes

- Uses Qt5 MOC (Meta-Object Compiler) for signals/slots
- C++11 features for modern syntax
- Modular design with separate widget class
- Extensible for additional tactical features


## Contact

Medha - 9494787039 
medha.rv@oceanautics.net
 
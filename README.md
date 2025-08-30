# TSA Screen - Tactical Situation Awareness Display

A Qt-based tactical display system for maritime/naval operations showing sensor coverage, target tracking, and vector analysis with advanced dynamic rendering.

## Overview

This application simulates a Tactical Situation Awareness (TSA) screen commonly used in naval operations. It displays:

- **Sensor Beam**: Green line from sensor to ship position (dynamic endpoint)
- **Dynamic Shaded Region**: Gray cross-hatched area with automatic gap calculation
- **Own Ship**: Yellow marker with cyan velocity vector (North at 10 knots)
- **Sensor Position**: Red marker on the sensor line
- **Adopted Track**: Red vector from sensor position (SW direction)
- **Bearing Rate**: Cyan vector showing rate of bearing change
- **Real-time Simulation**: Continuous updates with bearing/range calculations

## Latest Features

### Dynamic Gap Calculation
- **Automatic Clearance**: Shaded region automatically adjusts to avoid overlapping vectors
- **Vector Endpoint Tracking**: Monitors all vector endpoints for minimum distance calculation
- **Safety Margin**: 5-pixel buffer ensures clean visual separation
- **Real-time Updates**: Gap adjusts as vectors move during simulation

### Refined Beam Rendering
- **Ship-Terminated Beam**: Green beam stops exactly at ship position (not full-screen)
- **Off-screen Rendering**: Clean black background with transparent holes
- **Anti-aliasing**: Smooth visual rendering with Qt5 QPainter
- **Proper Layering**: Correct draw order for all visual elements

### Advanced Visual System
- **Cross-hatch Pattern**: Professional tactical display appearance
- **Transparent Punch-outs**: Clean holes around vector origins
- **Dynamic Half-space**: Shaded region on opposite side of ship
- **Vector Visibility**: All tactical vectors remain fully visible

## Features

- Real-time simulation with 2-second update intervals
- Dynamic target tracking with bearing/range calculations
- Advanced vector analysis for tactical decision making
- Intelligent sensor coverage visualization
- Smooth Qt-based rendering with anti-aliasing
- Automatic gap management for clean display

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
- **Advanced Simulation Engine**: Updates target position, bearing, and range every 2 seconds
- **Dynamic Drawing System**: Renders all visual elements with intelligent gap management
- **Vector Analysis**: Calculates and displays tactical vectors with endpoint tracking
- **Off-screen Rendering**: Uses QImage for clean background preservation

### Simulation Parameters
- **Own Ship**: Course 0° (North), Speed 10 knots, Depth 40m
- **Target**: Initial position (3,3) nm, Course 90°, Speed 8 knots
- **Update Rate**: 2-second intervals
- **Safety Margin**: 5 pixels for gap calculation

### Visual Elements
1. **Black Background**: Standard tactical display background
2. **Green Sensor Beam**: Active sensor coverage line (sensor to ship)
3. **Dynamic Hatched Area**: Inactive sensor region with automatic gap
4. **Yellow Own Ship**: Current vessel position with velocity vector
5. **Red Sensor Marker**: Sensor position on beam line
6. **Tactical Vectors**: Various colored arrows for analysis

## Technical Implementation

### Dynamic Gap Algorithm
```cpp
// Compute minimum distance from any endpoint to the sensor->ship segment
auto distanceToSegment = [&](const QPointF &P, const QPointF &A, const QPointF &B) {
    QPointF AB = B - A;
    QPointF AP = P - A;
    double abLen = std::hypot(AB.x(), AB.y());
    double cross = std::abs(AB.x()*AP.y() - AB.y()*AP.x());
    return cross / abLen;
};
```

### Off-screen Rendering
- Uses `QImage::Format_ARGB32_Premultiplied` for transparency support
- `CompositionMode_Clear` for punching out transparent holes
- Preserves black background in cleared areas

## Usage

The application runs automatically with continuous simulation updates. The display shows:

- Real-time bearing and range calculations in console output
- Visual representation of tactical situation with dynamic shading
- Vector analysis for course planning
- Automatic gap management for clean visual separation

## Technical Details

- **Rendering**: Qt5 QPainter with anti-aliasing and off-screen composition
- **Timing**: QTimer-based simulation updates (2-second intervals)
- **Math**: Trigonometric calculations for bearing/range and vector distances
- **Memory**: Automatic Qt memory management with RAII
- **Performance**: Efficient rendering with minimal redraws

## Development Notes

- Uses Qt5 MOC (Meta-Object Compiler) for signals/slots
- C++11 features for modern syntax and lambda functions
- Modular design with separate widget class
- Extensible for additional tactical features
- Version controlled with Git

## Recent Updates

- **Dynamic Gap Calculation**: Automatic clearance between shaded region and vectors
- **Refined Beam Rendering**: Ship-terminated beam with proper endpoints
- **Off-screen Rendering**: Clean background preservation with transparency
- **Vector Endpoint Tracking**: Intelligent gap calculation based on all vector endpoints
- **Safety Margin**: Configurable buffer for visual separation

## Contact

Medha - 9494787039 
medha.rv@oceanautics.net
 
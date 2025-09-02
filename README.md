# TSA Screen Widget - Tactical Situation Awareness Display

A professional-grade Qt5/C++ tactical display widget for maritime and military applications, featuring real-time sensor coverage visualization, tactical vectors, and responsive design.

## 🚢 Features

- **Real-time Tactical Display** - Live sensor coverage and target tracking
- **Responsive Design** - Automatically adapts to any window size
- **Professional Shading** - Half-plane coverage with configurable gaps
- **Tactical Vectors** - Ship heading, target tracks, and intercept courses
- **Clean API** - Easy integration into larger Qt applications
- **Simulation Engine** - Built-in time-based updates for testing

## 🏗️ Architecture

### Core Components

- **TSAWidget** - Main display widget with painting and event handling
- **DisplayTransform** - Coordinate transformation system
- **TacticalDisplay** - Data structure for tactical situation
- **TacticalVector** - Individual vector representation
- **SonarBeam** - Sensor coverage area definition

### Key Design Patterns

- **Observer Pattern** - Timer-based simulation updates
- **Strategy Pattern** - Configurable coordinate transformations
- **Factory Pattern** - Tactical vector creation and management

## 📦 Integration Guide

### Prerequisites

- Qt5 (Widgets, GUI, Core)
- C++11 compatible compiler
- CMake or qmake build system

### Basic Integration

```cpp
#include "diagramwidget.h"

// In your main window or widget
class MainWindow : public QMainWindow {
private:
    TSAWidget* tsaDisplay;
    
public:
    MainWindow() {
        // Create TSA widget
        tsaDisplay = new TSAWidget(this);
        setCentralWidget(tsaDisplay);
        
        // Configure display
        tsaDisplay->setWorldBounds(QRectF(-15, -15, 30, 30));
        tsaDisplay->setSimulationInterval(1000); // 1 second updates
        
        // Start simulation
        tsaDisplay->startSimulation();
    }
};
```

### Advanced Integration

```cpp
// Update tactical data in real-time
void updateTacticalSituation() {
    // Own ship updates
    tsaDisplay->updateOwnShip(45.0, 15.0); // 45° heading, 15 knots
    
    // Target updates
    tsaDisplay->updateTarget(90.0, 6.5, 0.08); // bearing, range, rate
    
    // Add tactical vectors
    QVector<TacticalVector> vectors;
    vectors.append(TacticalVector(
        QPointF(0, 0),           // Origin
        60.0,                     // Bearing
        8.0,                      // Magnitude
        VectorType::INTERCEPT_COURSE,
        Qt::blue                  // Color
    ));
    
    tsaDisplay->updateTacticalData(45.0, 15.0, vectors, 135.0, 0.05);
}
```

## 🔧 Configuration Options

### Display Settings

```cpp
// World coordinate bounds
tsaDisplay->setWorldBounds(QRectF(-20, -20, 40, 40));

// Simulation timing
tsaDisplay->setSimulationInterval(500);  // 500ms updates

// Transform settings
DisplayTransform& transform = tsaDisplay->getTransform();
transform.setWorldBounds(QRectF(-10, -10, 20, 20));
```

### Visual Customization

```cpp
// Custom colors and styles
struct TacticalVector customVector(
    QPointF(0, 0),              // Origin
    0.0,                         // Bearing
    10.0,                        // Magnitude
    VectorType::OWN_SHIP,        // Type
    QColor(255, 165, 0),        // Orange color
    4,                           // Line width
    15.0,                        // Head length
    30.0                         // Head angle
);

tsaDisplay->addTacticalVector(customVector);
```

## 📱 Responsive Design

### Automatic Resizing

The widget automatically handles resize events and maintains proper proportions:

- **Coordinate Transformation** - Updates on every resize event
- **Aspect Ratio Preservation** - Maintains tactical display proportions
- **Dynamic Shading** - Shaded region adapts to new dimensions
- **Vector Scaling** - Tactical elements scale appropriately

### Resize Event Handling

```cpp
void TSAWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    
    // Update transform for new size
    transform.updateTransform(event->size(), transform.getWorldBounds(), true);
}
```

## 🧪 Testing & Simulation

### Built-in Simulation

```cpp
// Start/stop simulation
tsaDisplay->startSimulation();
tsaDisplay->stopSimulation();

// Custom simulation intervals
tsaDisplay->setSimulationInterval(2000); // 2 second updates
```

### Simulation Data

The widget includes a demo simulation that:
- Updates target position over time
- Calculates bearing and range changes
- Demonstrates real-time tactical updates
- Shows bearing rate calculations

## 🚀 Performance Considerations

### Optimization Features

- **Efficient Painting** - Minimal redraws with smart update triggers
- **Memory Management** - RAII-compliant resource handling
- **Vector Operations** - Optimized geometric calculations
- **Timer Management** - Efficient simulation timing

### Memory Usage

- **Widget**: ~2-5 MB typical
- **Vectors**: ~100 bytes per tactical vector
- **Transformations**: Minimal overhead
- **Simulation**: Negligible memory footprint

## 🔌 API Reference

### Core Methods

```cpp
// Data updates
void updateOwnShip(double bearing, double speed);
void updateTarget(double bearing, double range, double bearingRate);
void updateTacticalData(double ownShipBearing, double ownShipSpeed,
                        const QVector<TacticalVector>& targetVectors,
                        double sonarBearing, double bearingRate);

// Vector management
void addTacticalVector(const TacticalVector& vector);
void clearTacticalVectors();

// Configuration
void setWorldBounds(QRectF bounds);
void setSimulationInterval(int milliseconds);
void startSimulation();
void stopSimulation();
```

### Data Structures

```cpp
struct TacticalVector {
    QPointF origin;         // Vector origin point
    double bearing;          // Bearing in degrees
    double magnitude;        // Magnitude in nautical miles
    VectorType type;         // Vector classification
    QColor color;            // Display color
    int lineWidth;           // Line thickness
    double headLength;       // Arrow head length
    double headAngle;        // Arrow head angle
};

enum class VectorType {
    OWN_SHIP,           // Own ship movement
    TARGET,             // Target track
    SONAR_BEARING,      // Sonar beam
    ADOPTED_TRACK,      // Adopted target track
    INTERCEPT_COURSE,   // Intercept course
    COLLISION_AVOIDANCE // Collision avoidance
};
```

## 🐛 Troubleshooting

### Common Issues

1. **Shaded Region Not Filling**
   - Check world bounds configuration
   - Verify coordinate transformation setup

2. **Vectors Not Displaying**
   - Ensure vector data is valid
   - Check coordinate system alignment

3. **Performance Issues**
   - Reduce simulation frequency
   - Limit number of tactical vectors
   - Check for excessive redraws

### Debug Output

Enable debug output for troubleshooting:

```cpp
// In your main function
QLoggingCategory::setFilterRules("*.debug=true");

// Widget will output simulation updates
qDebug() << "Time:" << simulationTime
         << "Bearing:" << newBearing
         << "Range:" << newRange;
```

## 📄 License

This module is provided as-is for integration into larger projects. Ensure compliance with your project's licensing requirements.

## 🤝 Contributing

When integrating this module:

1. **Maintain API Compatibility** - Don't break existing interfaces
2. **Follow Qt Conventions** - Use Qt naming and design patterns
3. **Add Error Handling** - Implement proper error checking
4. **Document Changes** - Update this README for modifications

## 🔗 Dependencies

- **Qt5 Core** - Base functionality
- **Qt5 Widgets** - UI components
- **Qt5 GUI** - Painting and graphics
- **C++11** - Modern C++ features

## 📊 System Requirements

- **OS**: Linux, Windows, macOS
- **Qt**: 5.12 or higher
- **Compiler**: GCC 4.8+, Clang 3.3+, MSVC 2015+
- **Memory**: 4 MB minimum
- **Display**: 800x600 minimum resolution

---

**Note**: This module is designed for integration into larger tactical systems and provides a solid foundation for maritime and military display applications.
 
#include <QApplication>
#include <QTimer>
#include <QDebug>
#include "diagramwidget.h"

/**
 * @brief Main entry point for TSA Screen application
 * 
 * Demonstrates the refactored TSA Widget with clean API for tactical data updates.
 * The new architecture provides responsive design and unified data interface.
 * 
 * @param argc Command line argument count
 * @param argv Command line arguments array
 * @return Application exit code
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Create the refactored TSA Widget
    TSAWidget* widget = new TSAWidget();
    widget->resize(800, 600);
    widget->setWindowTitle("TSA Screen - Refactored Architecture");
    widget->show();
    
    // Demonstrate the new clean API
    QTimer::singleShot(1000, [widget]() {
        qDebug() << "=== Demonstrating Refactored TSA Widget API ===";
        
        // Example 1: Update own ship data
        widget->updateOwnShip(45.0, 15.0);  // 45° heading, 15 knots
        qDebug() << "Updated own ship: 45° heading, 15 knots";
        
        // Example 2: Update target data
        widget->updateTarget(90.0, 6.5, 0.08);  // 90° bearing, 6.5 nm, 0.08°/s rate
        qDebug() << "Updated target: 90° bearing, 6.5 nm, 0.08°/s rate";
        
        // Example 3: Update sonar beam
        widget->updateSonarBeam(135.0, 3.0);  // 135° bearing, 3° width
        qDebug() << "Updated sonar beam: 135° bearing, 3° width";
        
        // Example 4: Add new tactical vector
        TacticalVector interceptVector(
            QPointF(0, 0),                    // Origin (own ship)
            60.0,                             // Bearing (60°)
            8.0,                              // Magnitude (8 nm)
            VectorType::INTERCEPT_COURSE,     // Type
            Qt::blue,                         // Color (blue for intercept course)
            2,                                // Line width
            15.0,                             // Head length
            30.0                              // Head angle
        );
        widget->addTacticalVector(interceptVector);
        qDebug() << "Added intercept course vector: 60° bearing, 8 nm, blue";
        
        // Example 5: Update world bounds for different zoom levels
        widget->setWorldBounds(QRectF(-15, -15, 30, 30));  // Zoom out
        qDebug() << "Updated world bounds: zoomed out to ±15 nm";
        
        // Example 6: Change simulation interval
        widget->setSimulationInterval(1000);  // 1 second updates
        qDebug() << "Changed simulation interval to 1 second";
        
        qDebug() << "=== API demonstration complete ===";
    });
    
    return app.exec();
} 
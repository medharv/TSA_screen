#ifndef DIAGRAMWIDGET_H
#define DIAGRAMWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QTransform>
#include <QVector>
#include <QPointF>
#include <QSize>
#include <QColor>

/**
 * @brief Tactical vector types for maritime display
 */
enum class VectorType {
    OWN_SHIP,           // Own ship movement vector
    TARGET,             // Target track vector
    SONAR_BEARING,      // Sonar beam direction
    ADOPTED_TRACK,      // Adopted target track
    INTERCEPT_COURSE,   // Intercept course vector
    COLLISION_AVOIDANCE // Collision avoidance vector
};

/**
 * @brief Tactical vector data structure
 */
struct TacticalVector {
    QPointF origin;         // Vector origin point (world coordinates)
    double bearing;          // Vector bearing in degrees
    double magnitude;        // Vector magnitude in nautical miles
    VectorType type;         // Vector type classification
    QColor color;            // Display color
    int lineWidth;           // Line thickness
    double headLength;       // Arrow head length
    double headAngle;        // Arrow head angle in degrees
    
    TacticalVector(QPointF o, double b, double m, VectorType t, 
                   QColor c = Qt::white, int w = 2, 
                   double hl = 12.0, double ha = 25.0)
        : origin(o), bearing(b), magnitude(m), type(t), 
          color(c), lineWidth(w), headLength(hl), headAngle(ha) {}
};

/**
 * @brief Sonar beam coverage data
 */
struct SonarBeam {
    QPointF startPoint;      // Beam start position
    QPointF endPoint;        // Beam end position
    double width;             // Beam width in degrees
    QColor color;            // Beam color
    int lineWidth;           // Line thickness
    
    SonarBeam(QPointF start, QPointF end, double w = 2.0, 
               QColor c = Qt::green, int lw = 4)
        : startPoint(start), endPoint(end), width(w), color(c), lineWidth(lw) {}
};

/**
 * @brief Complete tactical situation package
 */
struct TacticalDisplay {
    QPointF ownShipPosition;     // Own ship position (world coordinates)
    double ownShipBearing;       // Own ship heading in degrees
    double ownShipSpeed;         // Own ship speed in knots
    QVector<TacticalVector> vectors; // All tactical vectors
    SonarBeam sonarBeam;         // Sonar coverage area
    double targetBearing;        // Target bearing in degrees
    double targetRange;          // Target range in nautical miles
    double bearingRate;          // Bearing rate in degrees/second
    
    TacticalDisplay() : ownShipBearing(0.0), ownShipSpeed(10.0), 
                        targetBearing(45.0), targetRange(4.0), bearingRate(0.0),
                        sonarBeam(QPointF(0,0), QPointF(0,0), 2.0, Qt::green, 4) {}
};

/**
 * @brief Coordinate transformation system
 */
class DisplayTransform {
private:
    QTransform worldToScreen;    // World to screen transformation
    QTransform screenToWorld;    // Screen to world transformation
    QRectF worldBounds;          // World coordinate bounds
    QSize screenSize;            // Screen size in pixels
    
public:
    DisplayTransform() : worldBounds(-10, -10, 20, 20), screenSize(800, 600) {
        updateTransform(QSize(800, 600), QRectF(-10, -10, 20, 20), true);
    }
    
    void updateTransform(QSize widgetSize, QRectF bounds, bool maintainAspectRatio = true);
    QPointF mapToScreen(QPointF worldPoint) const;
    QPointF mapToWorld(QPointF screenPoint) const;
    double mapDistanceToScreen(double worldDistance) const;
    double mapDistanceToWorld(double screenDistance) const;
    QRectF getWorldBounds() const { return worldBounds; }
    void setWorldBounds(QRectF bounds) { 
        worldBounds = bounds; 
        updateTransform(screenSize, bounds, true); 
    }
};

/**
 * @brief Refactored TSA Widget with clean API and responsive design
 */
class TSAWidget : public QWidget {
    Q_OBJECT

private:
    QTimer* timer;                    // Simulation update timer
    TacticalDisplay tacticalData;     // Current tactical situation
    DisplayTransform transform;        // Coordinate transformation
    double simulationTime;             // Current simulation time
    
    // Drawing functions
    void drawTacticalVector(QPainter& p, const TacticalVector& vector);
    void drawSonarBeam(QPainter& p, const SonarBeam& beam);
    void drawOwnShip(QPainter& p, const QPointF& position);
    void drawTarget(QPainter& p, const QPointF& position);
    void drawBackground(QPainter& p);
    void drawCoordinateGrid(QPainter& p);
    
    // Helper functions
    QPointF calculateVectorEnd(const TacticalVector& vector) const;
    QPointF getOwnShipScreenPosition() const;
    QPointF getTargetScreenPosition() const;
    
    // Original display methods
    void drawOneSidedShadedRegion(QPainter& p, QPointF lineStart, QPointF lineEnd, QPointF shipPos);
    void drawSimpleArrow(QPainter& p, QPointF from, QPointF to, QColor color, int width);
    
private slots:
    void updateSimulation();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

public:
    explicit TSAWidget(QWidget* parent = nullptr);
    ~TSAWidget();
    
    // Clean API for data updates
    void updateTacticalData(double ownShipBearing, double ownShipSpeed,
                           const QVector<TacticalVector>& targetVectors,
                           double sonarBearing, double bearingRate);
    
    // Individual data update methods
    void updateOwnShip(double bearing, double speed);
    void updateTarget(double bearing, double range, double bearingRate);
    void updateSonarBeam(double bearing, double width);
    void addTacticalVector(const TacticalVector& vector);
    void clearTacticalVectors();
    
    // Configuration methods
    void setWorldBounds(QRectF bounds);
    void setSimulationInterval(int milliseconds);
    void startSimulation();
    void stopSimulation();
    
    // Getters for current state
    TacticalDisplay getCurrentTacticalData() const { return tacticalData; }
    DisplayTransform getTransform() const { return transform; }
};

#endif // DIAGRAMWIDGET_H 
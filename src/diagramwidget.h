#ifndef TSAWIDGET_H
#define TSAWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPointF>
#include <QPolygonF>
#include <QRectF>
#include <QColor>
#include <QVector>
#include <QtMath>

/**
 * @brief TSAWidget - Tactical Situation Awareness Display Widget
 * 
 * This widget provides a real-time tactical display for maritime operations,
 * showing sensor coverage, target tracking, and vector analysis. It simulates
 * a naval tactical situation with own ship, target, and various tactical vectors.
 * 
 * Key Features:
 * - Real-time simulation with configurable update intervals
 * - Visual sensor beam and coverage area
 * - Dynamic target tracking with bearing/range calculations
 * - Multiple tactical vectors for analysis
 * - Cross-hatched inactive sensor regions
 */
class TSAWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the TSA display widget
     * @param parent Parent widget (optional)
     */
    explicit TSAWidget(QWidget *parent = nullptr);

protected:
    /**
     * @brief Qt paint event handler - renders the tactical display
     * @param event Paint event information
     */
    void paintEvent(QPaintEvent *event) override;

private slots:
    /**
     * @brief Updates simulation state every timer interval
     * 
     * Called every 2 seconds to update target position, bearing, range,
     * and bearing rate calculations. Triggers widget repaint.
     */
    void updateSimulation();

private:
    // ===== DRAWING HELPER METHODS =====
    
    /**
     * @brief Draws an arrow with specified parameters
     * @param p QPainter reference for drawing
     * @param from Starting point of arrow
     * @param to Ending point of arrow
     * @param headLen Length of arrow head
     * @param headAngleDeg Angle of arrow head in degrees
     * @param color Arrow color
     * @param width Arrow line width
     */
    void drawArrow(QPainter &p, const QPointF &from, const QPointF &to,
                   qreal headLen, qreal headAngleDeg, const QColor &color, int width);
    
    /**
     * @brief Clip the half-space on the sideSelected side of line A→B to the rect
     * @param A First point of the line
     * @param B Second point of the line
     * @param bounds Widget rectangle bounds
     * @param sideSelectedIsLeft Whether the selected side is left of the line
     * @return Polygon representing the clipped half-space
     */
    QPolygonF buildHalfSpacePoly(const QPointF &A, const QPointF &B,
                                 const QRectF &bounds, bool sideSelectedIsLeft);
    
    /**
     * @brief Builds a convex hull from a set of points using Graham scan
     * @param points Input points
     * @return Convex hull polygon
     */
    QPolygonF buildConvexHull(const QVector<QPointF> &points);
    
    /**
     * @brief Gets the current own ship position on display
     * @return QPointF representing ship position in widget coordinates
     */
    QPointF getShipPosition() const;
    
    /**
     * @brief Gets the current sensor position on the display
     * @return QPointF representing sensor position in widget coordinates
     */
    QPointF getSensorPosition() const;
    
    /**
     * @brief Gets a fixed reference point for consistent shading calculations
     * @return QPointF representing a fixed ship reference position
     */
    QPointF getFixedShipReference() const;
    
    /**
     * @brief Gets a fixed reference point for consistent sensor calculations
     * @return QPointF representing a fixed sensor reference position
     */
    QPointF getFixedSensorReference() const;

    // ===== SIMULATION LOGIC METHODS =====
    
    /**
     * @brief Calculates new target position based on time and movement
     * 
     * Updates target_x, target_y, current_bearing, and current_range
     * based on target course/speed and own ship movement.
     */
    void calculateTargetPosition();
    
    /**
     * @brief Calculates range from origin to given coordinates
     * @param x X coordinate (nautical miles)
     * @param y Y coordinate (nautical miles)
     * @return Range in nautical miles
     */
    double calculateRange(double x, double y) const;
    
    /**
     * @brief Calculates bearing from origin to given coordinates
     * @param x X coordinate (nautical miles)
     * @param y Y coordinate (nautical miles)
     * @return Bearing in degrees (0-360°)
     */
    double calculateBearing(double x, double y) const;

    // ===== MEMBER VARIABLES =====
    
    QTimer *timer;                    ///< Timer for simulation updates
    double current_time_sec;          ///< Current simulation time in seconds
    double prev_bearing;              ///< Previous bearing for rate calculation
    double current_bearing;           ///< Current target bearing in degrees
    double current_range;             ///< Current target range in nautical miles
    double current_bearing_rate;      ///< Current bearing rate in degrees/second

    // ===== OWN-SHIP FIXED PARAMETERS =====
    const double C_own = 0.0;         ///< Own ship course over ground (degrees)
    const double S_own = 10.0;        ///< Own ship speed over ground (knots)
    const double depth_own = 40.0;    ///< Own ship depth (meters)

    // ===== TARGET SIMULATION PARAMETERS =====
    double target_course;             ///< Target's course over ground (degrees)
    double target_speed;              ///< Target's speed over ground (knots)
    double target_x;                  ///< Target X position (nautical miles)
    double target_y;                  ///< Target Y position (nautical miles)

    // ===== DISPLAY GEOMETRY =====
    QPointF sensor_line_start;        ///< Start point of sensor beam line
    QPointF sensor_line_end;          ///< End point of sensor beam line
};

#endif // TSAWIDGET_H 
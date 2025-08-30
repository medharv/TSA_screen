#include "diagramwidget.h"
#include <QPainter>
#include <QDebug>

/**
 * @brief Constructor - Initializes the TSA display widget
 * 
 * Sets up initial simulation parameters, calculates starting positions,
 * and configures the update timer for continuous simulation.
 * 
 * @param parent Parent widget (optional)
 */
TSAWidget::TSAWidget(QWidget *parent)
    : QWidget(parent),
      timer(new QTimer(this)),
      current_time_sec(0.0),
      prev_bearing(0.0),
      current_bearing(45.0),
      current_range(0.0),
      current_bearing_rate(0.0),
      target_course(90.0),      // Target heading East
      target_speed(8.0),        // Target speed 8 knots
      target_x(3.0),            // Initial target X position (nm)
      target_y(3.0),            // Initial target Y position (nm)
      sensor_line_start(80, 480),   // Sensor beam start point
      sensor_line_end(720, 80)      // Sensor beam end point
{
    // Calculate initial target position relative to own ship
    current_range   = calculateRange(target_x, target_y);
    current_bearing = calculateBearing(target_x, target_y);
    prev_bearing    = current_bearing;

    // Set up timer for simulation updates (every 2 seconds)
    connect(timer, &QTimer::timeout, this, &TSAWidget::updateSimulation);
    timer->start(2000);  // 2000ms = 2 seconds
}

/**
 * @brief Simulation update slot - called every timer interval
 * 
 * Updates target position, recalculates bearing/range/rate,
 * and triggers widget repaint. This is the main simulation loop.
 */
void TSAWidget::updateSimulation()
{
    // Store previous bearing for rate calculation
    prev_bearing = current_bearing;
    
    // Advance simulation time
    current_time_sec += 2.0;

    // Calculate new target position and update measurements
    calculateTargetPosition();
    
    // Calculate bearing rate (degrees per second)
    current_bearing_rate = (current_bearing - prev_bearing) / 2.0;

    // Normalize bearing rate to handle 360° wrap-around
    if (current_bearing_rate > 180.0)  current_bearing_rate -= 360.0;
    if (current_bearing_rate < -180.0) current_bearing_rate += 360.0;

    // Debug output for monitoring simulation
    qDebug() << "Time:" << current_time_sec
             << "Bearing:" << current_bearing
             << "Range:" << current_range
             << "Rate:"  << current_bearing_rate;

    // Trigger widget repaint to show updated display
    update();
}

/**
 * @brief Calculates new target position based on movement over time
 * 
 * Simulates both own ship movement (North at 10 knots) and target movement
 * (East at 8 knots), then calculates relative position and measurements.
 */
void TSAWidget::calculateTargetPosition()
{
    double t = current_time_sec / 3600.0; // Convert seconds to hours
    
    // Own ship movement (heading North at 10 knots)
    double own_x = 0.0;
    double own_y = S_own * t;  // Northward movement

    // Target movement (heading East at 8 knots)
    double dx = target_speed * qSin(qDegreesToRadians(target_course)) * t;
    double dy = target_speed * qCos(qDegreesToRadians(target_course)) * t;
    double x  = 3.0 + dx;  // Initial X + movement
    double y  = 3.0 + dy;  // Initial Y + movement

    // Calculate relative position (target position minus own ship position)
    double rel_x = x - own_x;
    double rel_y = y - own_y;
    
    // Update current measurements
    current_range   = calculateRange(rel_x, rel_y);
    current_bearing = calculateBearing(rel_x, rel_y);
}

/**
 * @brief Calculates range (distance) from origin to given coordinates
 * @param x X coordinate in nautical miles
 * @param y Y coordinate in nautical miles
 * @return Range in nautical miles
 */
double TSAWidget::calculateRange(double x, double y) const
{
    return qSqrt(x*x + y*y);  // Pythagorean theorem
}

/**
 * @brief Calculates bearing (direction) from origin to given coordinates
 * @param x X coordinate in nautical miles
 * @param y Y coordinate in nautical miles
 * @return Bearing in degrees (0-360°)
 */
double TSAWidget::calculateBearing(double x, double y) const
{
    double b = qRadiansToDegrees(qAtan2(x, y));
    return (b < 0.0 ? b + 360.0 : b);  // Normalize to 0-360°
}

/**
 * @brief Gets own ship position on the display
 * @return QPointF representing ship position in widget coordinates
 */
QPointF TSAWidget::getShipPosition() const
{
    return sensor_line_start + 0.75 * (sensor_line_end - sensor_line_start);
}

/**
 * @brief Gets sensor position on the display
 * @return QPointF representing sensor position in widget coordinates
 */
QPointF TSAWidget::getSensorPosition() const
{
    return sensor_line_start + 0.45 * (sensor_line_end - sensor_line_start);
}

/**
 * @brief Draws an arrow with specified parameters
 * 
 * Draws a line with arrowhead at the end, useful for displaying
 * velocity vectors and tactical directions.
 * 
 * @param p QPainter reference for drawing
 * @param from Starting point of arrow
 * @param to Ending point of arrow
 * @param headLen Length of arrow head
 * @param headAngleDeg Angle of arrow head in degrees
 * @param color Arrow color
 * @param width Arrow line width
 */
void TSAWidget::drawArrow(QPainter &p, const QPointF &from, const QPointF &to,
                          qreal headLen, qreal headAngleDeg,
                          const QColor &color, int width)
{
    // Draw the main arrow shaft
    p.setPen(QPen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawLine(from, to);

    // Calculate arrow head points
    qreal angle = qAtan2(to.y() - from.y(), to.x() - from.x());
    qreal a1 = angle + qDegreesToRadians(180.0 - headAngleDeg);
    qreal a2 = angle - qDegreesToRadians(180.0 - headAngleDeg);

    QPointF h1(to.x() + headLen * qCos(a1), to.y() + headLen * qSin(a1));
    QPointF h2(to.x() + headLen * qCos(a2), to.y() + headLen * qSin(a2));
    
    // Draw arrow head as filled polygon
    QPolygonF head; head << to << h1 << h2;
    p.setBrush(color);
    p.drawPolygon(head);
}

/**
 * @brief Helper function to determine which side of a line a point lies on
 * @param A First point of the line
 * @param B Second point of the line  
 * @param P Point to test
 * @return Positive value if P is on "left" side, negative if on "right" side
 */
static qreal sideOfLine(const QPointF &A, const QPointF &B, const QPointF &P) {
    // cross((B–A),(P–A))
    return (B.x()-A.x())*(P.y()-A.y()) - (B.y()-A.y())*(P.x()-A.x());
}

/**
 * @brief Draws the cross-hatched inactive sensor region
 * 
 * Creates a gray cross-hatched area on the opposite side of the sensor beam
 * from the ship position, using a bounding box approach for clean clipping.
 * 
 * @param p QPainter reference for drawing
 */
void TSAWidget::drawHatchedArea(QPainter &p)
{
    QPointF P1 = sensor_line_start;
    QPointF P2 = sensor_line_end;
    QPointF shipPos = getShipPosition();

    // 1. Determine which side is SHIP side
    bool shipOnLeft = sideOfLine(P1, P2, shipPos) > 0;

    // 2. Build AABB around beam
    qreal minX = qMin(P1.x(), P2.x());
    qreal maxX = qMax(P1.x(), P2.x());
    qreal minY = qMin(P1.y(), P2.y());
    qreal maxY = qMax(P1.y(), P2.y());

    // 3. Compute margin = 20% of max dimension
    qreal w = maxX - minX;
    qreal h = maxY - minY;
    qreal m = 0.2 * qMax(w, h);

    // 4. Expand box
    minX -= m; maxX += m;
    minY -= m; maxY += m;

    // 5. Collect box corners
    QVector<QPointF> corners = {
        {minX, minY}, {maxX, minY},
        {maxX, maxY}, {minX, maxY}
    };

    // 6. Keep only corners on SHADED side (opposite of ship)
    QVector<QPointF> polyPts;
    for (auto &pt : corners) {
        bool left = sideOfLine(P1, P2, pt) > 0;
        if (left != shipOnLeft)
            polyPts.append(pt);
    }

    // 7. Always include the beam endpoints to close the polygon
    polyPts.append(P2);
    polyPts.append(P1);

    // 8. Draw hatched area
    QPolygonF hatched(polyPts);
    QBrush hatch(QColor(80,80,80,150), Qt::BDiagPattern);
    p.setBrush(hatch);
    p.setPen(Qt::NoPen);
    p.drawPolygon(hatched);
}

/**
 * @brief Main paint event - renders the complete tactical display
 * 
 * This method draws all visual elements in the correct order:
 * 1. Black background
 * 2. Green sensor beam line
 * 3. Gray hatched inactive region
 * 4. Ship and sensor markers
 * 5. Various tactical vectors
 * 
 * @param event Paint event information (unused)
 */
void TSAWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);  // Smooth rendering

    // 1. Draw black background (standard tactical display)
    p.fillRect(rect(), Qt::black);

    // 2. Draw green sensor beam line
    p.setPen(QPen(Qt::green, 4, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(sensor_line_start, sensor_line_end);

    // 3. Draw hatched region BELOW beam (inactive sensor area)
    drawHatchedArea(p);

    // 4. Draw ship and sensor position markers
    QPointF shipPos   = getShipPosition();
    QPointF sensorPos = getSensorPosition();
    
    // Own ship marker (yellow circle)
    p.setBrush(Qt::yellow);
    p.drawEllipse(shipPos, 6, 6);
    
    // Sensor marker (red circle)
    p.setBrush(Qt::red);
    p.drawEllipse(sensorPos, 6, 6);

    // 5. Draw tactical vectors for analysis
    
    // Own ship velocity vector (CYAN) - heading North at 10 knots
    QPointF ownEnd = shipPos + QPointF(
        S_own * 6.0 * qSin(qDegreesToRadians(C_own)),
       -S_own * 6.0 * qCos(qDegreesToRadians(C_own))
    );
    drawArrow(p, shipPos, ownEnd, 12, 25, Qt::cyan, 3);

    // Trial track vector (YELLOW) - potential course change (+20° from current)
    double trialAng = C_own + 20.0;
    QPointF trialEnd = shipPos + QPointF(
        S_own * 5.5 * qSin(qDegreesToRadians(trialAng)),
       -S_own * 5.5 * qCos(qDegreesToRadians(trialAng))
    );
    drawArrow(p, shipPos, trialEnd, 12, 25, Qt::yellow, 3);

    // Adopted track vector (RED) - tactical direction from sensor
    double adoptedLen = 80.0;
    double adoptedAng = 225.0; // Southwest direction
    QPointF adoptedEnd = sensorPos + QPointF(
        adoptedLen * qSin(qDegreesToRadians(adoptedAng)),
       -adoptedLen * qCos(qDegreesToRadians(adoptedAng))
    );
    drawArrow(p, sensorPos, adoptedEnd, 12, 25, Qt::red, 3);

    // Bearing rate orientation vector (CYAN) - shows rate of bearing change
    double rateLen = 45.0;
    double rateAng = 180.0; // South direction
    QPointF rateStart = sensorPos + QPointF(25, 25);
    QPointF rateEnd   = rateStart + QPointF(
        rateLen * qSin(qDegreesToRadians(rateAng)),
       -rateLen * qCos(qDegreesToRadians(rateAng))
    );
    drawArrow(p, rateStart, rateEnd, 8, 30, Qt::cyan, 2);
} 
#include "diagramwidget.h"
#include <QPainter>
#include <QPainterPath>
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
 * @brief Returns the two points where line through A→B intersects the widget rectangle
 * @param A First point of the line
 * @param B Second point of the line
 * @param rect Widget rectangle bounds
 * @return Pair of intersection points spanning the full widget
 */
QPair<QPointF,QPointF> computeFullLine(const QPointF &A, const QPointF &B, const QRectF &rect)
{
    // Line: parametric P(t)=A+t*(B–A). Compute intersections with each of the four edges.
    QVector<QPointF> hits;
    QPointF d = B - A;

    auto intersect = [&](double x, double yMin, double yMax, bool vertical) {
        // if vertical edge: x=x0, solve t = (x0–Ax)/dx, then y=Ay+t*dy must lie between yMin,yMax
        // if horizontal edge: same logic swapping axes
        double t = vertical
            ? (x - A.x()) / d.x()
            : (x - A.y()) / d.y();
        QPointF P = A + t * d;
        double y = vertical ? P.y() : P.x();
        if (vertical ? (y >= yMin && y <= yMax)
                     : (y >= yMin && y <= yMax))
            hits.append(P);
    };

    // Left edge (x=rect.left())
    if (!qFuzzyIsNull(d.x()))
        intersect(rect.left(), rect.top(), rect.bottom(), /*vertical*/true);
    // Right edge
    if (!qFuzzyIsNull(d.x()))
        intersect(rect.right(), rect.top(), rect.bottom(), true);
    // Top edge (y=rect.top())
    if (!qFuzzyIsNull(d.y()))
        intersect(rect.top(), rect.left(), rect.right(), /*vertical*/false);
    // Bottom edge
    if (!qFuzzyIsNull(d.y()))
        intersect(rect.bottom(), rect.left(), rect.right(), false);

    // Keep only two unique intersection points (simple approach)
    QVector<QPointF> pts;
    for (const auto &hit : hits) {
        bool found = false;
        for (const auto &pt : pts) {
            if (qAbs(hit.x() - pt.x()) < 1e-6 && qAbs(hit.y() - pt.y()) < 1e-6) {
                found = true;
                break;
            }
        }
        if (!found) {
            pts.append(hit);
            if (pts.size() >= 2) break;
        }
    }
    
    if (pts.size() >= 2)
        return { pts[0], pts[1] };
    return { A, B }; // fallback
}

/**
 * @brief Clip the half-space on the sideSelected side of line A→B to the rect
 * @param A First point of the line
 * @param B Second point of the line
 * @param bounds Widget rectangle bounds
 * @param sideSelectedIsLeft Whether the selected side is left of the line
 * @return Polygon representing the clipped half-space
 */
QPolygonF TSAWidget::buildHalfSpacePoly(
    const QPointF &A, const QPointF &B,
    const QRectF &bounds,
    bool sideSelectedIsLeft)
{
    // Collect rectangle corners
    QVector<QPointF> pts = {
        bounds.topLeft(), bounds.topRight(),
        bounds.bottomRight(), bounds.bottomLeft()
    };
    // Keep corners on selected side
    QPolygonF poly;
    for (auto &pt : pts) {
        bool left = sideOfLine(A, B, pt) > 0;
        if (left == sideSelectedIsLeft)
            poly.append(pt);
    }
    // Add beam-rect intersections
    auto inte = computeFullLine(A, B, bounds);
    for (auto &pt : {inte.first, inte.second}) {
        bool left = sideOfLine(A, B, pt) > 0;
        if (left == sideSelectedIsLeft)
            poly.append(pt);
    }
    // Return a convex hull to ensure correct winding
    return buildConvexHull(QVector<QPointF>(poly.begin(), poly.end()));
}

/**
 * @brief Builds a convex hull from a set of points using Graham scan
 * @param points Input points
 * @return Convex hull polygon
 */
QPolygonF TSAWidget::buildConvexHull(const QVector<QPointF> &points)
{
    if (points.size() < 3) {
        return QPolygonF(points);
    }
    
    // Find the point with lowest y-coordinate (and leftmost if tied)
    int lowest = 0;
    for (int i = 1; i < points.size(); ++i) {
        if (points[i].y() < points[lowest].y() || 
            (points[i].y() == points[lowest].y() && points[i].x() < points[lowest].x())) {
            lowest = i;
        }
    }
    
    // Sort points by polar angle with respect to lowest point
    QVector<QPointF> sorted = points;
    std::swap(sorted[0], sorted[lowest]);
    
    // Sort remaining points by polar angle
    std::sort(sorted.begin() + 1, sorted.end(), [&](const QPointF &a, const QPointF &b) {
        double angleA = qAtan2(a.y() - sorted[0].y(), a.x() - sorted[0].x());
        double angleB = qAtan2(b.y() - sorted[0].y(), b.x() - sorted[0].x());
        return angleA < angleB;
    });
    
    // Graham scan
    QVector<QPointF> hull;
    hull.push_back(sorted[0]);
    hull.push_back(sorted[1]);
    
    for (int i = 2; i < sorted.size(); ++i) {
        while (hull.size() > 1 && 
               sideOfLine(hull[hull.size()-2], hull[hull.size()-1], sorted[i]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(sorted[i]);
    }
    
    return QPolygonF(hull);
}

/**
 * @brief Main paint event - renders the complete tactical display
 * 
 * This method draws all visual elements in the correct order:
 * 1. Black background
 * 2. Full far-side half-space with hatch
 * 3. Punch out clear corridor along beam
 * 4. Punch out circles around vector origins
 * 5. Draw beam and vectors on top
 * 
 * @param event Paint event information (unused)
 */
void TSAWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    
    // 1) Fill background
    p.fillRect(rect(), Qt::black);

    // 2) Get static positions
    QPointF sensorPos = getSensorPosition();
    QPointF shipPos = getShipPosition();
    
    // 3) Draw green bearing line (sensor to ship)
    p.setPen(QPen(Qt::green, 4, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(sensorPos, shipPos);

    // 4) SIMPLE shaded region: extend line to screen edges, shade one half
    auto full = computeFullLine(sensorPos, shipPos, rect());
    QPointF P1 = full.first, P2 = full.second;
    
    // 5) Build simple half-space polygon (just the widget corners on one side)
    QPolygonF half;
    bool shadeRight = true; // Always shade right side for now
    
    if (shadeRight) {
        // Add points clockwise: P1, P2, bottom-right, top-right
        half << P1 << P2 << rect().bottomRight() << rect().topRight();
    } else {
        // Add points clockwise: P1, P2, top-left, bottom-left
        half << P1 << P2 << rect().topLeft() << rect().bottomLeft();
    }
    
    // 6) Draw hatch
    p.setBrush(QBrush(QColor(80,80,80,150), Qt::BDiagPattern));
    p.setPen(Qt::NoPen);
    p.drawPolygon(half);

    // 7) Draw markers
    p.setBrush(Qt::yellow); p.setPen(Qt::NoPen);
    p.drawEllipse(shipPos, 6, 6);
    p.setBrush(Qt::red);
    p.drawEllipse(sensorPos, 6, 6);

    // 8) Own ship vector (from ship, perpendicular to bearing line)
    QPointF ownEnd = shipPos + QPointF(
        S_own*6*qSin(qDegreesToRadians(C_own)),
       -S_own*6*qCos(qDegreesToRadians(C_own))
    );
    drawArrow(p, shipPos, ownEnd, 12, 25, Qt::cyan, 3);

    // 9) Red vector (from sensor, perpendicular to bearing line)
    QPointF adoptedEnd = sensorPos + QPointF(
        80*qSin(qDegreesToRadians(225.0)),
       -80*qCos(qDegreesToRadians(225.0))
    );
    drawArrow(p, sensorPos, adoptedEnd, 12, 25, Qt::red, 3);
} 
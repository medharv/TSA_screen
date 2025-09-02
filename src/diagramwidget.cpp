#include "diagramwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QtMath>
#include <QResizeEvent>

// ===== DISPLAY TRANSFORM IMPLEMENTATION =====

void DisplayTransform::updateTransform(QSize widgetSize, QRectF bounds, bool maintainAspectRatio) {
    screenSize = widgetSize;
    worldBounds = bounds;
    
    if (maintainAspectRatio) {
        // Calculate scale factors for both dimensions
        double scaleX = screenSize.width() / worldBounds.width();
        double scaleY = screenSize.height() / worldBounds.height();
        
        // Use the smaller scale to maintain aspect ratio
        double scale = qMin(scaleX, scaleY);
        
        // Calculate centering offsets
        double offsetX = (screenSize.width() - worldBounds.width() * scale) / 2.0;
        double offsetY = (screenSize.height() - worldBounds.height() * scale) / 2.0;
        
        // Create transformation matrix
        worldToScreen = QTransform::fromTranslate(offsetX, offsetY)
                      .scale(scale, scale)
                      .translate(-worldBounds.left(), -worldBounds.top());
    } else {
        // Stretch to fill widget
        worldToScreen = QTransform::fromTranslate(0, 0)
                      .scale(screenSize.width() / worldBounds.width(),
                             screenSize.height() / worldBounds.height())
                      .translate(-worldBounds.left(), -worldBounds.top());
    }
    
    // Create inverse transformation
    screenToWorld = worldToScreen.inverted();
}

QPointF DisplayTransform::mapToScreen(QPointF worldPoint) const {
    return worldToScreen.map(worldPoint);
}

QPointF DisplayTransform::mapToWorld(QPointF screenPoint) const {
    return screenToWorld.map(screenPoint);
}

double DisplayTransform::mapDistanceToScreen(double worldDistance) const {
    // Map a world distance to screen pixels
    QPointF worldPoint(0, 0);
    QPointF worldPoint2(worldDistance, 0);
    QPointF screenPoint1 = mapToScreen(worldPoint);
    QPointF screenPoint2 = mapToScreen(worldPoint2);
    return QLineF(screenPoint1, screenPoint2).length();
}

double DisplayTransform::mapDistanceToWorld(double screenDistance) const {
    // Map a screen distance to world units
    QPointF screenPoint(0, 0);
    QPointF screenPoint2(screenDistance, 0);
    QPointF worldPoint1 = mapToWorld(screenPoint);
    QPointF worldPoint2 = mapToWorld(screenPoint2);
    return QLineF(worldPoint1, worldPoint2).length();
}

// ===== TSA WIDGET IMPLEMENTATION =====

TSAWidget::TSAWidget(QWidget* parent)
    : QWidget(parent),
      timer(new QTimer(this)),
      simulationTime(0.0)
{
    // Initialize tactical data with default values
    tacticalData.ownShipPosition = QPointF(0, 0);
    tacticalData.ownShipBearing = 0.0;  // North
    tacticalData.ownShipSpeed = 10.0;    // 10 knots
    
    // Set up sonar beam
    tacticalData.sonarBeam = SonarBeam(
        QPointF(80, 480),  // Start point
        QPointF(720, 80),  // End point
        2.0,                // Width
        Qt::green,          // Color
        4                   // Line width
    );
    
    // Add default tactical vectors
    addTacticalVector(TacticalVector(
        QPointF(0, 0),                    // Origin (own ship)
        0.0,                              // Bearing (North)
        6.0,                              // Magnitude (6 nm)
        VectorType::OWN_SHIP,             // Type
        Qt::cyan,                         // Color (cyan for own ship heading)
        3,                                // Line width
        12.0,                             // Head length
        25.0                              // Head angle
    ));
    
    addTacticalVector(TacticalVector(
        QPointF(3, 3),                    // Origin (sensor position)
        225.0,                            // Bearing (SW)
        8.0,                              // Magnitude (8 nm)
        VectorType::ADOPTED_TRACK,        // Type
        Qt::red,                          // Color (red for adopted track)
        3,                                // Line width
        12.0,                             // Head length
        25.0                              // Head angle
    ));
    
    // Set up timer for simulation updates
    connect(timer, &QTimer::timeout, this, &TSAWidget::updateSimulation);
    timer->start(2000);  // 2 second intervals
    
    // Initialize transform
    transform.setWorldBounds(QRectF(-10, -10, 20, 20));
}

TSAWidget::~TSAWidget() {
    if (timer) {
        timer->stop();
    }
}

void TSAWidget::updateTacticalData(double ownShipBearing, double ownShipSpeed,
                                   const QVector<TacticalVector>& targetVectors,
                                   double sonarBearing, double bearingRate) {
    // Update own ship data
    tacticalData.ownShipBearing = ownShipBearing;
    tacticalData.ownShipSpeed = ownShipSpeed;
    
    // Update target data
    tacticalData.targetBearing = sonarBearing;
    tacticalData.bearingRate = bearingRate;
    
    // Replace target vectors
    tacticalData.vectors.clear();
    for (const auto& vector : targetVectors) {
        tacticalData.vectors.append(vector);
    }
    
    // Trigger repaint
    update();
}

void TSAWidget::updateOwnShip(double bearing, double speed) {
    tacticalData.ownShipBearing = bearing;
    tacticalData.ownShipSpeed = speed;
    update();
}

void TSAWidget::updateTarget(double bearing, double range, double bearingRate) {
    tacticalData.targetBearing = bearing;
    tacticalData.targetRange = range;
    tacticalData.bearingRate = bearingRate;
    update();
}

void TSAWidget::updateSonarBeam(double bearing, double width) {
    // Calculate new sonar beam endpoints based on bearing
    double beamLength = 10.0; // 10 nautical miles
    QPointF start = QPointF(0, 0);
    QPointF end = QPointF(
        beamLength * qSin(qDegreesToRadians(bearing)),
        -beamLength * qCos(qDegreesToRadians(bearing))
    );
    
    tacticalData.sonarBeam.startPoint = start;
    tacticalData.sonarBeam.endPoint = end;
    tacticalData.sonarBeam.width = width;
    update();
}

void TSAWidget::addTacticalVector(const TacticalVector& vector) {
    tacticalData.vectors.append(vector);
    update();
}

void TSAWidget::clearTacticalVectors() {
    tacticalData.vectors.clear();
    update();
}

void TSAWidget::setWorldBounds(QRectF bounds) {
    transform.setWorldBounds(bounds);
    update();
}

void TSAWidget::setSimulationInterval(int milliseconds) {
    if (timer) {
        timer->setInterval(milliseconds);
    }
}

void TSAWidget::startSimulation() {
    if (timer) {
        timer->start();
    }
}

void TSAWidget::stopSimulation() {
    if (timer) {
        timer->stop();
    }
}

void TSAWidget::updateSimulation() {
    simulationTime += 2.0; // 2 second intervals
    
    // Update target position based on time
    double t = simulationTime / 3600.0; // Convert to hours
    
    // Simulate target movement (East at 8 knots)
    double targetX = 3.0 + 8.0 * t;
    double targetY = 3.0;
    
    // Calculate new bearing and range
    double dx = targetX;
    double dy = targetY;
    double newRange = qSqrt(dx*dx + dy*dy);
    double newBearing = qRadiansToDegrees(qAtan2(dx, dy));
    
    // Update tactical data
    updateTarget(newBearing, newRange, 0.05); // Fixed bearing rate for demo
    
    // Debug output
    qDebug() << "Time:" << simulationTime
             << "Bearing:" << newBearing
             << "Range:" << newRange
             << "Rate:" << 0.05;
}

void TSAWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    
    // Update transform for new size
    transform.updateTransform(event->size(), transform.getWorldBounds(), true);
}

void TSAWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    
    // 1. BLACK background
    p.fillRect(rect(), Qt::black);
    
    // 2. Calculate bearing line from screen edge to ship position
    QPointF shipPos(width() * 0.75, height() * 0.25);     // Ship position
    
    // Calculate line from bottom-left corner to ship (like original)
    QPointF lineStart(0, height());                        // Bottom-left screen edge
    QPointF lineEnd = shipPos;                            // Ends at ship
    
    // 3. EXTEND the bearing line beyond the ship to create complete half-plane
    // Calculate direction vector from start to ship
    QPointF direction = lineEnd - lineStart;
    double length = QLineF(lineStart, lineEnd).length();
    
    // Extend the line to reach the top-right corner or beyond
    QPointF extendedEnd = lineStart + direction * (width() + height()) / length;
    
    // 4. Calculate sensor position along the bearing line (like original)
    QPointF sensorPos = lineStart + 0.45 * (lineEnd - lineStart);  // 45% along line
    
    // 5. Draw shaded region on ONE SIDE using EXTENDED line (like original)
    drawOneSidedShadedRegion(p, lineStart, extendedEnd, shipPos);
    
    // 6. Draw GREEN bearing line from edge to ship (like original)
    p.setPen(QPen(Qt::green, 4, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(lineStart, lineEnd);
    
    // 7. Draw YELLOW ship marker
    p.setBrush(Qt::yellow);
    p.setPen(Qt::NoPen);
    p.drawEllipse(shipPos, 6, 6);
    
    // 8. Draw RED sensor marker
    p.setBrush(Qt::red);
    p.setPen(Qt::NoPen);
    p.drawEllipse(sensorPos, 6, 6);
    
    // 9. Draw CYAN own ship vector FROM bearing line (like original)
    drawSimpleArrow(p, shipPos, shipPos + QPointF(0, -60), Qt::cyan, 3);
    
    // 10. Draw RED target vector FROM sensor position on bearing line
    drawSimpleArrow(p, sensorPos, sensorPos + QPointF(80, -80), Qt::red, 3);
}

QPointF TSAWidget::calculateVectorEnd(const TacticalVector& vector) const {
    double angle = qDegreesToRadians(vector.bearing);
    return vector.origin + QPointF(
        vector.magnitude * qSin(angle),
        -vector.magnitude * qCos(angle)
    );
}

QPointF TSAWidget::getOwnShipScreenPosition() const {
    return transform.mapToScreen(tacticalData.ownShipPosition);
}

QPointF TSAWidget::getTargetScreenPosition() const {
    // Calculate target position based on bearing and range
    double angle = qDegreesToRadians(tacticalData.targetBearing);
    QPointF targetWorld = QPointF(
        tacticalData.targetRange * qSin(angle),
        -tacticalData.targetRange * qCos(angle)
    );
    return transform.mapToScreen(targetWorld);
}

// NEW METHOD: Draw one-sided shaded region (like original TSA)
void TSAWidget::drawOneSidedShadedRegion(QPainter& p, QPointF lineStart, QPointF lineEnd, QPointF shipPos) {
    QVector<QPointF> corners = {
        QPointF(0,0),
        QPointF(width(),0),
        QPointF(width(),height()),
        QPointF(0,height())
    };

    auto sideTest = [&](const QPointF& pt) {
        QPointF v1 = lineEnd - lineStart;
        QPointF v2 = pt - lineStart;
        return v1.x() * v2.y() - v1.y() * v2.x();
    };

    // Ship is ON the "reference" side. To shade OPPOSITE, explicitly flip the logic:
    bool shipOnLeft = sideTest(shipPos) > 0;
    bool shadeLeft = !shipOnLeft;  // FLIP here!

    // Create offset line for shading (1/2 inch gap from bearing line)
    // Convert 1/2 inch to pixels (assuming 96 DPI = 1 inch = 96 pixels)
    double halfInchPixels = 48.0;  // 96 DPI / 2
    
    // Calculate perpendicular vector for offset
    QPointF lineVector = lineEnd - lineStart;
    double lineLength = QLineF(lineStart, lineEnd).length();
    QPointF perpVector(-lineVector.y(), lineVector.x());  // 90 degree rotation
    perpVector = perpVector / lineLength;  // Normalize
    
    // Offset the line away from the ship side
    QPointF offsetDirection = (shadeLeft ? perpVector : -perpVector);
    QPointF offsetStart = lineStart + offsetDirection * halfInchPixels;
    QPointF offsetEnd = lineEnd + offsetDirection * halfInchPixels;

    // Build polygon with ALL corners on the shaded side, then add offset line
    QPolygonF shadePoly;
    
    // Add all corners on the shaded side
    for (const QPointF& c : corners) {
        if ((sideTest(c) > 0) == shadeLeft) {
            shadePoly << c;
        }
    }
    
    // Add offset line points to close the polygon
    // Order matters: add offsetEnd first, then offsetStart to connect properly
    shadePoly << offsetEnd;
    shadePoly << offsetStart;
    
    // Ensure we have enough points for a valid polygon
    if (shadePoly.size() >= 3) {
        // Draw shaded area
        p.setBrush(QBrush(QColor(80,80,80,150), Qt::BDiagPattern));
        p.setPen(Qt::NoPen);
        p.drawPolygon(shadePoly);

        // White outline
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(Qt::white, 2, Qt::SolidLine));
        p.drawPolygon(shadePoly);
    }
}

void TSAWidget::drawSimpleArrow(QPainter& p, QPointF from, QPointF to, QColor color, int width) {
    // Draw arrow shaft
    p.setPen(QPen(color, width, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(from, to);
    
    // Draw small arrow head (not large polygon)
    double angle = qAtan2(to.y() - from.y(), to.x() - from.x());
    double headLen = 12.0;
    double headAngle = qDegreesToRadians(25.0);
    
    QPointF h1(to.x() + headLen * qCos(angle + M_PI - headAngle),
               to.y() + headLen * qSin(angle + M_PI - headAngle));
    QPointF h2(to.x() + headLen * qCos(angle + M_PI + headAngle),
               to.y() + headLen * qSin(angle + M_PI + headAngle));
    
    QPolygonF head;
    head << to << h1 << h2;
    
    p.setBrush(color);
    p.setPen(Qt::NoPen);
    p.drawPolygon(head);
} 
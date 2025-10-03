#include "drawingwidget.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QFontMetrics>
#include <algorithm>
#include <chrono>
#include <set>

DrawingWidget::DrawingWidget(QWidget *parent)
    : QWidget(parent), showResult(false)
{
    setStyleSheet("background-color: white; border: 1px solid black;");
    setMouseTracking(false);
}

void DrawingWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawPoints(painter);

    if (showResult && !currentResult.hull.isEmpty()) {
        drawConvexHull(painter);
    }

    if (showResult) {
        drawStatistics(painter);
    }
}

void DrawingWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        Point2D newPoint(event->pos());
        points.append(newPoint);
        showResult = false;
        currentResult = AlgorithmResult();
        update();
    }
}

void DrawingWidget::drawPoints(QPainter& painter)
{
    QPen pointPen(Qt::blue, 2);
    QBrush pointBrush(Qt::blue);
    painter.setPen(pointPen);
    painter.setBrush(pointBrush);

    for (const Point2D& point : points) {
        painter.drawEllipse(point.toQPoint(), 4, 4);
    }
}

void DrawingWidget::drawConvexHull(QPainter& painter)
{
    if (currentResult.hull.size() < 3) return;

    QPen hullPen(Qt::red, 3);
    painter.setPen(hullPen);
    painter.setBrush(Qt::NoBrush);

    // Draw hull edges
    for (int i = 0; i < currentResult.hull.size(); ++i) {
        int next = (i + 1) % currentResult.hull.size();
        painter.drawLine(currentResult.hull[i].toQPoint(), currentResult.hull[next].toQPoint());
    }

    // Highlight hull vertices
    QPen vertexPen(Qt::red, 2);
    QBrush vertexBrush(Qt::red);
    painter.setPen(vertexPen);
    painter.setBrush(vertexBrush);

    for (const Point2D& vertex : currentResult.hull) {
        painter.drawEllipse(vertex.toQPoint(), 6, 6);
    }
}

void DrawingWidget::drawStatistics(QPainter& painter)
{
    QFont font("Arial", 12, QFont::Bold);
    painter.setFont(font);
    painter.setPen(Qt::white);

    QFontMetrics metrics(font);
    int lineHeight = metrics.height() + 5;
    int x = 10;
    int y = 20;

    QStringList lines;
    lines << QString("Algorithm: %1").arg(currentResult.algorithmName);
    lines << QString("Points: %1").arg(points.size());
    lines << QString("Hull Vertices: %1").arg(currentResult.hull.size());
    lines << QString("Iterations: %1").arg(currentResult.iterations);

    int maxWidth = 0;
    for (const QString& line : lines) {
        maxWidth = qMax(maxWidth, metrics.horizontalAdvance(line));
    }

    for (const QString& line : lines) {
        painter.drawText(x, y, line);
        y += lineHeight;
    }
}

void DrawingWidget::clearPoints()
{
    points.clear();
    currentResult = AlgorithmResult();
    showResult = false;
    update();
}

void DrawingWidget::runSlowConvexHull()
{
    if (points.size() < 3) {
        showResult = false;
        update();
        return;
    }

    currentResult = computeSlowConvexHull();
    showResult = true;
    update();
}

void DrawingWidget::runFastConvexHull()
{
    if (points.size() < 3) {
        showResult = false;
        update();
        return;
    }

    currentResult = computeFastConvexHull();
    showResult = true;
    update();
}

AlgorithmResult DrawingWidget::computeSlowConvexHull()
{
    auto start = std::chrono::high_resolution_clock::now();

    AlgorithmResult result;
    result.algorithmName = "Slow Convex Hull";

    // Calculate theoretical maximum iterations
    int n = points.size();
    result.iterations = n * (n - 1) * (n - 2);

    QVector<QPair<Point2D, Point2D>> validEdges;

    // the algorithm
    for (int i = 0; i < points.size(); ++i) {
        for (int j = 0; j < points.size(); ++j) {
            if (i == j) continue;

            bool valid = true;
            Point2D p = points[i];
            Point2D q = points[j];

            for (int k = 0; k < points.size(); ++k) {
                if (k == i || k == j) continue;

                Point2D r = points[k];

                if (isLeftOfLine(r, p, q)) {
                    valid = false;
                    break;
                }
            }

            if (valid) {
                validEdges.append(qMakePair(p, q));
            }
        }
    }

    constructHullFromEdges(validEdges, result.hull);

    return result;
}

bool DrawingWidget::isLeftOfLine(const Point2D& point, const Point2D& lineStart, const Point2D& lineEnd)
{
    // Calculate the cross product to determine orientation
    // Positive means left, negative means right, zero means collinear
    long long cross = (long long)(lineEnd.x - lineStart.x) * (point.y - lineStart.y) -
                      (long long)(lineEnd.y - lineStart.y) * (point.x - lineStart.x);
    return cross > 0;
}

void DrawingWidget::constructHullFromEdges(const QVector<QPair<Point2D, Point2D>>& edges, QVector<Point2D>& hull)
{
    if (edges.isEmpty()) return;

    // Use a set to collect unique vertices
    std::set<std::pair<int, int>> vertexSet;
    for (const auto& edge : edges) {
        vertexSet.insert({edge.first.x, edge.first.y});
        vertexSet.insert({edge.second.x, edge.second.y});
    }

    // Convert back to Point2D and sort by angle around centroid
    QVector<Point2D> vertices;
    for (const auto& vertex : vertexSet) {
        vertices.append(Point2D(vertex.first, vertex.second));
    }

    if (vertices.size() < 3) return;

    // Find centroid
    double cx = 0, cy = 0;
    for (const Point2D& v : vertices) {
        cx += v.x;
        cy += v.y;
    }
    cx /= vertices.size();
    cy /= vertices.size();

    // Sort vertices by polar angle around centroid
    std::sort(vertices.begin(), vertices.end(), [cx, cy](const Point2D& a, const Point2D& b) {
        double angleA = atan2(a.y - cy, a.x - cx);
        double angleB = atan2(b.y - cy, b.x - cx);
        return angleA < angleB;
    });

    hull = vertices;
}

AlgorithmResult DrawingWidget::computeFastConvexHull()
{
    auto start = std::chrono::high_resolution_clock::now();

    AlgorithmResult result;
    result.algorithmName = "Fast Convex Hull";
    result.iterations = 0;

    QVector<Point2D> sortedPoints = points;
    std::sort(sortedPoints.begin(), sortedPoints.end());

    int n = sortedPoints.size();

    // Build upper hull
    QVector<Point2D> upper;
    if (n >= 1) upper.append(sortedPoints[0]);
    if (n >= 2) upper.append(sortedPoints[1]);

    // Step 3-6: for i ← 3 to n
    for (int i = 2; i < n; ++i) {
        result.iterations++; // Count main loop iteration

        // Append pi to Lupper
        upper.append(sortedPoints[i]);

        // While loop - count EACH removal operation
        while (upper.size() > 2 &&
               cross(upper[upper.size()-3], upper[upper.size()-2], upper[upper.size()-1]) <= 0) {
            result.iterations++; // COUNT WHILE LOOP ITERATIONS TOO!
            upper.removeAt(upper.size() - 2);
        }
    }

    // Build lower hull
    QVector<Point2D> lower;
    if (n >= 1) lower.append(sortedPoints[n-1]);
    if (n >= 2) lower.append(sortedPoints[n-2]);

    // Step 8-11: for i ← n-2 downto 1
    for (int i = n - 3; i >= 0; --i) {
        result.iterations++; // Count main loop iteration

        // Append pi to Llower
        lower.append(sortedPoints[i]);

        // While loop - count EACH removal operation
        while (lower.size() > 2 &&
               cross(lower[lower.size()-3], lower[lower.size()-2], lower[lower.size()-1]) <= 0) {
            result.iterations++; // COUNT WHILE LOOP ITERATIONS TOO!
            lower.removeAt(lower.size() - 2);
        }
    }

    // Remove duplicates and concatenate
    if (lower.size() > 0) lower.removeFirst();
    if (lower.size() > 0) lower.removeLast();

    result.hull = upper;
    result.hull.append(lower);

    return result;
}


int DrawingWidget::cross(const Point2D& O, const Point2D& A, const Point2D& B)
{
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}
